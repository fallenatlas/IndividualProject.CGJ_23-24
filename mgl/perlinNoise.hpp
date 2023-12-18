////////////////////////////////////////////////////////////////////////////////
//
// CallBack Abstraction Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MGL_PERLIN_NOISE_HPP
#define MGL_PERLIN_NOISE_HPP

#include <GL/glew.h>

#include <vector>

namespace mgl {

	class PerlinNoiseGenerator;

	class PerlinNoiseGenerator {
	public:
		PerlinNoiseGenerator();
		~PerlinNoiseGenerator();
		double noise(double x, double y, double z);

	private:
		 std::vector<int> p;

		 double fade(double t);
		 double lerp(double t, double a, double b);
		 double grad(int hash, double x, double y, double z);
	};

}

#endif // !MGL_PERLIN_NOISE_HPP

