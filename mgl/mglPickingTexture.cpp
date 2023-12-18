////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "mglPickingTexture.hpp"

namespace mgl {

    PickingTexture::~PickingTexture() {}

    void PickingTexture::Init(unsigned int windowWidth, unsigned int windowHeight) {
		// Create the FBO
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Create the texture object for the primitive information buffer
		glGenTextures(1, &pickingTexture);
		glBindTexture(GL_TEXTURE_2D, pickingTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, windowWidth, windowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pickingTexture, 0);

		// Create the texture object for the depth
		glGenTextures(1, &depthTexture);
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			printf("FB error: status 0x%x\n", status);
			exit(1);
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

	void PickingTexture::EnableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	}

	void PickingTexture::DisableWriting() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	PickingTexture::PixelInfo PickingTexture::ReadPixel(unsigned int x, unsigned int y) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		PixelInfo pixel;
		glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);

		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return pixel;
	}

}

// texture filtering aka sampler
// criar texture extra que e um mipmap
// 
// texture generation -> noise
// for smooth shading use generateSmoothNormals like interpolation of the normals when loading the thing
// 
// step -> 0 or 1 acording to threshold
//
// add specular component
// 
//
//
