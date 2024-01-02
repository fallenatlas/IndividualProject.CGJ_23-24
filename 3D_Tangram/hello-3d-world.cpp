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
  const GLuint POSITION = 0, COLOR = 1, UBO_BP = 0, UBO_BP_LIGHT = 1;
  GLuint VaoId;

  mgl::OrbitCamera* OrbitCamera = nullptr;
  mgl::SceneGraph* SceneGraph = nullptr;

  mgl::NearestSampler* BaseSampler = nullptr;

  static constexpr std::uint8_t backgroundIndex = 0xFF;
  std::uint8_t hoveredIndex = backgroundIndex;
  std::uint8_t previousSelectedIndex = backgroundIndex;

  double previousMousePositionX = 0;
  double previousMousePositionY = 0;

  void activateStencilBuffer();
  void createMeshes();
  void createMesh(std::string name, std::string meshFile);
  void createTextures();
  void createTexture3D(std::string name, mgl::Texture3D::Type type);
  void createShaderPrograms();
  void createShaderProgram(std::string shaderName, std::string vsFile, std::string fsFile, bool sillouette);
  void createCallBacks();
  void createSillouetteInfos();
  void createSillouetteInfo(std::string name, glm::vec3 scale);
  void createCameras();
  void createScene();
  mgl::SceneNode* createNode(int nodeId, std::string mesh, glm::vec3 position, glm::quat rotation, glm::vec3 scale, mgl::SceneNode* parent, std::string shader);
  void drawScene(double elapsed);
  void processAnimation(double elapsed);
  void handleObjectMovement(double xpos, double ypos);
  void handleObjectRotation();
  void handleAxisObjectMovement(mgl::SceneNode* node, int key, glm::vec3 nodeDirection, glm::vec3 direction, float xMoved, float yMoved);
};

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (mgl::KeyState::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
        handleObjectMovement(xpos, ypos);
    }

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

void MyApp::handleObjectMovement(double xpos, double ypos) {
    mgl::SceneNode* node = SceneGraph->getNode(static_cast<int>(previousSelectedIndex));
    if (node == nullptr) return;

    glm::vec3 xDirection = { 1.0f, 0.0f, 0.0f };
    glm::vec3 yDirection = { 0.0f, 1.0f, 0.0f };
    glm::vec3 zDirection = { 0.0f, 0.0f, 1.0f };
    glm::quat gloabalNodeRotation = node->getGlobalRotation();

    glm::vec3 nodeForward = glm::vec3(glm::mat4(gloabalNodeRotation) * glm::vec4(zDirection, 1.0f));
    glm::vec3 nodeUp = glm::vec3(glm::mat4(gloabalNodeRotation) * glm::vec4(yDirection, 1.0f));
    glm::vec3 nodeSide = glm::vec3(glm::mat4(gloabalNodeRotation) * glm::vec4(xDirection, 1.0f));

    float xMoved = xpos - previousMousePositionX;
    float yMoved = ypos - previousMousePositionY;

    handleAxisObjectMovement(node, GLFW_KEY_C, nodeUp, yDirection, xMoved, yMoved);
    handleAxisObjectMovement(node, GLFW_KEY_X, nodeSide, xDirection, xMoved, yMoved);
    handleAxisObjectMovement(node, GLFW_KEY_Z, nodeForward, zDirection, xMoved, yMoved);
}

void MyApp::handleAxisObjectMovement(mgl::SceneNode* node, int key, glm::vec3 nodeDirection, glm::vec3 direction, float xMoved, float yMoved) {
    if (!mgl::KeyState::getInstance().isKeyPressed(key)) return;

    glm::quat nodeRotation = node->getRotation();

    // camera vectors
    glm::vec3 up = SceneGraph->getCamera()->getUpVector();
    glm::vec3 side = SceneGraph->getCamera()->getSideVector();

    float xMultiplier = -glm::dot(nodeDirection, side);
    float yMultiplier = -glm::dot(nodeDirection, up);

    glm::vec3 movement = (direction * yMoved * yMultiplier + direction * xMoved * xMultiplier);

    glm::vec3 directedMovement = glm::vec3(glm::mat4(nodeRotation) * glm::vec4(movement, 1.0f));
    node->addFrameMovement(directedMovement);
}

void MyApp::mouseButtonCallback(GLFWwindow* win, int button, int action,
    int mods) {
    mgl::KeyState::getInstance().updateMouseButtonState(button, action);

    double xpos, ypos;
    glfwGetCursorPos(win, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(win, &width, &height);
    const glm::vec2 openglCursorPosition = { xpos, height - ypos };

    glReadPixels(static_cast<GLint>(openglCursorPosition.x), static_cast<GLint>(openglCursorPosition.y), 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &hoveredIndex);

    if (mgl::KeyState::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
        //std::cout << "in mouse:" << static_cast<int>(previousSelectedIndex) << " " << static_cast<int>(hoveredIndex) << std::endl;
        mgl::SceneNode* previousNode = SceneGraph->getNode(static_cast<int>(previousSelectedIndex));
        if (previousNode != nullptr) previousNode->isSelected(false);

        if (hoveredIndex != backgroundIndex) {
            mgl::SceneNode* node = SceneGraph->getNode(static_cast<int>(hoveredIndex));
            if (node != nullptr) node->isSelected(true);
        }

        previousSelectedIndex = hoveredIndex;
    }
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

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_K)) {
        SceneGraph->serialize();
    }

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_L)) {
        SceneGraph->deserialize();
    }

    handleObjectRotation();

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
        windowCloseCallback(win);
    }
}

void MyApp::handleObjectRotation() {
    mgl::SceneNode* node = SceneGraph->getNode(static_cast<int>(previousSelectedIndex));
    if (node == nullptr) return;

    glm::quat nodeRotation = node->getRotation();
    glm::vec3 nodeUp = glm::vec3(glm::mat4(nodeRotation) * glm::vec4(glm::vec3(0.0f, 1.0f, 0.0f), 1.0f));
    glm::vec3 nodeSide = glm::vec3(glm::mat4(nodeRotation) * glm::vec4(glm::vec3(1.0f, 0.0f, 0.0f), 1.0f));

    float pitch = 0;
    float yaw = 0;

    double anglex = 60;
    double angley = 60;

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_LEFT)) {
        float yawAngle = glm::radians(-anglex);
        yaw += yawAngle;
    }

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) {
        float yawAngle = glm::radians(anglex);
        yaw += yawAngle;
    }

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_DOWN)) {
        float pitchAngle = glm::radians(angley);
        pitch += pitchAngle;
    }

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_UP)) {
        float pitchAngle = glm::radians(-angley);
        pitch += pitchAngle;
    }

    //compute pitch quaternion
    glm::quat pitchQuat = glm::angleAxis(pitch, nodeSide);

    //compute yaw quaternion
    glm::quat yawQuat = glm::angleAxis(yaw, nodeUp);

    //compute total rotation
    glm::quat rotQuat = pitchQuat * yawQuat;
    node->addFrameRotation(rotQuat);
}

///////////////////////////////////////////////////////////////////////// STENCIL BUFFER


void MyApp::activateStencilBuffer() {
    // Stencil for mouse picking
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);
    glClearStencil(backgroundIndex);
}

///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_dir = "../assets/";
    std::string base_mesh_file = "floating_wood_base_centered.obj";
    std::string floating_obj_mesh_file = "floating_top_marble_smooth.obj";
    std::string cube_mesh_file = "cube-vtn.obj";

    std::string cube_mesh_fullname = mesh_dir + cube_mesh_file;
    std::string base_mesh_fullname = mesh_dir + base_mesh_file /*"../04-assets/models/cube-vtn.obj"*/;
    std::string floating_obj_mesh_fullname = mesh_dir + floating_obj_mesh_file;

    createMesh("cubeMesh", cube_mesh_fullname);
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

    createTexture3D("baseTexture3D", mgl::Texture3D::WOOD);
    createTexture3D("floatingTexture3D", mgl::Texture3D::MARBLE);
}

void MyApp::createTexture3D(std::string name, mgl::Texture3D::Type type) {
    mgl::Texture3D* Texture3D = new mgl::Texture3D();

    // If the app is failing to load due to memory or its taking too long to start, reduce the texture resolution
    Texture3D->generatePerlinNoiseTexture(256, 256, 256, type);
    //Texture3D->generatePerlinNoiseTexture(32, 32, 32, type);

    mgl::TextureInfo* TextureInfo = new mgl::TextureInfo(GL_TEXTURE0, GL_TEXTURE0, mgl::TEXTURE, Texture3D, BaseSampler);
    mgl::TextureInfoManager::getInstance().add(name, TextureInfo);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    createShaderProgram("woodShader", "wood-vs.glsl", "wood-fs.glsl", false);
    createShaderProgram("marbleShader", "marble-vs.glsl", "marble-fs.glsl", false);
    createShaderProgram("sillouetteShader", "color-vs.glsl", "color-fs.glsl", true);
    createShaderProgram("lightShader", "light-vs.glsl", "light-fs.glsl", true);
}

void MyApp::createShaderProgram(std::string shaderName, std::string vsFile, std::string fsFile, bool sillouette) {
    mgl::Mesh* BaseMesh = mgl::MeshManager::getInstance().get("baseMesh");
    mgl::Mesh* FloatingMesh = mgl::MeshManager::getInstance().get("floatingMesh");

    mgl::ShaderProgram* Shader = new mgl::ShaderProgram();
    Shader->addShader(GL_VERTEX_SHADER, vsFile);
    Shader->addShader(GL_FRAGMENT_SHADER, fsFile);

    Shader->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);

    if (!sillouette) {
        if (BaseMesh->hasNormals() && FloatingMesh->hasNormals()) {
            Shader->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
        }
        if (BaseMesh->hasTexcoords() && FloatingMesh->hasTexcoords()) {
            Shader->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
        }
        if (BaseMesh->hasTangentsAndBitangents() && FloatingMesh->hasTangentsAndBitangents()) {
            Shader->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
        }

        Shader->addUniform(mgl::NORMAL_MATRIX);
        Shader->addUniform(mgl::TEXTURE);
    }

    Shader->addUniform(mgl::MODEL_MATRIX);
    Shader->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    Shader->addUniformBlock(mgl::LIGHT_BLOCK, UBO_BP_LIGHT);
    Shader->create();

    mgl::ShaderManager::getInstance().add(shaderName, Shader);
}

void MyApp::createCallBacks() {
    mgl::SillouetteCallBack* callback = new mgl::SillouetteCallBack();
    mgl::CallbackManager::getInstance().add("sillouetteCallback", callback);

    mgl::StencilCallBack* stencilCallback = new mgl::StencilCallBack();
    mgl::CallbackManager::getInstance().add("stencilCallback", stencilCallback);
}

void MyApp::createSillouetteInfos() {
    createSillouetteInfo("defaultSillouetteInfo", glm::vec3(1.05f, 1.05f, 1.05f));
    createSillouetteInfo("baseSillouetteInfo", glm::vec3(1.03f, 1.14f, 1.03f));
    createSillouetteInfo("floatingSillouetteInfo", glm::vec3(1.03f, 1.08f, 1.03f));
}

void MyApp::createSillouetteInfo(std::string name, glm::vec3 scale) {
    mgl::SillouetteInfo* sillouetteInfo = new mgl::SillouetteInfo(
        mgl::CallbackManager::getInstance().get("sillouetteCallback"),
        mgl::ShaderManager::getInstance().get("sillouetteShader"),
        scale
    );
    mgl::SillouetteInfoManager::getInstance().add(name, sillouetteInfo);
}

// object transformation constants
const glm::vec3 basePosition = { 0.0f, 0.0f, 0.0f }; //-0.207143
const glm::quat baseRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
const glm::vec3 baseScale = { 1.0f, 1.0f, 1.0f };
const glm::vec3 floatingPosition = { 0.0f, 1.3f, 0.0f };
const glm::quat floatingRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
const glm::vec3 floatingScale = { 1.0f, 1.3f, 1.0f };

const glm::vec3 lightPosition = { 2.2f, 3.0f, 3.0f };
const glm::quat lightRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
const glm::vec3 lightScale = { 0.2f, 0.2f, 0.2f };


void MyApp::createScene() {
    int nodeId = 0;
    SceneGraph = new mgl::SceneGraph();

    // comment to disable loading from file
    SceneGraph->deserialize();
    
    // uncomment to enable loading from app, this is where you can add things to the scene
    /*
    mgl::SceneNode* root = new mgl::SceneNode(nodeId++);
    SceneGraph->addRoot(root);

    // create the light node
    mgl::SceneNode* light = new mgl::PointLightNode(nodeId++, UBO_BP_LIGHT);
    light->setMesh("cubeMesh");
    light->setPosition(lightPosition);
    light->setRotation(lightRotation);
    light->setScale(lightScale);
    light->setShaderProgram("lightShader");
    light->setCallBack("stencilCallback");
    root->addChild(light);
    light->setSillouetteInfo("defaultSillouetteInfo");

    // create the wood base object
    mgl::SceneNode* base = createNode(nodeId++, "baseMesh", basePosition, baseRotation, baseScale, root, "woodShader");
    base->setTextureInfo("baseTexture3D");
    base->setCallBack("stencilCallback");
    base->setSillouetteInfo("baseSillouetteInfo");

    // create the marble top object
    mgl::SceneNode* floating = createNode(nodeId++, "floatingMesh", floatingPosition, floatingRotation, floatingScale, base, "marbleShader");
    floating->setTextureInfo("floatingTexture3D");
    floating->setCallBack("stencilCallback");
    floating->setSillouetteInfo("floatingSillouetteInfo");
    */
}

mgl::SceneNode* MyApp::createNode(int nodeId, std::string mesh, glm::vec3 position, glm::quat rotation, glm::vec3 scale, mgl::SceneNode* parent,std::string shader) {

    mgl::SceneNode* node = new mgl::SceneNode(nodeId);
    node->setMesh(mesh);
    node->setPosition(position);
    node->setRotation(rotation);
    node->setScale(scale);
    node->setShaderProgram(shader);
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
    activateStencilBuffer();
    createMeshes();
    createTextures();
    createShaderPrograms();  // after mesh;
    createCallBacks();
    createSillouetteInfos();
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
    delete SceneGraph;
    mgl::MeshManager::getInstance().DestroyObjects();
    mgl::TextureInfoManager::getInstance().DestroyObjects();
    mgl::ShaderManager::getInstance().DestroyObjects();
    mgl::CallbackManager::getInstance().DestroyObjects();
    mgl::SillouetteInfoManager::getInstance().DestroyObjects();
}

void MyApp::processAnimation(double elapsed) {
    // empty for now
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
