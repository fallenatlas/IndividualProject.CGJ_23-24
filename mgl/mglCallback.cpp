////////////////////////////////////////////////////////////////////////////////
//
// Camera Abstraction Class
//
// Copyright (c)2023 by Carlos Martinho
//
////////////////////////////////////////////////////////////////////////////////

#include "mglCallBack.hpp"

namespace mgl {

    void SillouetteCallBack::beforeDraw() {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
    }

    void SillouetteCallBack::afterDraw() {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }

    void StencilCallback::beforeDraw() {

    }

    void StencilCallback::afterDraw() {

    }

}
