////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef AUXILIARY_HPP
#define AUXILIARY_HPP

#include "./glm/glm.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <vector>
#include <algorithm>

namespace aux {

	glm::vec3 deserialize_vec3(std::string str);
	glm::mat4 deserialize_mat4(std::string str);
	glm::quat deserialize_quat(std::string str);

}

#endif // !AUXILIARY_HPP

