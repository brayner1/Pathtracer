#pragma once
#include "Rendering/Integrator/Integrator.h"

namespace Renderer
{
    class Spectrum;
    class SamplerIntegrator :
        public Integrator
    {
    public:
        SamplerIntegrator(const int width = 512, const int height = 512, const bool useDenoiser = true) : Integrator(width, height, useDenoiser) {}

        void Render(Scene scene);

        virtual Spectrum SampleRadiance(const Ray& ray, const Scene& scenes, int depth = 0) = 0;



    };

}
