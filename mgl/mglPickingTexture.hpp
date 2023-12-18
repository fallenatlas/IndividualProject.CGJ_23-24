////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_PICKING_TEXTURE_HPP
#define MGL_PICKING_TEXTURE_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace mgl {

	class PickingTexture;

	class PickingTexture {
	public:
		PickingTexture() {}
		~PickingTexture();
		void Init(unsigned int windowWidth, unsigned int windowHeight);
		void EnableWriting();
		void DisableWriting();
		struct PixelInfo {
			uint16_t ObjectID = 0;
			uint16_t DrawID = 0;
			uint16_t PrimID = 0;
		};
		PixelInfo ReadPixel(unsigned int x, unsigned int y);

	private:
		GLuint fbo = 0;
		GLuint pickingTexture = 0;
		GLuint depthTexture = 0;
	};

}

#endif // !MGL_PICKING_TEXTURE_HPP

