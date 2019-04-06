#pragma once
#include <cstdlib>
#include <stdlib.h>
#include <random>
inline float uniform_random_01()
{
	/// RAND_MAX in <cstdlib>
	return static_cast<float>(rand()) / RAND_MAX - 0.5f;
	//return 0.0f;
}