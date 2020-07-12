#pragma once
#include "Rendering/Scene.h"

namespace Renderer {

	class Integrator
	{
	public:
		Integrator(const int width = 512, const int height = 512, const bool useDenoiser = true) : Width(width), Height(height), bUseDenoiser(useDenoiser) {}

		virtual void Render(Scene scene) = 0;




		int Width, Height;
		bool bUseDenoiser;

	protected:
		int** frameBuffer[3];
		float* colorBuffer, * albedoBuffer, * normalBuffer;

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();
	};

}
