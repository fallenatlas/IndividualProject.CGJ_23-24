////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_CALLBACK_HPP
#define MGL_CALLBACK_HPP

#include <GL/glew.h>

namespace mgl {

	class CallBack;

	class CallBack {
	public:
		virtual void beforeDraw() = 0;
		virtual void afterDraw() = 0;
	};

	class SillouetteCallBack : public CallBack {
	public:
		void beforeDraw() override;
		void afterDraw() override;
	};

	class StencilCallback : public CallBack {
	public:
		void beforeDraw() override;
		void afterDraw() override;
	};

}

#endif // !MGL_CALLBACK_HPP

