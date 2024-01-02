////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_SILLOUETTE_INFO_HPP
#define MGL_SILLOUETTE_INFO_HPP

#include "./glm/glm.hpp"
#include "./mglCallBack.hpp"
#include "./mglShader.hpp"

namespace mgl {

	class SillouetteInfo;

	class SillouetteInfo {
	public:
		CallBack* callback;
		ShaderProgram* shaderProgram;
		glm::vec3 scale;
		//glm::vec4 Color;

		SillouetteInfo(CallBack* callback, ShaderProgram* shaderProgram, glm::vec3 scale) : callback(callback), shaderProgram(shaderProgram), scale(scale) {}
	};
}

#endif // !MGL_SILLOUETTE_INFO_HPP

