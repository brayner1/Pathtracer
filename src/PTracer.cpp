// PTracer.cpp: define o ponto de entrada para o aplicativo.
//

#include "PTracer.h"
#include "pch.h"
//#include "RayTracing_from_scratch.h"
#include "RenderHeaders.h"
#include <ctime>
#include <random>
#include <filesystem>

void SaveVerticesInFile(Renderer::Scene& scene, std::string file_path);
void SaveTrianglesInFile(Renderer::Scene& scene, std::string file_path);

int main(int argc, char* argv[])
 {
	std::cout << "Supported SIMD instruction sets:\n" << Eigen::SimdInstructionSetsInUse() << std::endl;

	std::string file_path = "Scenes/sdl_cornell/cornellroom.sdl";
	if (argc > 1)
	{
		std::string tmp{ argv[1] };
		std::filesystem::path argFile{ tmp };
		if (argFile.has_filename() && argFile.extension() == ".sdl")
		{
			file_path = tmp;
		}
	}
	Renderer::Scene scene;
	std::optional<Renderer::RenderOptions> inputOptions = Renderer::SDLLoader::SDL_LoadScene(file_path, scene);
	if (!inputOptions)
	{
		std::cout << "SDL file failed to load. Exiting program.";
		return -1;
	}
	std::cout << "\nScene file loaded and converted" << std::endl;
	std::cout << "The scene has " << scene.GetNumberOfVertices() << " vertices and " << scene.GetNumberOfTriangles() << " triangles.\n\n";

	std::cout << "Building Bounding Volume Hierarchy" << std::endl;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	scene.BuildSceneTree();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Scene Bounding Volume Hierarchy built" << std::endl;
	std::cout << "BVH build time = " << (float)std::chrono::duration_cast<
		std::chrono::microseconds>(end - begin).count() / 1000000.f << "[s]\n" << std::endl;

	//SaveVerticesInFile(scene, "Log/vertices.txt");
	//SaveTrianglesInFile(scene, "Log/triangles.txt");

	std::cout << "Rendering Started" << std::endl;

	Renderer::FrameManager frame_manager = Renderer::FrameManager(inputOptions.value());
	//frame_manager.SetOptions(inputOptions.value());

	frame_manager.RenderScene(scene, Renderer::ImageType::PPM);
}

void SaveVerticesInFile(Renderer::Scene& scene, std::string file_path)
{
	std::ofstream file(file_path);

	std::cout << "Saving vertices in log file\n";

	const std::vector<Renderer::Object*>& objects = scene.GetObjects();

	uint32_t count = 0;
	uint32_t n_objects = objects.size();
	for (int i = 0; i < n_objects; ++i)
	{
		if (Renderer::Mesh* mesh = dynamic_cast<Renderer::Mesh*>(objects[i]))
		{
			uint32_t n_vertices = mesh->vertices.size();
			for (int j = 0; j < n_vertices; ++j)
			{
				file << "vertex["<<count++<<"]: " << mesh->vertices[j].transpose() << std::endl;
			}
		}
	}
	file.close();
}

void SaveTrianglesInFile(Renderer::Scene& scene, std::string file_path)
{
	std::ofstream file(file_path);

	std::cout << "Saving triangles to log file\n";

	const std::vector<Renderer::Object*>& objects = scene.GetObjects();

	uint32_t count = 0;
	uint32_t n_objects = objects.size();
	for (int i = 0; i < n_objects; ++i)
	{
		if (Renderer::Mesh* mesh = dynamic_cast<Renderer::Mesh*>(objects[i]))
		{
			uint32_t n_indices = mesh->triangles.size();
			for (int j = 0; j < n_indices; ++j)
			{
				const Eigen::Vector3i& indices = mesh->triangles[j];
				file << "Triangle["<<count++<<"]:\n\tv0 = { " << mesh->vertices[indices.x()].transpose() << 
            " }\n\tv1 = { " << mesh->vertices[indices.y()].transpose() << " }\n\tv2 = { " << mesh->vertices[indices.z()].transpose() << " }" << std::endl;
			}
		}
	}
	file.close();
}
//#include "Eigen"
//
//using namespace std;
//
//int main()
//{
//	cout << "Hello CMake." << endl;
//	cout << Eigen::Vector3f::Zero() << endl;
//	return 0;
//}
