#include "perlinNoise.hpp"

namespace mgl {

	
	PerlinNoiseGenerator::PerlinNoiseGenerator() {
		permutations = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
			8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
			35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
			134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
			55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
			18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
			250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
			189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
			43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
			97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
			107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 
		};
		// Duplicate the permutation vector
		permutations.insert(permutations.end(), permutations.begin(), permutations.end());
	}

	PerlinNoiseGenerator::~PerlinNoiseGenerator() {}

	double PerlinNoiseGenerator::noise(double x, double y, double z) {
		// Find the unit cube that contains the point
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;
		int Z = (int)floor(z) & 255;

		// Find relative x, y,z of point in cube
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		// Compute fade curves for each of x, y, z
		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		// Hash coordinates of the 8 cube corners
		int A = permutations[X] + Y;
		int AA = permutations[A] + Z;
		int AB = permutations[A + 1] + Z;
		int B = permutations[X + 1] + Y;
		int BA = permutations[B] + Z;
		int BB = permutations[B + 1] + Z;

		// Generate hash values for each point of the unit-cube.
		int const h000 = permutations[permutations[permutations[X + 0] + Y + 0] + Z + 0];
		int const h001 = permutations[permutations[permutations[X + 0] + Y + 0] + Z + 1];
		int const h010 = permutations[permutations[permutations[X + 0] + Y + 1] + Z + 0];
		int const h011 = permutations[permutations[permutations[X + 0] + Y + 1] + Z + 1];
		int const h100 = permutations[permutations[permutations[X + 1] + Y + 0] + Z + 0];
		int const h101 = permutations[permutations[permutations[X + 1] + Y + 0] + Z + 1];
		int const h110 = permutations[permutations[permutations[X + 1] + Y + 1] + Z + 0];
		int const h111 = permutations[permutations[permutations[X + 1] + Y + 1] + Z + 1];

		// Linearly interpolate between dot products of each gradient with its distance to the input location.
		double const x11 = lerp(grad(h000, x, y, z), grad(h100, x - 1, y, z), u);
		double const x12 = lerp(grad(h010, x, y - 1, z), grad(h110, x - 1, y - 1, z), u);
		double const x21 = lerp(grad(h001, x, y, z - 1), grad(h101, x - 1, y, z - 1), u);
		double const x22 = lerp(grad(h011, x, y - 1, z - 1), grad(h111, x - 1, y - 1, z - 1), u);

		double const y1 = lerp(x11, x12, v);
		double const y2 = lerp(x21, x22, v);

		return lerp(y1, y2, w);
	}
	
	double PerlinNoiseGenerator::fade(double t) {
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	double PerlinNoiseGenerator::lerp(double a, double b, double t) {
		return a + t * (b - a);
	}

	double PerlinNoiseGenerator::grad(int hash, double x, double y, double z) {
		switch (hash & 0xF) {
			case 0x0: return  x + y;
			case 0x1: return -x + y;
			case 0x2: return  x - y;
			case 0x3: return -x - y;
			case 0x4: return  x + z;
			case 0x5: return -x + z;
			case 0x6: return  x - z;
			case 0x7: return -x - z;
			case 0x8: return  y + z;
			case 0x9: return -y + z;
			case 0xA: return  y - z;
			case 0xB: return -y - z;
			case 0xC: return  y + x;
			case 0xD: return -y + z;
			case 0xE: return  y - x;
			case 0xF: return -y - z;
			default:  return  0.0;
		}
	}

}
