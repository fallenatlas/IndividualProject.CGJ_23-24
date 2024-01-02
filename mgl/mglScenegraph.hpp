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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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
#include "./mglSillouette.hpp"

const double THRESHOLD = static_cast<float>(1.0e-5);

using json = nlohmann::json;

namespace mgl {

class SceneGraph;
class SceneNode;
class PointLightNode;

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

	SceneNode* getNode(int nodeId);
};

////////////////////////////////////////////////////////////////////// SceneNode

class SceneNode {
private:
	int id = 0;
	bool selected;

	glm::mat4 ModelMatrix;

	glm::vec3 Position;
	glm::quat Rotation;
	glm::vec3 Scale;

	glm::vec3 frameMovement;
	glm::quat frameRotation;

	Mesh* mesh;
	std::string meshName;
	ShaderProgram* shaderProgram;
	std::string shaderProgramName;
	// when getting the shader get these
	GLint ModelMatrixId;
	GLint NormalMatrixId;
	glm::mat3 NormalMatrix;

	SceneNode* parent;
	std::vector<SceneNode*> children;

	CallBack* callback;
	std::string callbackName;
	TextureInfo* textureInfo;
	std::string textureInfoName;

	std::string sillouetteInfoName;
	SillouetteInfo* sillouetteInfo;

	// parent
	// vector childs

public:
	SceneNode(int nodeId);
	virtual ~SceneNode();

	void setModelMatrix(glm::mat4 ModelMatrix);
	glm::mat4 getModelMatrix();

	void setNormalMatrix(glm::mat3 NormalMatrix);
	glm::mat3 getNormalMatrix();

	Mesh* getMesh();
	void setMesh(std::string meshName);
	std::string getMeshName();

	TextureInfo* getTextureInfo();
	void setTextureInfo(std::string textureInfoName);
	std::string getTextureInfoName();

	virtual void setPosition(glm::vec3 position);
	const glm::vec3 getPosition();
	const glm::vec3 getGlobalPosition();
	void setRotation(glm::quat rotation);
	const glm::quat getRotation();
	const glm::quat getGlobalRotation();
	void setScale(glm::vec3 scale);
	const glm::vec3 getScale();

	ShaderProgram* getShaderProgram();
	void setShaderProgram(std::string shaderProgramName);
	std::string getShaderProgramName();

	void addChild(SceneNode* node);
	std::vector<SceneNode*> getChildren();

	void setParent(SceneNode* node);
	SceneNode* getParent();

	void setCallBack(std::string callbackName);
	CallBack* getCallBack();
	std::string getCallBackName();

	void update(double elapsed);
	void draw();

	virtual json serialize();
	virtual void deserialize(json node_json);

	SceneNode* getNode(int nodeId);

	void setSillouetteInfo(std::string sillouetteInfoName);
	SillouetteInfo* getSillouetteInfo();

	void isSelected(bool value);

	void addFrameMovement(glm::vec3 movement);
	void addFrameRotation(glm::quat rotation);

	void applyFrameTransformations(double elapsed);

	void drawSillouette();
};

class PointLightNode : public SceneNode {
private:
	GLint BindingPoint;
	GLuint UboId;
public:
	PointLightNode(int nodeId, GLint BindingPoint);
	virtual ~PointLightNode();
	virtual void setPosition(glm::vec3 position) override;
	void bindBuffer();
	void UnbindBuffer();
	void setUniformPosition();
	virtual json serialize() override;
	virtual void deserialize(json node_json) override;
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace mgl

#endif /* MGL_SCENEGRAPH_HPP */
