////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_SILLOUETTE_INFO_HPP
#define MGL_SILLOUETTE_INFO_HPP

#include "./mglCallBack.hpp"
#include "./mglShader.hpp"

namespace mgl {

	class SillouetteInfo;

	class SillouetteInfo {
	public:
		CallBack* callback;
		ShaderProgram* shaderProgram;
		//glm::vec4 Color;

		SillouetteInfo(CallBack* callback, ShaderProgram* shaderProgram) : callback(callback), shaderProgram(shaderProgram) {}
	};
}

#endif // !MGL_SILLOUETTE_INFO_HPP

