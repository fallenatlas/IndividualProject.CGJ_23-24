////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "mglCallBack.hpp"

namespace mgl {

    void SillouetteCallBack::beforeDraw(int id) {
        glStencilFunc(GL_NOTEQUAL, static_cast<GLint>(id), 0xFF);
        glStencilMask(0x00);
    }

    void SillouetteCallBack::afterDraw(int id) {
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }

    void StencilCallBack::beforeDraw(int id) {
        glStencilFunc(GL_ALWAYS, static_cast<GLint>(id), 0xFF);
        glStencilMask(0xFF);
    }

    void StencilCallBack::afterDraw(int id) {
        // empty
    }

}
