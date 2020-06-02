#pragma once
#include "Rendering/Scene.h"

namespace Renderer
{
	enum ImageType { PPM };

	class RenderManager
	{
	private:
		// Rendering Output Properties
		float gamma;
		float vertical_fov, horizontal_fov, screen_aspect_ratio;
		int width, height;
		int samplesPerPixel;
		int maxDepth;

		// RGB FrameBuffer Pointer
		int** frameBuffer[3];

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		RenderManager(int width = 512, int height = 512, float horizontal_field_of_view = 60.0f);

		void RenderScene(Scene scene, ImageType outputType);

	};
}
