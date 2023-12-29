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
		virtual void beforeDraw(int id) = 0;
		virtual void afterDraw(int id) = 0;
	};

	class SillouetteCallBack : public CallBack {
	public:
		void beforeDraw(int id) override;
		void afterDraw(int id) override;
	};

	class StencilCallBack : public CallBack {
	public:
		void beforeDraw(int id) override;
		void afterDraw(int id) override;
	};

}

#endif // !MGL_CALLBACK_HPP

