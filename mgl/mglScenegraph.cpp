//Scnene Graph Class

#include "./mglScenegraph.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "./mglShader.hpp"
#include "./mglMesh.hpp"

namespace mgl
{
	SceneGraph::SceneGraph() {
		this->root = nullptr;
	}

	SceneGraph::~SceneGraph() {
		delete root;
	}

	void SceneGraph::setCamera(OrbitCamera* camera) {
		this->camera = camera;
	}

	OrbitCamera* SceneGraph::getCamera() {
		return camera;
	}

	void SceneGraph::addRoot(SceneNode* node) {
		this->root = node;
	}

	SceneNode* SceneGraph::getRoot() {
		return this->root;
	}

	void SceneGraph::renderScene(double elapsed) {
		camera->updateRotation(elapsed);
		root->draw();
	}

	void SceneGraph::drawNode(SceneNode* node) {
		if (node->getMesh()) {
			node->draw();
		}
	}

	///////////////////////////////////////////////////////////////////////// SceneNode
	SceneNode::SceneNode(GLint modelMatrixId, GLint normalMatrixId, GLint colorId) {
		active = true;
		ModelMatrix = glm::mat4(1.0f);
		Position = { 0.0f, 0.0f, 0.0f };
		Rotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		Scale = { 1.0f, 1.0f, 1.0f };
		ModelMatrixId = modelMatrixId;
		NormalMatrixId = normalMatrixId;
		mesh = nullptr;
		shaderProgram = nullptr;
		callback = nullptr;

		parent = nullptr;
		children = std::vector<SceneNode*>();
	}

	SceneNode::~SceneNode() {}

	void SceneNode::setModelMatrix(glm::mat4 modelmatrix) {
		ModelMatrix = modelmatrix;
		setNormalMatrix(glm::transpose(glm::inverse(ModelMatrix)));
	}

	glm::mat4 SceneNode::getModelMatrix() {
		return ModelMatrix;
	}

	void SceneNode::setTextureInfo(TextureInfo* TextureInfo) {
		textureInfo = TextureInfo;
	}

	TextureInfo* SceneNode::getTextureInfo() {
		return textureInfo;
	}

	void SceneNode::setPosition(glm::vec3 position) {
		Position = position;
	}

	const glm::vec3 SceneNode::getPosition() {
		return Position;
	}

	void SceneNode::setRotation(glm::quat rotation) {
		Rotation = rotation;
	}

	const glm::quat SceneNode::getRotation() {
		return Rotation;
	}

	void SceneNode::setScale(glm::vec3 scale) {
		Scale = scale;
	}

	const glm::vec3 SceneNode::getScale() {
		return Scale;
	}

	void SceneNode::setNormalMatrix(glm::mat4 normalMatrix) {
		NormalMatrix = normalMatrix;
	}

	glm::mat4 SceneNode::getNormalMatrix() {
		return NormalMatrix;
	}

	void SceneNode::setMesh(Mesh* mesh) {
		this->mesh = mesh;
	}

	Mesh* SceneNode::getMesh() {
		return mesh;
	}

	void SceneNode::setShaderProgram(ShaderProgram* shaderProgram) {
		this->shaderProgram = shaderProgram;
	}

	ShaderProgram* SceneNode::getShaderProgram() {
		return shaderProgram;
	}

	void SceneNode::addChild(SceneNode* node) {
		children.push_back(node);
		node->parent = this;
	}

	std::vector<SceneNode*> SceneNode::getChildren() {
		return children;
	}

	void SceneNode::setParent(SceneNode* node) {
		parent = node;
	}

	SceneNode* SceneNode::getParent() {
		return parent;
	}

	void SceneNode::setCallBack(CallBack* callback) {
		this->callback = callback;
	}

	CallBack* SceneNode::getCallBack() {
		return callback;
	}

	void SceneNode::draw() {
		if (callback) {
			callback->beforeDraw();
		}

		// if (shaderProgram)
		
		if (mesh) {
			shaderProgram->bind();

			if (textureInfo) {
				textureInfo->updateShader(shaderProgram);
			}

			glm::mat4 parentModelMatrix = parent ? parent->getModelMatrix() : glm::mat4(1.0f);
			setModelMatrix(parentModelMatrix * glm::translate(Position) * glm::mat4(Rotation) * glm::scale(Scale));

			glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
			glUniformMatrix4fv(NormalMatrixId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
			//glUniform4fv(ColorId, 1, glm::value_ptr(Color));
			mesh->draw();
			shaderProgram->unbind();
		}

		if (callback) {
			callback->afterDraw();
		}

		// draw children
		for (auto child : children) {
			child->draw();
		}
	}
	
}