#ifndef PTracer_RGBTexture_h
#define PTracer_RGBTexture_h

namespace Renderer {
	class RGBTexture
	{
		friend class TextureManager;
	private:
		static int numTextures;

		unsigned char* data;
		unsigned int w, h, c;

		std::string texture_file{};
	public:
		RGBTexture();
		RGBTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char* data, std::string texture_name = "");

		RGBTexture(RGBTexture&& other) noexcept;

		~RGBTexture();

		[[nodiscard]]
		Eigen::Vector3i GetColor(uint32_t x, uint32_t y) const;
		[[nodiscard]]
		Eigen::Vector3i GetColorUV(float u, float v) const;


		void SetTexture(unsigned int Width, unsigned int Height, unsigned int Channels, unsigned char* texData);

		static RGBTexture* LoadTexturePtrFromFile(std::string path);
		static RGBTexture LoadTextureFromFile(std::string path);
		void SaveTextureOnFile(std::string path) const;

		
	};
}

#endif