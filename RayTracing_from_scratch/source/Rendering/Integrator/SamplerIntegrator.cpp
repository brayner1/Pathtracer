#include "pch.h"
#include "RenderHeaders.h"
#include "Rendering/Integrator/SamplerIntegrator.h"

using namespace Renderer;

void SamplerIntegrator::Integrate(Scene scene)
{
	this->InitializeFramebuffer();
}