#ifndef PTracer_Integrator_h
#define PTracer_Integrator_h
#include "Rendering/Scene.h"

namespace Renderer {

	class Integrator
	{
	public:
		Integrator(const int width = 512, const int height = 512, const bool useDenoiser = true) : Width(width), Height(height), bUseDenoiser(useDenoiser) {}

		virtual void Integrate(const Scene& scene) = 0;


		int Width, Height;
		bool bUseDenoiser;

	protected:
		int** frameBuffer[3];
		float* colorBuffer, * albedoBuffer, * normalBuffer;

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();
	};

	Eigen::Vector3f UniformSampleOneLight(const HitInfo& hit, const Scene& scene);

}

#endif