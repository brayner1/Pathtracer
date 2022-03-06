#include "pch.h"
#include "Object/GroundPlane.h"
#include "RenderHeaders.h"
using namespace Renderer;


GroundPlane::GroundPlane(float height, bool checkerboard) : Height(height), is_checkerboard(checkerboard)
{
	this->material = std::make_shared<DiffuseMaterial>(Eigen::Vector3f(0.0f, 0.0f, 0.0f));
	this->Material2 = new DiffuseMaterial(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
}

GroundPlane::GroundPlane(Eigen::Vector3f color, float height, bool checkerboard):  Height(height), is_checkerboard(checkerboard)
{
	//this->SetColor(intensity);
	this->material = std::make_shared<DiffuseMaterial>(color);
	this->Material2 = new DiffuseMaterial(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
}


GroundPlane::~GroundPlane()
{
	Object::~Object();
	delete Material2;
}

void GroundPlane::SetSquareSize(int size)
{
	this->square_size = size;
}
