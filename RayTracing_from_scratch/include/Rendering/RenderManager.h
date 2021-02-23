#pragma once
#include "Rendering/Scene.h"

namespace Renderer
{
	enum ImageType { PPM };

	struct OutputProperties
	{
		Eigen::Vector3f Color;
		Eigen::Vector3f Albedo;
		Eigen::Vector3f Normal;

		struct OutputProperties()
		{
			this->Color = Eigen::Vector3f::Zero();
			this->Albedo = Eigen::Vector3f::Zero();
			this->Normal = Eigen::Vector3f::Zero();
		}
	};

	class RenderManager
	{
	private:
		// Rendering Output Properties
		float gamma;
		float vertical_fov, horizontal_fov, screen_aspect_ratio;
		int width, height;
		int samplesPerPixel;
		int maxDepth;

		bool useDenoiser = true;

		// RGB FrameBuffer Pointer
		int** frameBuffer[3];
		float *colorBuffer, *albedoBuffer, *normalBuffer;

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		RenderManager(int width = 512, int height = 512, float horizontal_field_of_view = 60.0f);

		void RenderScene(Scene& scene, ImageType outputType);


		void ExecuteDenoiser();
	};
}
