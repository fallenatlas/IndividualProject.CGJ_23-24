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
		root->update(elapsed);
	}

	void SceneGraph::drawNode(SceneNode* node) {
		if (node->getMesh()) {
			node->draw();
		}
	}

	void SceneGraph::serialize() {
		json node_json = root->serialize();

		std::ofstream o("pretty.json");
		o << std::setw(4) << node_json << std::endl;
	}

	void SceneGraph::deserialize() {
		// read a JSON file
		std::ifstream i("pretty.json");
		json node_json;
		i >> node_json;

		SceneNode* root = new SceneNode(0);

		root->deserialize(node_json);

		addRoot(root);
	}

	SceneNode* SceneGraph::getNode(int nodeId) {
		return root->getNode(nodeId);
	}

	///////////////////////////////////////////////////////////////////////// SceneNode
	SceneNode::SceneNode(int nodeId) {
		id = nodeId;
		selected = false;
		ModelMatrix = glm::mat4(1.0f);
		Position = { 0.0f, 0.0f, 0.0f };
		Rotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		Scale = { 1.0f, 1.0f, 1.0f };

		frameMovement = { 0.0f, 0.0f, 0.0f };
		frameRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

		mesh = nullptr;
		shaderProgram = nullptr;
		callback = nullptr;
		sillouetteInfo = nullptr;

		parent = nullptr;
		children = std::vector<SceneNode*>();
	}

	SceneNode::~SceneNode() {}

	void SceneNode::setModelMatrix(glm::mat4 modelmatrix) {
		ModelMatrix = modelmatrix;
		glm::mat3 nm = glm::mat3(glm::transpose(glm::inverse(ModelMatrix)));
		setNormalMatrix(nm);
	}

	glm::mat4 SceneNode::getModelMatrix() {
		return ModelMatrix;
	}

	TextureInfo* SceneNode::getTextureInfo() {
		return textureInfo;
	}
	

	void SceneNode::setTextureInfo(std::string textureInfoName) {
		this->textureInfoName = textureInfoName;
		this->textureInfo = TextureInfoManager::getInstance().get(textureInfoName);
	}

	std::string SceneNode::getTextureInfoName() {
		return textureInfoName;
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

	const glm::vec3 SceneNode::getGlobalPosition() {
		return glm::vec3(ModelMatrix[3][0], ModelMatrix[3][1], ModelMatrix[3][2]);
	}

	const glm::quat SceneNode::getGlobalRotation() {
		glm::quat parentGlobalRotation = parent ? parent->getGlobalRotation() : glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));;
		return parentGlobalRotation * getRotation();
	}

	void SceneNode::setScale(glm::vec3 scale) {
		Scale = scale;
	}

	const glm::vec3 SceneNode::getScale() {
		return Scale;
	}

	void SceneNode::setNormalMatrix(glm::mat3 normalMatrix) {
		NormalMatrix = normalMatrix;
	}

	glm::mat3 SceneNode::getNormalMatrix() {
		return NormalMatrix;
	}

	Mesh* SceneNode::getMesh() {
		return mesh;
	}
	

	void SceneNode::setMesh(std::string meshName) {
		this->meshName = meshName;
		this->mesh = MeshManager::getInstance().get(meshName);
	}

	std::string SceneNode::getMeshName() {
		return meshName;
	}

	ShaderProgram* SceneNode::getShaderProgram() {
		return shaderProgram;
	}
	

	void SceneNode::setShaderProgram(std::string shaderProgramName) {
		this->shaderProgramName = shaderProgramName;
		this->shaderProgram = ShaderManager::getInstance().get(shaderProgramName);
		if (this->shaderProgram == nullptr) return;
		ModelMatrixId = this->shaderProgram->Uniforms[mgl::MODEL_MATRIX].index;
		//NormalMatrixId = this->shaderProgram->Uniforms[mgl::NORMAL_MATRIX].index;
	}

	std::string SceneNode::getShaderProgramName() {
		return shaderProgramName;
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

	void SceneNode::setCallBack(std::string callbackName) {
		this->callbackName = callbackName;
		this->callback = CallbackManager::getInstance().get(callbackName);
	}

	CallBack* SceneNode::getCallBack() {
		return callback;
	}

	std::string SceneNode::getCallBackName() {
		return callbackName;
	}

	void SceneNode::update(double elapsed) {
		applyFrameTransformations(elapsed);
		draw();

		// draw children
		for (auto child : children) {
			child->update(elapsed);
		}
	}

	void SceneNode::draw() {
		if (callback) {
			callback->beforeDraw(id);
		}

		if (mesh && shaderProgram) {
			shaderProgram->bind();

			if (textureInfo) {
				textureInfo->updateShader(shaderProgram);
			}

			glm::mat4 parentModelMatrix = parent ? parent->getModelMatrix() : glm::mat4(1.0f);
			setModelMatrix(parentModelMatrix * glm::translate(Position) * glm::mat4(Rotation) * glm::scale(Scale));

			glUniformMatrix4fv(ModelMatrixId, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
			if (this->shaderProgram->isUniform(mgl::NORMAL_MATRIX)) {
				glUniformMatrix3fv(this->shaderProgram->Uniforms[mgl::NORMAL_MATRIX].index, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
			}
			mesh->draw();
			shaderProgram->unbind();
		}

		if (callback) {
			callback->afterDraw(id);
		}

		if (sillouetteInfo && selected) {
			drawSillouette();
		}
	}

	SceneNode* SceneNode::getNode(int nodeId) {
		if (id == nodeId) return this;
		for (auto child : children) {
			SceneNode* node = child->getNode(nodeId);
			if (node != nullptr) return node;
		}
		return nullptr;
	}

	void SceneNode::setSillouetteInfo(std::string sillouetteInfoName) {
		this->sillouetteInfoName = sillouetteInfoName;
		this->sillouetteInfo = SillouetteInfoManager::getInstance().get(sillouetteInfoName);
	}

	SillouetteInfo* SceneNode::getSillouetteInfo() {
		return sillouetteInfo;
	}

	void SceneNode::isSelected(bool value) {
		selected = value;

		for (auto child : children) {
			child->isSelected(value);
		}
	}

	void SceneNode::addFrameMovement(glm::vec3 movement) {
		frameMovement += movement;
	}

	void SceneNode::addFrameRotation(glm::quat rotation) {
		frameRotation = rotation * frameRotation;
	}

	void SceneNode::applyFrameTransformations(double elapsed) {
		setPosition(Position + (frameMovement * (float)elapsed));

		glm::quat targetRotation = (frameRotation) * Rotation;
		setRotation(glm::slerp(Rotation, targetRotation, (float)elapsed));

		frameMovement = { 0.0f, 0.0f, 0.0f };
		frameRotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	}

	void SceneNode::drawSillouette() {

		if (sillouetteInfo->callback) {
			sillouetteInfo->callback->beforeDraw(id);
		}

		if (mesh && sillouetteInfo->shaderProgram) {
			sillouetteInfo->shaderProgram->bind();

			const glm::mat4 sillouetteMatrix = ModelMatrix * glm::scale(sillouetteInfo->scale);
			glUniformMatrix4fv(sillouetteInfo->shaderProgram->Uniforms[mgl::MODEL_MATRIX].index, 1, GL_FALSE, glm::value_ptr(sillouetteMatrix));

			mesh->draw();

			sillouetteInfo->shaderProgram->unbind();
		}

		if (sillouetteInfo->callback) {
			sillouetteInfo->callback->afterDraw(id);
		}

	}

	json SceneNode::serialize() {
		json node_json;

		node_json["Type"] = "Object";
		node_json["ModelMatrix"] = glm::to_string(ModelMatrix);
		node_json["Position"] = glm::to_string(Position);
		node_json["Rotation"] = glm::to_string(Rotation);
		node_json["Scale"] = glm::to_string(Scale);
		node_json["Mesh"] = meshName;
		node_json["Shader"] = shaderProgramName;
		node_json["Callback"] = callbackName;
		node_json["Texture"] = textureInfoName;
		node_json["Sillouette"] = sillouetteInfoName;

		std::cout << glm::to_string(Position) << std::endl;
		std::cout << glm::to_string(Rotation) << std::endl;
		json node_children_json;

		for (auto child : children) {
			node_children_json[std::to_string(child->id)] = child->serialize();
		}
		node_json["Children"] = node_children_json;

		return node_json;
	}

	void SceneNode::deserialize(json node_json) {

		ModelMatrix = aux::deserialize_mat4(node_json["ModelMatrix"].template get<std::string>());

		Position = aux::deserialize_vec3(node_json["Position"].template get<std::string>());
		
		Rotation = aux::deserialize_quat(node_json["Rotation"].template get<std::string>());
		
		Scale = aux::deserialize_vec3(node_json["Scale"].template get<std::string>());
		
		setMesh(node_json["Mesh"].template get<std::string>());
		
		setShaderProgram(node_json["Shader"].template get<std::string>());

		setCallBack(node_json["Callback"].template get<std::string>());
		
		setTextureInfo(node_json["Texture"].template get<std::string>());

		setSillouetteInfo(node_json["Sillouette"].template get<std::string>());

		json children = node_json["Children"];

		for (auto& el : children.items()) {
			std::cout << el.key() << std::endl;

			SceneNode* child;
			std::string type = el.value()["Type"].template get<std::string>();
			if (type == "Light") {
				int bp = el.value()["BindingPoint"].template get<int>();
				child = new PointLightNode(stoi(el.key()), bp);
			}
			else {
				child = new SceneNode(stoi(el.key()));
			}

			std::cout << el.key() << std::endl;

			child->deserialize(el.value());

			addChild(child);
		}
	}

	PointLightNode::PointLightNode(int nodeId, GLint BindingPoint) : SceneNode(nodeId) {
		this->BindingPoint = BindingPoint;
		bindBuffer();
	}

	PointLightNode::~PointLightNode() {
		UnbindBuffer();
	}

	void PointLightNode::setPosition(glm::vec3 position) {
		SceneNode::setPosition(position);
		setUniformPosition();
	}

	void PointLightNode::bindBuffer() {
		glGenBuffers(1, &UboId);
		glBindBuffer(GL_UNIFORM_BUFFER, UboId);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec3), 0, GL_STREAM_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, BindingPoint, UboId);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		setUniformPosition();
	}

	void PointLightNode::UnbindBuffer() {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glDeleteBuffers(1, &UboId);
	}

	void PointLightNode::setUniformPosition() {
		glm::vec3 position = getGlobalPosition();
		glBindBuffer(GL_UNIFORM_BUFFER, UboId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3),
			glm::value_ptr(position));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	json PointLightNode::serialize() {
		json node_json = SceneNode::serialize();
		node_json["Type"] = "Light";
		node_json["BindingPoint"] = BindingPoint;
		return node_json;
	}

	void PointLightNode::deserialize(json node_json) {
		SceneNode::deserialize(node_json);
	}
	
}
