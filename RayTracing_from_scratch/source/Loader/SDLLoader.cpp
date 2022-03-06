#include "pch.h"
#include "Loader/SDLLoader.h"
#include "Loader/AssimpUtils.h"
#include "RenderHeaders.h"
#include <string>
#include <filesystem>
#include <fstream>

namespace Renderer
{
	std::optional<RenderOptions> SDLLoader::SDL_LoadScene(std::string file_path, Scene& outScene)
	{
		Assimp::Importer assimpImporter;
		std::filesystem::path fPath {file_path};
		if (!fPath.has_filename())
		{
			std::cout << "ERROR: The provided path is not a valid file path.\n";
			return std::nullopt;
		}

		if (fPath.extension() != ".sdl")
		{
			std::cout << "ERROR: The file provided must have the .sdl extension.\n";
			return std::nullopt;
		}

		std::cout << "Loading file: " << file_path << std::endl;

		std::ifstream sdlFile { fPath };

		if (!sdlFile.is_open())
		{
			std::cout << "ERROR: File couldn't be loaded.\n";
			return std::nullopt;
		}

		std::cout << "File opened successfully\n";
		const std::string filePath = fPath.parent_path().string() + "/";
		RenderOptions inputOptions;
		Eigen::Vector3f cameraEye {0.f, 0.f, -1.f};
		float horizontalFOV = 60.f;
		Eigen::Vector3f cameraTarget {0.f, 0.f, 0.f};

		std::string command;
		uint32_t fileLine = 1;
		for (; !sdlFile.eof(); fileLine++)
		{
			std::string line;
			std::getline(sdlFile, line);
			std::istringstream lineStream {line};
			lineStream >> command;

			if (command.starts_with("#"))
				continue;

			if (command == "object")
			{
				std::string modelFile;
				Eigen::Vector3f objColor;
				float ka, kd, ks, kt, n, ior;

				lineStream >> modelFile >> objColor.x() >> objColor.y() >> objColor.z() >> ka >> kd >> ks >> kt >> n >> ior;

				if (lineStream)
				{
					std::cout << "loading object at path: " << filePath + modelFile << std::endl;
					if (const aiScene* assimpScene = assimpImporter.ReadFile(filePath + modelFile, aiProcessPreset_TargetRealtime_MaxQuality & ~aiProcess_GenSmoothNormals | aiProcess_GenNormals))
					{
						Material* mat = nullptr;
						if (kt > 0.f && ks == 0.f && kd == 0.f)
						{
							mat = new RefractiveMaterial(objColor * kt, ior);
							std::cout << "Refractive material created with color:\n" << mat->GetAlbedo() << std::endl;
						}
						else if (ks > 0.f && kt == 0.f && kd == 0.f)
						{
							mat = new GlossyMaterial(objColor * ks);
							std::cout << "Reflective material created with color:\n" << mat->GetAlbedo() << std::endl;
						}
						else if (kd > 0.f && kt == 0.f && ks == 0.f)
						{
							mat = new DiffuseMaterial(objColor * kd);
							std::cout << "Diffuse material created with color:\n" << mat->GetAlbedo() << std::endl;
						}
						else
						{
							mat = new PhongMaterial(objColor, kd, ks, kt, n, ior);
							std::cout << "Phong material created with color:\n" << mat->GetAlbedo() << std::endl;
							std::cout << "kd: " << kd << "\nks: " << ks << "\nkt: " << kt << "\nior: " << ior << "\n";
						}

						std::vector<Object*> insertedObjs = ConvertAssimpScene(assimpScene, outScene, mat);
						std::cout << "Object loaded containing " << insertedObjs.size() << " meshes;\n";
						for (size_t i = 0; i < insertedObjs.size(); i++)
						{
							std::cout << "Mesh[" << i << "] triangles: " << insertedObjs[i]->GetPrimitiveCount() << std::endl;
						}
					}
					else
					{
						std::cout << "Assimp Error on load:" << std::endl << assimpImporter.GetErrorString() << std::endl;
					}
				}
				else
				{
					std::cout << "Line " << fileLine << " could not be loaded correctly.\n";
				}
			}
			else if (command == "sphere")
			{
				Eigen::Vector3f spherePosition;
				float radius;
				Eigen::Vector3f sphereColor;
				float ka, kd, ks, kt, n, ior;

				lineStream >> spherePosition.x() >> spherePosition.y() >> spherePosition.z() >> radius >> sphereColor.x() >> sphereColor.y() >> sphereColor.z() >> ka >> kd >> ks >> kt >> n >> ior;

				if (lineStream)
				{
					//TODO: Include spheres
				}
				else
				{
					std::cout << "Line " << fileLine << " could not be loaded correctly.\n";
				}
			}
			else if (command == "scene")
			{
				std::string modelFile;

				lineStream >> modelFile;

				if (lineStream)
				{
					std::cout << "loading scene objects at path: " << filePath + modelFile << std::endl;
					if (const aiScene* assimpScene = assimpImporter.ReadFile(filePath + modelFile, aiProcessPreset_TargetRealtime_MaxQuality & ~aiProcess_GenSmoothNormals | aiProcess_GenNormals))
					{
						std::vector<Object*> insertedObjs = ConvertAssimpScene(assimpScene, outScene);
						std::cout << "Scene loaded with " << insertedObjs.size() << " objects.\n";
					}
					else
					{
						std::cout << "Assimp Error on load:" << std::endl << assimpImporter.GetErrorString() << std::endl;
					}
				}
				else
				{
					std::cout << "Line " << fileLine << " could not be loaded correctly.\n";
				}
			}
			else if (command == "light")
			{
				std::string lighFile;
				Eigen::Vector3f lightColor;
				float lightIntensity;

				lineStream >> lighFile >> lightColor.x() >> lightColor.y() >> lightColor.z() >> lightIntensity;

				if (lineStream)
				{
					
					std::cout << "loading light object at path: " << filePath + lighFile << std::endl;
					if (const aiScene* assimpScene = assimpImporter.ReadFile(filePath + lighFile, aiProcessPreset_TargetRealtime_MaxQuality & ~aiProcess_GenSmoothNormals | aiProcess_GenNormals))
					{
						Material* mat = nullptr;
						{
							mat = new DiffuseMaterial(lightColor);
						}
						std::vector<Object*> insertedObjects = ConvertAssimpScene(assimpScene, outScene, mat);
						for (Object* object : insertedObjects)
						{
							for (int i = 0; i < object->GetPrimitiveCount(); ++i)
							{
								Light* areaLight = new PrimitiveLight(lightColor * lightIntensity, object, i);
								outScene.InsertLight(areaLight);
								object->SetPrimitiveLight(i, areaLight);
								object->SetMaterial(nullptr);
								std::cout << "Primitive light created with intensity: " << areaLight->GetColor().transpose() << std::endl;
							}
						}
					}
					else
					{
						std::cout << "Assimp Error on load of light geometry:" << std::endl << assimpImporter.GetErrorString() << std::endl;
					}
				}
			}
			else if (command == "pointlight")
			{
				Eigen::Vector3f lightPosition;
				Eigen::Vector3f lightColor;
				float lightIntensity;

				lineStream >> lightPosition.x() >> lightPosition.y() >> lightPosition.z()
					>> lightColor.x() >> lightColor.y() >> lightColor.z()
					>> lightIntensity;

				if (lineStream)
				{
					Light* newLight = new PointLight(lightColor * lightIntensity, lightPosition);
					outScene.InsertLight(newLight);
					std::cout << "Added point light with intensity: " << newLight->GetColor().transpose() << std::endl;
				}
			}
			else if (command == "eye")
			{
				Eigen::Vector3f camPosition;
				lineStream >> camPosition.x() >> camPosition.y() >> camPosition.z();

				if (lineStream)
				{
					cameraEye = camPosition;
				}

			}
			else if (command == "target")
			{
				Eigen::Vector3f targetPosition;
				lineStream >> targetPosition.x() >> targetPosition.y() >> targetPosition.z();

				if (lineStream)
				{
					cameraTarget = targetPosition;
				}
			}
			else if (command == "fov")
			{
				float horFOV;
				lineStream >> horFOV;

				if (lineStream)
				{
					horizontalFOV = horFOV;
				}
			}
			else if (command == "background")
			{
				Eigen::Vector3f backgroundColor;

				lineStream >> backgroundColor.x() >> backgroundColor.y() >> backgroundColor.z();

				if (lineStream)
				{
					outScene.SetSkyColor(backgroundColor);
				}
			}
			else if (command == "npaths")
			{
				uint32_t numSamples;

				lineStream >> numSamples;

				if (lineStream)
				{
					inputOptions.samplesPerPixel = numSamples;
				}
			}
			else if (command == "size")
			{
				uint32_t width, height;

				lineStream >> width >> height;

				if (lineStream)
				{
					inputOptions.width = width;
					inputOptions.height = height;
				}
			}
			else if (command == "seed")
			{
				int seed;

				lineStream >> seed;

				if (lineStream)
				{
					Renderer::seed = seed;
					std::cout << "Setting rendering seed to: " << seed << std::endl;
				}
			}
			else if (command == "tonemapping")
			{
				float tonemapping;

				lineStream >> tonemapping;

				if (lineStream)
				{
					inputOptions.tonemapping = std::abs(tonemapping);
					std::cout << "Setting tonemapping value to: " << tonemapping << std::endl;
				}
			}
			else if (command == "gamma")
			{
				float gamma;

				lineStream >> gamma;

				if (lineStream)
				{
					inputOptions.gamma = std::abs(gamma);
					std::cout << "Setting gamma to: " << gamma << std::endl;
				}
			}
			else if (command == "output")
			{
				std::string outputFile;

				lineStream >> outputFile;

				if (lineStream)
				{
					inputOptions.outputFileName = outputFile;
				}
			}

			if (sdlFile.fail() && !sdlFile.eof())
			{
				std::cout << "ERROR: failed to load file at line " << fileLine << std::endl;
				return std::nullopt;
			}
		}

		PinholeCamera camera (inputOptions.width.value_or(512), inputOptions.height.value_or(512), horizontalFOV);
		camera.SetPosition(cameraEye);
		camera.LookAt(cameraTarget);

		outScene.SetCamera(camera);

		return inputOptions;
	}
}
