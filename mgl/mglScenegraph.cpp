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

	void SceneGraph::moveToBox(double elapsed) {
		root->moveToBox(elapsed);
	}

	void SceneGraph::moveToShape(double elapsed) {
		root->moveToShape(elapsed);
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
		ModelMatrix = glm::mat4(1.0f);
		AnimatedModelMatrix = ModelMatrix;
		ModelMatrixId = modelMatrixId;
		NormalMatrixId = normalMatrixId;
		ColorId = colorId;
		mesh = nullptr;
		shaderProgram = nullptr;

		parent = nullptr;
		children = std::vector<SceneNode*>();
	}

	SceneNode::~SceneNode() {}

	void SceneNode::setModelMatrix(glm::mat4 modelmatrix) {
		ModelMatrix = modelmatrix;
		AnimatedModelMatrix = ModelMatrix;
		setNormalMatrix(glm::transpose(glm::inverse(AnimatedModelMatrix)));
	}

	glm::mat4 SceneNode::getModelMatrix() {
		return ModelMatrix;
	}

	void SceneNode::setAnimationMovement(glm::quat axisRotationDest, glm::quat boxRotationDest, glm::vec3 boxTranslationDest) {
		AxisRotationDest = axisRotationDest;
		BoxRotationDest = boxRotationDest;
		BoxTranslationDest = boxTranslationDest;
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

	void SceneNode::setColor(glm::vec4 color) {
		Color = color;
	}

	glm::vec4 SceneNode::getColor() {
		return Color;
	}

	void SceneNode::setShaderProgram(ShaderProgram* shaderProgram) {
		this->shaderProgram = shaderProgram;
	}

	ShaderProgram* SceneNode::getShaderProgram() {
		return shaderProgram;
	}

	void SceneNode::setAnimationTime(double time) {
		AnimationTime = time;
	}

	void SceneNode::animate() {
		glm::vec3 BoxTranslationCurr = glm::mix(BoxTranslationOrig, BoxTranslationDest, Accum / AnimationTime);

		glm::quat AxisRotationCurr = glm::slerp(AxisRotationOrig, AxisRotationDest, (float)Accum / (float)AnimationTime);

		// start rotation only half way through the movement bc why not
		glm::quat BoxRotationCurr;
		if (Accum > AnimationTime / 2) {
			float num = ((float)Accum - AnimationTime / 2) / ((float)AnimationTime / 2.0f);
			BoxRotationCurr = glm::slerp(BoxRotationOrig, BoxRotationDest, num);
		}
		else {
			BoxRotationCurr = BoxRotationOrig;
		}

		AnimatedModelMatrix = glm::mat4(AxisRotationCurr) * glm::translate(BoxTranslationCurr) * ModelMatrix * glm::mat4(BoxRotationCurr);

		// update for color
		setNormalMatrix(glm::transpose(glm::inverse(AnimatedModelMatrix)));
	}

	void SceneNode::moveToBox(double elapsed) {
		// move children
		for (auto child : children) {
			child->moveToBox(elapsed);
		}


		if (glm::abs(Accum - AnimationTime) < THRESHOLD) return;

		if (Accum + elapsed < AnimationTime) {
			Accum += elapsed;
		}
		else {
			Accum += AnimationTime - Accum;
		}

		animate();

		
	}

	void SceneNode::moveToShape(double elapsed) {

		// move children
		for (auto child : children) {
			child->moveToShape(elapsed);
		}

		if (glm::abs(Accum - 0.0) < THRESHOLD) return;

		if (Accum - elapsed > 0.0) {
			Accum -= elapsed;
		}
		else {
			Accum -= Accum;
		}

		animate();

		
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

	void SceneNode::draw() {
		if (mesh) {
			shaderProgram->bind();
			glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(AnimatedModelMatrix));
			glUniformMatrix4fv(NormalMatrixId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
			glUniform4fv(ColorId, 1, glm::value_ptr(Color));
			mesh->draw();
			shaderProgram->unbind();
		}

		// draw children
		for (auto child : children) {
			child->draw();
		}
	}
	
}