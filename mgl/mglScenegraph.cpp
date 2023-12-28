//Scnene Graph Class

#include "./mglScenegraph.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "./mglShader.hpp"
#include "./mglMesh.hpp"

//using namespace nlohmann::literals;
//using json = nlohmann::json;

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

		SceneNode* root = new SceneNode(0, 0, 0);

		root->deserialize(node_json);

		addRoot(root);
	}

	///////////////////////////////////////////////////////////////////////// SceneNode
	SceneNode::SceneNode(GLint modelMatrixId, GLint normalMatrixId, GLint colorId) {
		active = true;
		ModelMatrix = glm::mat4(1.0f);
		Position = { 0.0f, 0.0f, 0.0f };
		Rotation = glm::quat(glm::angleAxis(glm::radians<float>(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		Scale = { 1.0f, 1.0f, 1.0f };
		//ModelMatrixId = modelMatrixId;
		//NormalMatrixId = normalMatrixId;
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

	/*
	void SceneNode::setTextureInfo(TextureInfo* TextureInfo) {
		textureInfo = TextureInfo;
	}
	*/
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

	/*
	void SceneNode::setMesh(Mesh* mesh) {
		this->mesh = mesh;
	}
	*/

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

	/*
	void SceneNode::setShaderProgram(ShaderProgram* shaderProgram) {
		this->shaderProgram = shaderProgram;
		ModelMatrixId = this->shaderProgram->Uniforms[mgl::MODEL_MATRIX].index;
		NormalMatrixId = this->shaderProgram->Uniforms[mgl::NORMAL_MATRIX].index;
	}
	*/

	ShaderProgram* SceneNode::getShaderProgram() {
		return shaderProgram;
	}
	

	void SceneNode::setShaderProgram(std::string shaderProgramName) {
		this->shaderProgramName = shaderProgramName;
		this->shaderProgram = ShaderManager::getInstance().get(shaderProgramName);
		if (this->shaderProgram == nullptr) return;
		ModelMatrixId = this->shaderProgram->Uniforms[mgl::MODEL_MATRIX].index;
		NormalMatrixId = this->shaderProgram->Uniforms[mgl::NORMAL_MATRIX].index;
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

	json SceneNode::serialize() {
		json node_json;

		node_json["active"] = active;
		node_json["ModelMatrix"] = glm::to_string(ModelMatrix);
		node_json["Position"] = glm::to_string(Position);
		node_json["Rotation"] = glm::to_string(Rotation);
		node_json["Scale"] = glm::to_string(Scale);
		node_json["Mesh"] = meshName;
		node_json["Shader"] = shaderProgramName;
		node_json["Callback"] = callbackName;
		node_json["Texture"] = textureInfoName;

		std::cout << glm::to_string(Position) << std::endl;
		std::cout << glm::to_string(Rotation) << std::endl;
		json node_children_json;

		int size = children.size();
		//node_children_json["size"] = size;
		for (auto child : children) {
			node_children_json[std::to_string(--size)] = child->serialize();
		}
		node_json["Children"] = node_children_json;

		return node_json;

		//SceneNode* parent;
		//std::vector<SceneNode*> children;
	}

	glm::vec3 deserialize_vec3(std::string str) {
		// create vec
		// remove everything except , and number
		// for until no more number and change the position where that number is supposed to be
		// vec[1] = ...

		std::string open_delimiter = "(";
		std::string close_delimiter = ")";
		std::string delimiter = ",";

		int i = 0;
		size_t pos = 0;
		std::string token;
		glm::vec3 vec = { 0.0f, 0.0f, 0.0f };

		pos = str.find(open_delimiter); // find (
		str.erase(0, pos + delimiter.length()); // get first number in position
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); // delete spaces
		str.erase(std::remove(str.begin(), str.end(), ')'), str.end()); // delete ending ')'
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			vec[i] = std::stof(token);
			std::cout << token << std::endl;
			str.erase(0, pos + delimiter.length());
			i++;
		}

		vec[i] = std::stof(str);
		std::cout << glm::to_string(vec) << std::endl;

		return vec;
	}

	glm::mat4 deserialize_mat4(std::string str) {
		std::string open_delimiter = "(";
		std::string close_delimiter = ")";
		std::string delimiter = ",";

		int col = 0, row = 0;
		size_t pos = 0;
		std::string token;
		glm::mat4 mat(0.0f);

		pos = str.find(open_delimiter); // find (
		str.erase(0, pos + open_delimiter.length()); // get first number in position
		str.erase(std::remove(str.begin(), str.end(), '('), str.end()); // delete opening '('
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); // delete spaces
		str.erase(std::remove(str.begin(), str.end(), ')'), str.end()); // delete ending ')'

		std::cout << str << std::endl;

		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			mat[col][row] = std::stof(token);
			std::cout << col << row <<  token << std::endl;
			str.erase(0, pos + delimiter.length());
			row++;
			if (row == 4) {
				col++;
				row = 0;
			}
		}

		mat[col][row] = std::stof(str);
		std::cout << glm::to_string(mat) << std::endl;

		return mat;
	}

	glm::quat deserialize_quat(std::string str) {
		std::string open_delimiter = "(";
		std::string close_delimiter = ")";
		std::string delimiter = ",";

		int i = 0;
		size_t pos = 0;
		std::string token;

		float number = 0.0f;
		glm::vec3 vec = { 0.0f, 0.0f, 0.0f };

		pos = str.find(open_delimiter); // find (
		str.erase(0, pos + delimiter.length()); // get first number in position

		pos = str.find(delimiter);
		token = str.substr(0, pos); // get number
		number = std::stof(token);
		str.erase(0, pos + delimiter.length());

		str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); // delete spaces
		str.erase(std::remove(str.begin(), str.end(), ')'), str.end()); // delete ending ')'
		str.erase(std::remove(str.begin(), str.end(), '{'), str.end()); // delete opening '{
		str.erase(std::remove(str.begin(), str.end(), '{'), str.end()); // delete ending '}'
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			vec[i] = std::stof(token);
			std::cout << token << std::endl;
			str.erase(0, pos + delimiter.length());
			i++;
		}

		vec[i] = std::stof(str);
		glm::quat quat(number, vec);
		std::cout << glm::to_string(quat) << std::endl;

		return vec;
	}

	void SceneNode::deserialize(json node_json) {

		active = node_json["active"].template get<bool>();

		ModelMatrix = deserialize_mat4(node_json["ModelMatrix"].template get<std::string>());

		Position = deserialize_vec3(node_json["Position"].template get<std::string>());
		
		Rotation = deserialize_quat(node_json["Rotation"].template get<std::string>());
		
		Scale = deserialize_vec3(node_json["Scale"].template get<std::string>());
		
		setMesh(node_json["Mesh"].template get<std::string>());

		std::cout << "after mesh" << std::endl;
		
		setShaderProgram(node_json["Shader"].template get<std::string>());

		std::cout << "after shader" << std::endl;

		//callbackName = node_json["Callback"].template get<std::string>();
		//setCallBack()
		
		setTextureInfo(node_json["Texture"].template get<std::string>());

		std::cout << "after texture" << std::endl;

		json children = node_json["Children"];

		//std::cout << children.dump() << std::endl;

		for (auto& el : children.items()) {
			std::cout << el.key() << std::endl;
			//if (stoi(el.key()) == 0) {
				SceneNode* child = new SceneNode(0, 0, 0);

				std::cout << el.key() << std::endl;

				child->deserialize(el.value());

				//std::cout << el.key() << " : " << el.value() << "\n";

				addChild(child);
			//}
		}

		std::cout << "after children" << std::endl;


		// vai buscar as suas cenas
		// for children:
		//  create node
		//  call deserialize for node
		//
	}

	

	// normal matrix updated with the transformation matrix
	
}

/*
//// Serialize
// explicit conversion to string
std::string s = j.dump();    // {"happy":true,"pi":3.141}

// serialization with pretty printing
// pass in the amount of spaces to indent
std::cout << j.dump(4) << std::endl;
// {
//     "happy": true,
//     "pi": 3.141
// }

// read a JSON file
std::ifstream i("file.json");
json j;
i >> j;

// write prettified JSON to another file
std::ofstream o("pretty.json");
o << std::setw(4) << j << std::endl;
*/