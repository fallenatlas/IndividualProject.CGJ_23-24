////////////////////////////////////////////////////////////////////////////////
//
// Scene Management Class
//
// Copyright (c)2022-23 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_SCENEGRAPH_HPP
#define MGL_SCENEGRAPH_HPP

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "./mglOrbitCamera.hpp"
#include "./mglShader.hpp"
#include "./mglMesh.hpp"

const double THRESHOLD = static_cast<float>(1.0e-5);

namespace mgl {

class SceneGraph;
class SceneNode;

////////////////////////////////////////////////////////////////////// SceneGraph

class SceneGraph {
private:
	OrbitCamera* camera;
	SceneNode* root;

public:
	SceneGraph();
	virtual ~SceneGraph();

	void setCamera(OrbitCamera* camera);
	OrbitCamera* getCamera();

	void addRoot(SceneNode* node);
	SceneNode* getRoot();

	void moveToBox(double elapsed);
	void moveToShape(double elapsed);

	void renderScene(double elapsed);

	void drawNode(SceneNode* node);
};

////////////////////////////////////////////////////////////////////// SceneNode

class SceneNode {
private:
	glm::mat4 ModelMatrix;
	Mesh* mesh;
	ShaderProgram* shaderProgram;
	GLint ModelMatrixId;
	GLint ColorId;
	GLint NormalMatrixId;
	glm::mat4 NormalMatrix;
	glm::vec4 Color;

	SceneNode* parent;
	std::vector<SceneNode*> children;
	
	// animation destination
	glm::quat AxisRotationDest;
	glm::quat BoxRotationDest;
	glm::vec3 BoxTranslationDest;

	// animation origin
	const glm::quat AxisRotationOrig = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	const glm::quat BoxRotationOrig = glm::angleAxis(glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::vec3 BoxTranslationOrig = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::mat4 AnimatedModelMatrix;

	double Accum = 0;
	double AnimationTime = 6;

	// parent
	// vector childs

public:
	SceneNode(GLint ModelMatrixId, GLint normalMatrixId, GLint colorId);
	virtual ~SceneNode();

	void setModelMatrix(glm::mat4 ModelMatrix);
	glm::mat4 getModelMatrix();

	void setAnimationMovement(glm::quat axisRotationDest, glm::quat boxRotationDest, glm::vec3 boxTranslationDest);

	void setNormalMatrix(glm::mat4 NormalMatrix);
	glm::mat4 getNormalMatrix();

	void setMesh(Mesh* mesh);
	Mesh* getMesh();

	void setColor(glm::vec4 color);
	glm::vec4 getColor();

	void setShaderProgram(ShaderProgram* shaderProgram);
	ShaderProgram* getShaderProgram();

	void setAnimationTime(double time);
	void animate();
	void moveToBox(double elapsed);
	void moveToShape(double elapsed);

	void addChild(SceneNode* node);
	std::vector<SceneNode*> getChildren();

	void setParent(SceneNode* node);
	SceneNode* getParent();

	void draw();
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_SCENEGRAPH_HPP */
