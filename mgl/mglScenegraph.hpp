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
#include "./mglCallBack.hpp"

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

	void renderScene(double elapsed);

	void drawNode(SceneNode* node);
};

////////////////////////////////////////////////////////////////////// SceneNode

class SceneNode {
private:
	bool active;

	glm::mat4 ModelMatrix;

	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;

	Mesh* mesh;
	ShaderProgram* shaderProgram;
	GLint ModelMatrixId;
	GLint ColorId;
	GLint NormalMatrixId;
	glm::mat4 NormalMatrix;
	glm::vec4 Color;

	SceneNode* parent;
	std::vector<SceneNode*> children;

	CallBack* callback;

	// parent
	// vector childs

public:
	SceneNode(GLint ModelMatrixId, GLint normalMatrixId, GLint colorId);
	virtual ~SceneNode();

	void setModelMatrix(glm::mat4 ModelMatrix);
	glm::mat4 getModelMatrix();

	void setNormalMatrix(glm::mat4 NormalMatrix);
	glm::mat4 getNormalMatrix();

	void setMesh(Mesh* mesh);
	Mesh* getMesh();

	void setPosition(glm::vec3 position);
	const glm::vec3 getPosition();
	void setRotation(glm::quat rotation);
	const glm::quat getRotation();
	void setScale(glm::vec3 scale);
	const glm::vec3 getScale();

	void setColor(glm::vec4 color);
	glm::vec4 getColor();

	void setShaderProgram(ShaderProgram* shaderProgram);
	ShaderProgram* getShaderProgram();

	void addChild(SceneNode* node);
	std::vector<SceneNode*> getChildren();

	void setParent(SceneNode* node);
	SceneNode* getParent();

	void setCallBack(CallBack* callback);
	CallBack* getCallBack();

	void draw();
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_SCENEGRAPH_HPP */
