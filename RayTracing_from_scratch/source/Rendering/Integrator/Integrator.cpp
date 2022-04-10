#include "pch.h"
#include "Rendering/Integrator/Integrator.h"
#include "RenderHeaders.h"

using namespace Renderer;

void Integrator::SaveBufferToPPM(std::string out_file_path)
{
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->Width << " " << this->Height << " 255";
	std::cout << "starting to write image" << std::endl;
	for (int y = Height - 1; y >= 0; y--)
	{
		for (int x = Width - 1; x >= 0; x--)
		{
			//std::cout << "writing pixel: " << x << ", " << y << std::endl;
			outfile << " " << frameBuffer[0][x][y] << " " << frameBuffer[1][x][y] << " " << frameBuffer[2][x][y];
		}
	}
	outfile.close();
}

void Integrator::InitializeFramebuffer()
{
	int w = Width; int h = Height;
	for (size_t i = 0; i < 3; i++)
	{
		frameBuffer[i] = new int* [w];
		for (size_t j = 0; j < w; j++)
		{
			frameBuffer[i][j] = new int[h];
			for (size_t k = 0; k < h; k++)
			{
				frameBuffer[i][j][k] = 0;
			}
		}
	}

	this->colorBuffer = new float[w * h * 3];
	this->albedoBuffer = new float[w * h * 3];
	this->normalBuffer = new float[w * h * 3];
}

float PowerHeuristic(float nf, float fPdf, float ng, float gPdf)
{
	float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}

Eigen::Vector3f EstimateDirectLighting(const Light* light, const HitInfo& hit, const Scene& scene)
{
	Eigen::Vector3f Li { Eigen::Vector3f::Zero() };
	float lightPdf = 0.f;
	Eigen::Vector3f lightDir = Eigen::Vector3f::Zero();
	const Eigen::Vector3f lightIntensity = light->SampleLightIntensity(scene, hit, lightDir, lightPdf);

	if (!lightIntensity.isZero() && lightPdf > 0.f)
	{
		Eigen::Vector3f bsdf = hit.Material->BSDF(hit.rayDir, lightDir, hit) * abs(lightDir.dot(hit.shadNormal));
		float scattPdf = hit.Material->PDF(hit.rayDir, lightDir, hit);
		float weight = 1.f;
		if (!light->IsDeltaLight())
			weight = PowerHeuristic(1.f, lightPdf, 1.f, scattPdf);
		Li = (lightIntensity.array() * bsdf.array() * weight) / lightPdf;
	}

	return Li;
}

Eigen::Vector3f Renderer::UniformSampleOneLight(const HitInfo& hit, const Scene& scene)
{
	const std::vector<Light*>& scene_lights { scene.GetLights() };
	Eigen::Vector3f Li { Eigen::Vector3f::Zero() };
	if (scene_lights.size() > 0)
	{
		int light_idx = uniform_random_int(0, scene_lights.size() - 1);
		const Light* light = scene_lights[light_idx];
		Li = scene_lights.size() * EstimateDirectLighting(light, hit, scene);
	}

	return Li;
}

