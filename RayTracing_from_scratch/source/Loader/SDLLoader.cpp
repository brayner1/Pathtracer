#include "pch.h"
#include "Loader/SDLLoader.h"
#include "Loader/AssimpUtils.h"
#include "RenderHeaders.h"
#include <string>
#include <filesystem>
#include <fstream>

namespace Renderer
{
	std::optional<SDLInputOptions> SDLLoader::SDL_LoadScene(std::string file_path, Scene& outScene)
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

		std::cout << "File loaded successfully\n";
		const std::string filePath = fPath.parent_path().string() + "/";
		SDLInputOptions inputOptions;
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
				float ka, kd, ks, kt, n;

				lineStream >> modelFile >> objColor.x() >> objColor.y() >> objColor.z() >> ka >> kd >> ks >> kt >> n;

				if (lineStream)
				{
					std::cout << "loading object at path: " << filePath + modelFile << std::endl;
					if (const aiScene* assimpScene = assimpImporter.ReadFile(filePath + modelFile, aiProcessPreset_TargetRealtime_MaxQuality & ~aiProcess_GenSmoothNormals | aiProcess_GenNormals))
					{
						Material* mat = nullptr;
						if (kt > 0.f)
						{
							mat = new RefractiveMaterial(objColor, kt);
							std::cout << "Refractive object created\n";
						}
						else if (ks > 0.f)
						{
							mat = new GlossyMaterial(objColor);
							std::cout << "Reflective object created\n";
						}
						else
						{
							mat = new DiffuseMaterial(objColor);
							std::cout << "Diffuse object created\n";
						}

						ConvertAssimpScene(assimpScene, outScene, mat);
					}
					else
					{
						std::cout << "Assimp Error on load:" << std::endl << assimpImporter.GetErrorString() << std::endl;
					}
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
					//TODO: load model and assign it as light source
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
					//TODO: Define rng seed
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

		PinholeCamera camera (inputOptions.width, inputOptions.height, horizontalFOV);
		camera.SetPosition(cameraEye);
		camera.LookAt(cameraTarget);

		outScene.SetCamera(camera);

		return inputOptions;
	}
}
