#pragma once
#include <cstdlib>
#include <stdlib.h>
#include <random>
inline float uniform_random_01()
{
	/// RAND_MAX in <cstdlib>
	return static_cast<float>(rand()) / RAND_MAX;
	//return 0.0f;
}

inline float uniform_random_minus11()
{
	/// RAND_MAX in <cstdlib>
	return 2.0f*(static_cast<float>(rand()) / RAND_MAX - 0.5f);
	//return 0.0f;
}