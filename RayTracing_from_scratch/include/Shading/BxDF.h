#pragma once
namespace Renderer
{
	enum BxDFType {
		BSDF_REFLECTION = 1 << 0,
		BSDF_TRANSMISSION = 1 << 1,
		BSDF_DIFFUSE = 1 << 2,
		BSDF_GLOSSY = 1 << 3,
		BSDF_SPECULAR = 1 << 4,
		BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR |
		BSDF_REFLECTION | BSDF_TRANSMISSION,
	};

	class Spectrum;
	struct HitInfo;

	class BxDF
	{
	public:
		virtual ~BxDF() {}

		BxDF(BxDFType type) : type(type) {}

		virtual Spectrum f(HitInfo& hit, Eigen::Vector3f& InDir) const = 0;

		/*virtual Spectrum sample_f(HitInfo& hit, Eigen::Vector3f& LightRay, const Eigen::Vector2f* Sample, float* pdf, BxDFType* sampledType = nullptr) const;

		virtual Spectrum reflectance(HitInfo& hit, int nSamples, const Eigen::Vector2f& samples) const;

		virtual Spectrum reflectance(int nSamples, const Eigen::Vector2f& samples1, const Eigen::Vector2f& samples2) const;

		virtual float Pdf(const HitInfo& hit, const Eigen::Vector3f& InRay) const;*/

		bool EqualType(BxDFType t) const { return (type & t) == type; }

		const BxDFType type;
	};

}
