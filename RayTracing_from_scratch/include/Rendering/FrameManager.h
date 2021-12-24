#pragma once
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
		std::optional<bool> useDenoiser = std::nullopt;
	};

	struct OutputProperties
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
		float gamma;
		uint32_t width, height;
		uint32_t samplesPerPixel;
		uint32_t maxDepth;
		bool useDenoiser = true;

		// RGB FrameBuffer Pointer
		int** frameBuffer[3];
		float *colorBuffer, *albedoBuffer, *normalBuffer;

		void SaveBufferToPPM(std::string out_file_path);

		void InitializeFramebuffer();

		void ExecuteDenoiser();
	};
}
