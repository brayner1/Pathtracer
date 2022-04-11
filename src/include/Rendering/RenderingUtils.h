#ifndef PTracer_RenderingUtils_h
#define PTracer_RenderingUtils_h
  
namespace Renderer
{
	inline Eigen::Affine3f Scaling(float x, float y, float z)
	{
		float mat_arr[16] =
		{
			x,		0.f,	0.f,	0.f,
			0.f,	y,		0.f,	0.f,
			0.f,	0.f,	z,		0.f,
			0.f,	0.f,	0.f,	1.f
		};

		return Eigen::Affine3f{ Eigen::Matrix4f{ mat_arr } };
	}

	
	// Illumination Utilities Functions

	inline float FrDieletric(const float CosI, const float CosT, const float EtaI, const float EtaT)
	{
		const float etaI_cosI = (EtaI * CosI);
		const float etaT_cosT = (EtaT * CosT);
		const float rPerp = (etaI_cosI - etaT_cosT) / (etaI_cosI + etaT_cosT);

		const float etaT_cosI = (EtaT * CosI);
		const float etaI_cosT = (EtaI * CosT);
		const float rParl = (etaT_cosI - etaI_cosT) / (etaT_cosI + etaI_cosT);

		return (rParl * rParl + rPerp * rPerp) / 2.0f;
		/*float Rperp = ((EtaI * CosI) - (EtaT * CosT)) / ((EtaI * CosI) + (EtaT * CosT));
		float Rparl = ((EtaT * CosI) - (EtaI * CosT)) / ((EtaT * CosI) + (EtaI * CosT));

		return (Rparl * Rparl + Rperp * Rperp) / 2.0f;*/
	}
}
#endif
