////////////////////////////////////////////////////////////////////////////////
//
// Adding a camera and moving to the 3rd dimension.
// A "Hello 3D World" of Modern OpenGL.
//
// Copyright (c) 2013-23 by Carlos Martinho
//
// INTRODUCES:
// VIEW PIPELINE, UNIFORM BUFFER OBJECTS, mglCamera.hpp
//
////////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "../mgl/mgl.hpp"

////////////////////////////////////////////////////////////////////////// MYAPP

class MyApp : public mgl::App {
 public:
  void initCallback(GLFWwindow *win) override;
  void displayCallback(GLFWwindow *win, double elapsed) override;
  void windowCloseCallback(GLFWwindow *win) override;
  void windowSizeCallback(GLFWwindow *win, int width, int height) override;
  void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods) override;
  void cursorCallback(GLFWwindow* win, double xpos, double ypos) override;
  void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods) override;
  void scrollCallback(GLFWwindow* win, double xoffset, double yoffset) override;

 private:
  const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0;
  GLuint VaoId;

  mgl::ShaderProgram *Shaders = nullptr;
  mgl::OrbitCamera* OrbitCamera = nullptr;
  GLint ModelMatrixId;
  GLint NormalMatrixId;
  GLint ColorId;
  mgl::SceneGraph* SceneGraph = nullptr;

  mgl::Texture2D* BaseTexture = nullptr;
  mgl::Texture3D* BaseTexture3D = nullptr;
  mgl::Texture3D* FloatingTexture3D = nullptr;
  mgl::NearestSampler* BaseSampler = nullptr;
  mgl::TextureInfo* BaseTextureInfo = nullptr;
  mgl::TextureInfo* FloatingTextureInfo = nullptr;

  double previousMousePositionX = 0;
  double previousMousePositionY = 0;

  void createMeshes();
  void createMesh(std::string name, std::string meshFile);
  void createTextures();
  void createTexture(std::string name, mgl::Texture3D::Type type);
  void createShaderPrograms();
  void createShaderProgram(std::string shaderName, std::string vsFile, std::string fsFile);
  void createCameras();
  void createScene();
  mgl::SceneNode* createNode(mgl::Mesh* mesh, glm::vec3 position, glm::quat rotation, glm::vec3 scale, mgl::SceneNode* parent, mgl::ShaderProgram* shader);
  void drawScene(double elapsed);
  void processAnimation(double elapsed);
};

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (mgl::KeyState::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {

        double xMoved = xpos - previousMousePositionX;
        double yMoved = ypos - previousMousePositionY;
        
        double anglex = xMoved * 20;
        double angley = yMoved * 20;
        float yawAngle = glm::radians(anglex);
        float pitchAngle = glm::radians(angley);

        OrbitCamera->addYaw(yawAngle);
        OrbitCamera->addPitch(pitchAngle);
    }
    previousMousePositionX = xpos;
    previousMousePositionY = ypos;
}

void MyApp::mouseButtonCallback(GLFWwindow* win, int button, int action,
    int mods) {
    mgl::KeyState::getInstance().updateMouseButtonState(button, action);
}

void MyApp::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    OrbitCamera->addZoom(yoffset);
}

void MyApp::keyCallback(GLFWwindow* win, int key, int scancode, int action,
    int mods) {
    mgl::KeyState::getInstance().updateKeyState(key, action);

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_P)) {
        OrbitCamera->switchProjection();
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
        windowCloseCallback(win);
    }
}

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_dir = "../assets/";
    std::string base_mesh_file = "floating_wood_base.obj";
    std::string floating_obj_mesh_file = "floating_top_marble.obj";

    std::string base_mesh_fullname = mesh_dir + base_mesh_file /*"../04-assets/models/cube-vtn.obj"*/;
    std::string floating_obj_mesh_fullname = mesh_dir + floating_obj_mesh_file;

    createMesh("baseMesh", base_mesh_fullname);
    createMesh("floatingMesh", floating_obj_mesh_fullname);
}

void MyApp::createMesh(std::string name, std::string meshFile) {
    mgl::Mesh* mesh = new mgl::Mesh();
    mesh->joinIdenticalVertices();
    mesh->create(meshFile);

    mgl::MeshManager::getInstance().add(name, mesh);
}

void MyApp::createTextures() {
    BaseSampler = new mgl::NearestSampler();
    BaseSampler->create();

    createTexture("baseTexture3D", mgl::Texture3D::WOOD);
    createTexture("floatingTexture3D", mgl::Texture3D::MARBLE);
}

void MyApp::createTexture(std::string name, mgl::Texture3D::Type type) {
    mgl::Texture3D* Texture3D = new mgl::Texture3D();
    Texture3D->generatePerlinNoiseTexture3(256, 256, 256, type);
    mgl::TextureManager::getInstance().add(name, Texture3D);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    createShaderProgram("woodShader", "wood-vs.glsl", "wood-fs.glsl");
    createShaderProgram("marbleShader", "marble-vs.glsl", "marble-fs.glsl");
}

void MyApp::createShaderProgram(std::string shaderName, std::string vsFile, std::string fsFile) {
    mgl::Mesh* BaseMesh = mgl::MeshManager::getInstance().get("baseMesh");
    mgl::Mesh* FloatingMesh = mgl::MeshManager::getInstance().get("floatingMesh");

    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, vsFile);
    Shaders->addShader(GL_FRAGMENT_SHADER, fsFile);

    Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    if (BaseMesh->hasNormals() && FloatingMesh->hasNormals()) {
        Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
    }
    if (BaseMesh->hasTexcoords() && FloatingMesh->hasTexcoords()) {
        Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
    }
    if (BaseMesh->hasTangentsAndBitangents() && FloatingMesh->hasTangentsAndBitangents()) {
        Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
    }

    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniform(mgl::NORMAL_MATRIX);
    Shaders->addUniform(mgl::TEXTURE);
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    Shaders->create();

    mgl::ShaderManager::getInstance().add(shaderName, Shaders);

    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
    NormalMatrixId = Shaders->Uniforms[mgl::NORMAL_MATRIX].index;
    ColorId = Shaders->Uniforms[mgl::PRIMARY_COLOR_UNIFORM].index;
}

// common transformation matrixes
// scale
const glm::mat4 scaleUp = glm::scale(glm::vec3(1.416f, 1.0f, 1.416f));

// model matrixes
const glm::mat4 squareMM = glm::translate(glm::vec3(-2.0f * 1.416f * 1.416f, 0.0f, 0.0f)) * glm::rotate(glm::radians(135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 paralelogramMM = glm::rotate(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 cyanMM = glm::translate(glm::vec3(-2.0f, 0.0f, 0.0f)) * glm::translate(glm::vec3(-2.0f * 1.416f * 1.416f, 0.0f, 2.0f * 1.416f * 1.416f));
const glm::mat4 orangeMM = glm::translate(glm::vec3(-2.0f * 1.416f * 1.416f, 0.0f, -1.0f * 1.416f * 1.416f)) * glm::rotate(glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 purpleMM = scaleUp;
const glm::mat4 blueMM = scaleUp * scaleUp * glm::translate(glm::vec3(-2.0f, 0.0f, -2.0f)) * glm::rotate(glm::radians(-90.f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::mat4 magentaMM = scaleUp * scaleUp * glm::rotate(glm::radians(180.f), glm::vec3(0.0f, 1.0f, 0.0f));

// colors
const glm::vec4 purpleColor = { 0.451f, 0.247f, 0.800f, 1.0f };
const glm::vec4 blueColor = { 0.0f, 0.0f, 1.0f, 1.0f };
const glm::vec4 magentaColor = { 0.808f, 0.172f, 0.416f, 1.0f };
const glm::vec4 aquaColor = { 0.0f, 0.6f, 0.6f, 1.0f };
const glm::vec4 orangeColor = { 1.0f, 0.25f, 0.25f, 1.0f };
const glm::vec4 paralelogramColor = { 1.0f, 0.608f, 0.153f, 1.0f };
const glm::vec4 squareColor = { 0.0f, 0.6f, 0.0f, 1.0f };

const glm::vec4 basePrimaryColor = { 0.62745f, 0.52941f, 0.52549f, 1.0f};
const glm::vec4 baseSecondaryColor = { 0.35686f, 0.30980f, 0.30980f, 1.0f};

const glm::vec4 floatingPrimaryColor = {1.0f, 1.0f, 1.0f, 1.0f};
const glm::vec4 floatingSecondaryColor = { 0.2f, 0.2f, 0.2f, 1.0f };

// animation
// animation axis rotation
const glm::quat zAxisRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

// animation box rotation
const glm::quat zeroBoxRotation = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::quat squareBoxRotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::quat paralelogramBoxRotation = glm::angleAxis(glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::quat cyanBoxRotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::quat orangeBoxRotation = glm::angleAxis(glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
const glm::quat purpleBoxRotation = glm::angleAxis(glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// animation box translation
const glm::vec3 paralelogramBoxTranslation = glm::vec3(-4.0f, 0.0f, -2.0f);
const glm::vec3 cyanBoxTranslation = glm::vec3(2.0f, 0.0f, 0.0f);
const glm::vec3 orangeBoxTranslation = glm::vec3(-2.0f, 0.0f, 2.0f);
const glm::vec3 purpleBoxTranslation = glm::vec3(-6.0f, 0.0f, -2.0f);

// animation translation up
const glm::vec3 translationUp = glm::vec3(-4.0f, 0.0f, 0.0f);


const glm::mat4 baseMM = glm::mat4(1.0f);
const glm::mat4 floatingMM = glm::translate(glm::vec3(0.0f, 1.3f, 0.0f)) * glm::scale(glm::vec3(1.0f, 1.3f, 1.0f));

const glm::vec3 basePosition = { 0.0f, 0.0f, 0.0f };
const glm::quat baseRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
const glm::vec3 baseScale = { 1.0f, 1.0f, 1.0f };
const glm::vec3 floatingPosition = { 0.0f, 1.3f, 0.0f };
const glm::quat floatingRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
const glm::vec3 floatingScale = { 1.0f, 1.3f, 1.0f };


void MyApp::createScene() {
    SceneGraph = new mgl::SceneGraph();
    mgl::SceneNode* root = new mgl::SceneNode(ModelMatrixId, NormalMatrixId, ColorId);

    SceneGraph->addRoot(root);

    // square
    mgl::SillouetteCallBack* callback = new mgl::SillouetteCallBack();
    mgl::SceneNode* base = createNode(mgl::MeshManager::getInstance().get("baseMesh"), basePosition, baseRotation, baseScale, root, mgl::ShaderManager::getInstance().get("woodShader"));
    BaseTextureInfo = new mgl::TextureInfo(GL_TEXTURE0, GL_TEXTURE0, mgl::TEXTURE, mgl::TextureManager::getInstance().get("baseTexture3D"), BaseSampler);
    base->setTextureInfo(BaseTextureInfo);

    //mgl::SceneNode* baseSillouette = createNode(BaseMesh, basePosition, baseRotation, { 1.01f, 1.01f, 1.01f }, orangeColor, root);
    //baseSillouette->setCallBack(callback);

    // paralelogram
    mgl::SceneNode* floating = createNode(mgl::MeshManager::getInstance().get("floatingMesh"), floatingPosition, floatingRotation, floatingScale, base, mgl::ShaderManager::getInstance().get("marbleShader"));
    FloatingTextureInfo = new mgl::TextureInfo(GL_TEXTURE0, GL_TEXTURE0, mgl::TEXTURE, mgl::TextureManager::getInstance().get("floatingTexture3D"), BaseSampler);
    floating->setTextureInfo(FloatingTextureInfo);
    //mgl::SceneNode* floatingSillouette = createNode(FloatingMesh, basePosition, baseRotation, { 1.01f, 1.01f, 1.01f }, orangeColor, floating);
    //floatingSillouette->setCallBack(callback);
}

mgl::SceneNode* MyApp::createNode(mgl::Mesh *mesh, glm::vec3 position, glm::quat rotation, glm::vec3 scale, mgl::SceneNode* parent, mgl::ShaderProgram* shader) {

    mgl::SceneNode* node = new mgl::SceneNode(ModelMatrixId, NormalMatrixId, ColorId);

    //We are creating a root node (only happens once)
    node->setMesh(mesh);
    node->setPosition(position);
    node->setRotation(rotation);
    node->setScale(scale);
    node->setShaderProgram(shader);
    //SceneGraph->addRoot(node);
    parent->addChild(node);

    return node;
}

////////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::createCameras() {
    OrbitCamera = new mgl::OrbitCamera(UBO_BP, true, glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    SceneGraph->setCamera(OrbitCamera);
}

////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 ModelMatrix(1.0f);

void MyApp::drawScene(double elapsed) {
    SceneGraph->renderScene(elapsed);
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
    createMeshes();
    createTextures();
    createShaderPrograms();  // after mesh;
    createScene();
    createCameras();
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
  OrbitCamera->updatePerspectiveProjectionMatrix(winx, winy);
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
    processAnimation(elapsed);
    drawScene(elapsed); 
}
void MyApp::windowCloseCallback(GLFWwindow* win) {
    delete OrbitCamera;
    delete Shaders;
    delete SceneGraph;
}

void MyApp::processAnimation(double elapsed) {
    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_LEFT)) {
        //SceneGraph->moveToBox(elapsed);
    }
    else if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) {
        //SceneGraph->moveToShape(elapsed);
    }
}

/////////////////////////////////////////////////////////////////////////// MAIN

int main(int argc, char *argv[]) {
  mgl::Engine &engine = mgl::Engine::getInstance();
  engine.setApp(new MyApp());
  engine.setOpenGL(4, 6);
  engine.setWindow(800, 600, "Hello Modern 3D World", 0, 1);
  engine.init();
  engine.run();
  exit(EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
