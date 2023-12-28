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
#include <glm/gtx/string_cast.hpp>

#include <vector>
#include <fstream>
#include <algorithm>

#include "./json.hpp"
#include "./mglOrbitCamera.hpp"
#include "./mglShader.hpp"
#include "./mglMesh.hpp"
#include "./mglCallBack.hpp"
#include "./mglTexture.hpp"
#include "./mglConventions.hpp"
#include "./mglManager.hpp"

const double THRESHOLD = static_cast<float>(1.0e-5);

using json = nlohmann::json;

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

	void serialize();
	void deserialize();
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
	std::string meshName;
	ShaderProgram* shaderProgram;
	std::string shaderProgramName;
	// when getting the shader get these
	GLint ModelMatrixId;
	GLint NormalMatrixId;
	glm::mat4 NormalMatrix;

	SceneNode* parent;
	std::vector<SceneNode*> children;

	CallBack* callback;
	std::string callbackName;
	TextureInfo* textureInfo;
	std::string textureInfoName;

	// parent
	// vector childs

public:
	SceneNode(GLint ModelMatrixId, GLint normalMatrixId, GLint colorId);
	virtual ~SceneNode();

	void setModelMatrix(glm::mat4 ModelMatrix);
	glm::mat4 getModelMatrix();

	void setNormalMatrix(glm::mat4 NormalMatrix);
	glm::mat4 getNormalMatrix();

	//void setMesh(Mesh* mesh);
	Mesh* getMesh();
	void setMesh(std::string meshName);
	std::string getMeshName();

	//void setTextureInfo(TextureInfo* TextureInfo);
	TextureInfo* getTextureInfo();
	void setTextureInfo(std::string textureInfoName);
	std::string getTextureInfoName();

	void setPosition(glm::vec3 position);
	const glm::vec3 getPosition();
	void setRotation(glm::quat rotation);
	const glm::quat getRotation();
	void setScale(glm::vec3 scale);
	const glm::vec3 getScale();

	//void setShaderProgram(ShaderProgram* shaderProgram);
	ShaderProgram* getShaderProgram();
	void setShaderProgram(std::string shaderProgramName);
	std::string getShaderProgramName();

	void addChild(SceneNode* node);
	std::vector<SceneNode*> getChildren();

	void setParent(SceneNode* node);
	SceneNode* getParent();

	void setCallBack(CallBack* callback);
	CallBack* getCallBack();

	void draw();

	json serialize();
	void deserialize(json node_json);
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_SCENEGRAPH_HPP */
