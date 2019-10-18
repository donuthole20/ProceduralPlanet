#pragma once
#include <vector>
#include <array>
#include <glm.hpp>
class Noise
{
public:
	Noise();
	Noise(int seed);
	float evaluate(glm::vec3 point);
	float evaluateNormalized(glm::vec3 point);
private:
	/// Initial permutation table
	const std::array<int,256> source = {
		151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142,
		8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203,
		117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165,
		71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41,
		55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89,
		18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
		124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
		28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
		129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
		242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
		181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114,
		67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
	};
	const int RandomSize = 256;
	const double Sqrt3 = 1.7320508075688772935;
	const double Sqrt5 = 2.2360679774997896964;
	std::array<int,512> _random;

	/// Skewing and unskewing factors for 2D, 3D and 4D, 
	/// some of them pre-multiplied.
	const double F2 = 0.5 * (Sqrt3 - 1.0);

	const double G2 = (3.0 - Sqrt3) / 6.0;
	const double G22 = G2 * 2.0 - 1;

	const double F3 = 1.0 / 3.0;
	const double G3 = 1.0 / 6.0;

	const double F4 = (Sqrt5 - 1.0) / 4.0;
	const double G4 = (5.0 - Sqrt5) / 20.0;
	const double G42 = G4 * 2.0;
	const double G43 = G4 * 3.0;
	const double G44 = G4 * 4.0 - 1.0;

	std::vector<std::vector<int>> Grad3{
		{1, 1, 0}, {-1, 1, 0}, {1, -1, 0},
		{-1, -1, 0}, {1, 0, 1}, {-1, 0, 1},
		{1, 0, -1}, {-1, 0, -1}, {0, 1, 1},
		{0, -1, 1}, {0, 1, -1}, {0, -1, -1}
	};

	void Randomize(int seed);
	double Dot(std::vector<int> g, double x, double y, double z, double t);
	double Dot(std::vector<int> g, double x, double y, double z);
	double Dot(std::vector<int> g, double x, double y);
	int FastFloor(double x);
	void UnpackLittleUint32(int value, unsigned char* buffer);


};

