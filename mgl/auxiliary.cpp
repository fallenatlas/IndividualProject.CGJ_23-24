#include "auxiliary.hpp"

namespace aux {

	glm::vec3 deserialize_vec3(std::string str) {
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
			str.erase(0, pos + delimiter.length());
			i++;
		}

		vec[i] = std::stof(str);

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

		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			mat[col][row] = std::stof(token);
			str.erase(0, pos + delimiter.length());
			row++;
			if (row == 4) {
				col++;
				row = 0;
			}
		}

		mat[col][row] = std::stof(str);

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
			str.erase(0, pos + delimiter.length());
			i++;
		}

		vec[i] = std::stof(str);
		glm::quat quat(number, vec);

		return quat;
	}

}
