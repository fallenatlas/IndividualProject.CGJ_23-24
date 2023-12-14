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
  mgl::OrbitCamera* OrbitCameras[2] = { nullptr };
  int activeIndex = 0;
  GLint ModelMatrixId;
  GLint NormalMatrixId;
  GLint ColorId;
  mgl::SceneGraph* SceneGraph = nullptr;
  mgl::Mesh* SquareMesh = nullptr;
  mgl::Mesh* ParalelogramMesh = nullptr;
  mgl::Mesh* TriangleMesh = nullptr;

  double previousMousePositionX = 0;
  double previousMousePositionY = 0;

  void createMeshes();
  void createShaderPrograms();
  void createCameras();
  void createScene();
  void createNode(mgl::Mesh* mesh, glm::mat4 modelMatrix, glm::vec4 color, double animationTime, glm::quat axisRotationDest, glm::quat boxRotationDest, glm::vec3 boxTranslationDest, mgl::SceneNode* parent);
  void drawScene(double elapsed);
  void processAnimation(double elapsed);
  void switchCamera();
};

void MyApp::cursorCallback(GLFWwindow* win, double xpos, double ypos) {
    if (mgl::KeyState::getInstance().isMouseButtonPressed(GLFW_MOUSE_BUTTON_1)) {

        double xMoved = xpos - previousMousePositionX;
        double yMoved = ypos - previousMousePositionY;
        
        double anglex = xMoved * 20;
        double angley = yMoved * 20;
        float yawAngle = glm::radians(anglex);
        float pitchAngle = glm::radians(angley);

        OrbitCameras[activeIndex]->addYaw(yawAngle);
        OrbitCameras[activeIndex]->addPitch(pitchAngle);
    }
    previousMousePositionX = xpos;
    previousMousePositionY = ypos;
}

void MyApp::mouseButtonCallback(GLFWwindow* win, int button, int action,
    int mods) {
    mgl::KeyState::getInstance().updateMouseButtonState(button, action);
}

void MyApp::scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    OrbitCameras[activeIndex]->addZoom(yoffset);
}

void MyApp::keyCallback(GLFWwindow* win, int key, int scancode, int action,
    int mods) {
    mgl::KeyState::getInstance().updateKeyState(key, action);

    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_C)) {
        switchCamera();
    }
    else if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_P)) {
        OrbitCameras[activeIndex]->switchProjection();
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, GLFW_TRUE);
        windowCloseCallback(win);
    }
}

void MyApp::switchCamera() {
    OrbitCameras[activeIndex]->setActive(false);
    activeIndex = activeIndex == 0 ? 1 : 0;
    OrbitCameras[activeIndex]->setActive(true);
    SceneGraph->setCamera(OrbitCameras[activeIndex]);
}


///////////////////////////////////////////////////////////////////////// MESHES

void MyApp::createMeshes() {
    std::string mesh_dir = "../assets/";
    std::string square_mesh_file = "square_fixed.obj";
    std::string paralelogram_mesh_file = "paralelogram_fixed.obj";
    std::string triangle_mesh_file = "triangle_fixed.obj";
    std::string square_mesh_fullname = mesh_dir + square_mesh_file;
    std::string paralelogram_mesh_fullname = mesh_dir + paralelogram_mesh_file;
    std::string triangle_mesh_fullname = mesh_dir + triangle_mesh_file;

    SquareMesh = new mgl::Mesh();
    SquareMesh->joinIdenticalVertices();
    SquareMesh->create(square_mesh_fullname);

    ParalelogramMesh = new mgl::Mesh();
    ParalelogramMesh->joinIdenticalVertices();
    ParalelogramMesh->create(paralelogram_mesh_fullname);

    TriangleMesh = new mgl::Mesh();
    TriangleMesh->joinIdenticalVertices();
    TriangleMesh->create(triangle_mesh_fullname);
}

///////////////////////////////////////////////////////////////////////// SHADER

void MyApp::createShaderPrograms() {
    Shaders = new mgl::ShaderProgram();
    Shaders->addShader(GL_VERTEX_SHADER, "cube-vs.glsl");
    Shaders->addShader(GL_FRAGMENT_SHADER, "cube-fs.glsl");

    Shaders->addAttribute(mgl::POSITION_ATTRIBUTE, mgl::Mesh::POSITION);
    if (SquareMesh->hasNormals() && TriangleMesh->hasNormals() && ParalelogramMesh->hasNormals()) {
        Shaders->addAttribute(mgl::NORMAL_ATTRIBUTE, mgl::Mesh::NORMAL);
    }
    if (SquareMesh->hasTexcoords() && TriangleMesh->hasTexcoords() && ParalelogramMesh->hasTexcoords()) {
        Shaders->addAttribute(mgl::TEXCOORD_ATTRIBUTE, mgl::Mesh::TEXCOORD);
    }
    if (SquareMesh->hasTangentsAndBitangents() && TriangleMesh->hasTangentsAndBitangents() && ParalelogramMesh->hasTangentsAndBitangents()) {
        Shaders->addAttribute(mgl::TANGENT_ATTRIBUTE, mgl::Mesh::TANGENT);
    }

    Shaders->addUniform(mgl::MODEL_MATRIX);
    Shaders->addUniform(mgl::COLOR_UNIFORM);
    Shaders->addUniform(mgl::NORMAL_MATRIX);
    Shaders->addUniformBlock(mgl::CAMERA_BLOCK, UBO_BP);
    Shaders->create();

    ModelMatrixId = Shaders->Uniforms[mgl::MODEL_MATRIX].index;
    NormalMatrixId = Shaders->Uniforms[mgl::NORMAL_MATRIX].index;
    ColorId = Shaders->Uniforms[mgl::COLOR_UNIFORM].index;
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


void MyApp::createScene() {
    SceneGraph = new mgl::SceneGraph();
    mgl::SceneNode* parent = new mgl::SceneNode(ModelMatrixId, NormalMatrixId, ColorId);

    SceneGraph->addRoot(parent);

    // square
    createNode(SquareMesh, squareMM, squareColor, 6.0, zAxisRotation, squareBoxRotation, translationUp, parent);

    // paralelogram
    createNode(ParalelogramMesh, paralelogramMM, paralelogramColor, 3.0, zAxisRotation, paralelogramBoxRotation, paralelogramBoxTranslation + translationUp, parent);

    // small triangles
    // cyan
    createNode(TriangleMesh, cyanMM, aquaColor, 8.0, zAxisRotation, cyanBoxRotation, cyanBoxTranslation + translationUp, parent);

    // orange
    createNode(TriangleMesh, orangeMM, orangeColor, 6.0, zAxisRotation, orangeBoxRotation, orangeBoxTranslation + translationUp, parent);
    
    // medium triangles
    // purple
    createNode(TriangleMesh, purpleMM, purpleColor, 2.0, zAxisRotation, purpleBoxRotation, purpleBoxTranslation + translationUp, parent);

    // big triangles
    // blue
    createNode(TriangleMesh, blueMM, blueColor, 6.0, zAxisRotation, zeroBoxRotation, translationUp, parent);

    // magenta
    createNode(TriangleMesh, magentaMM, magentaColor, 6.0, zAxisRotation, zeroBoxRotation, translationUp, parent);
}

void MyApp::createNode(mgl::Mesh *mesh, glm::mat4 modelMatrix, glm::vec4 color, double animationTime, glm::quat axisRotationDest, glm::quat boxRotationDest, glm::vec3 boxTranslationDest, mgl::SceneNode* parent) {

    mgl::SceneNode* node = new mgl::SceneNode(ModelMatrixId, NormalMatrixId, ColorId);

    //We are creating a root node (only happens once)
    node->setMesh(mesh);
    node->setModelMatrix(modelMatrix);
    node->setAnimationTime(animationTime);
    node->setAnimationMovement(axisRotationDest, boxRotationDest, boxTranslationDest);
    node->setColor(color);
    node->setShaderProgram(Shaders);
    //SceneGraph->addRoot(node);
    parent->addChild(node);
    
    
}

////////////////////////////////////////////////////////////////////////// CAMERA

void MyApp::createCameras() {
    OrbitCameras[0] = new mgl::OrbitCamera(UBO_BP, true, glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    OrbitCameras[1] = new mgl::OrbitCamera(UBO_BP, false, glm::vec3(5.0f, 13.0f, 5.0f), glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    activeIndex = 0;
    SceneGraph->setCamera(OrbitCameras[activeIndex]);
}

////////////////////////////////////////////////////////////////////////// SCENE

const glm::mat4 ModelMatrix(1.0f);

void MyApp::drawScene(double elapsed) {
    SceneGraph->renderScene(elapsed);
}

////////////////////////////////////////////////////////////////////// CALLBACKS

void MyApp::initCallback(GLFWwindow* win) {
    createMeshes();
    createShaderPrograms();  // after mesh;
    createScene();
    createCameras();
}

void MyApp::windowSizeCallback(GLFWwindow *win, int winx, int winy) {
  glViewport(0, 0, winx, winy);
  OrbitCameras[0]->updatePerspectiveProjectionMatrix(winx, winy);
  OrbitCameras[1]->updatePerspectiveProjectionMatrix(winx, winy);
}

void MyApp::displayCallback(GLFWwindow *win, double elapsed) { 
    processAnimation(elapsed);
    drawScene(elapsed); 
}
void MyApp::windowCloseCallback(GLFWwindow* win) {
    delete SquareMesh;
    delete ParalelogramMesh;
    delete TriangleMesh;
    delete OrbitCameras[0];
    delete OrbitCameras[1];
    delete Shaders;
    delete SceneGraph;
}

void MyApp::processAnimation(double elapsed) {
    if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_LEFT)) {
        SceneGraph->moveToBox(elapsed);
    }
    else if (mgl::KeyState::getInstance().isKeyPressed(GLFW_KEY_RIGHT)) {
        SceneGraph->moveToShape(elapsed);
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
