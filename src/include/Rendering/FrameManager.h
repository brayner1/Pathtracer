#ifndef PTracer_FrameManager_h
#define PTracer_FrameManager_h
#include "Rendering/Scene.h"
#include <optional>

namespace Renderer
{
	enum ImageType { PPM };

	struct RenderOptions
	{
		std::optional<uint32_t> width = std::nullopt, height = std::nullopt;
		std::optional<uint32_t> samplesPerPixel = std::nullopt;
		std::optional<uint32_t> maxDepth = std::nullopt;
		std::optional<float> horizontalFOV = std::nullopt;
		std::optional<float> gamma = std::nullopt;
		std::optional<float> exposure = std::nullopt;
		std::optional<bool> useDenoiser = std::nullopt;
		std::optional<std::string> outputFileName = std::nullopt;
	};

	struct alignas(64) OutputProperties
	{
		Eigen::Vector3f color;
		Eigen::Vector3f albedo;
		Eigen::Vector3f normal;

		OutputProperties()
		{
			this->color = Eigen::Vector3f::Zero();
			this->albedo = Eigen::Vector3f::Zero();
			this->normal = Eigen::Vector3f::Zero();
		}
	};

	class FrameManager
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		FrameManager(int width = 512, int height = 512);
		FrameManager(const RenderOptions& renderOptions);

		FrameManager(const FrameManager&) = delete;
		FrameManager(FrameManager&&) = delete;

		void SetOptions(const RenderOptions& renderOptions);

		void RenderScene(Scene& scene, ImageType outputType);

	private:
		// Rendering Output Properties
		std::string outFile = "output";
		
		float gamma = 2.2f;
		float exposure = 1.f;
		uint32_t width = 512, height = 512;
		uint32_t samplesPerPixel = 256;
		uint32_t maxDepth = 8;
		bool useDenoiser = false;

		// RGB FrameBuffer Pointer
		bool frameBufferInitialized = false;
		int** frameBuffer[3];
		float *colorBuffer, *albedoBuffer, *normalBuffer;

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();

		void ExecuteDenoiser();
	};
}

#endif
