// RayTracing_from_scratch.cpp : Este arquivo contém a função 'main'. A execução do programa começa e termina ali.
//

#include "pch.h"
#include "RayTracing_from_scratch.h"
#include "RenderHeaders.h"
#include <ctime>
#include <random>

int square_size = 2;

void SaveVerticesInFile(Renderer::Scene& scene, std::string file_path);
void SaveTrianglesInFile(Renderer::Scene& scene, std::string file_path);

int main()
 {
	std::cout << Eigen::SimdInstructionSetsInUse() << std::endl;

	//EIGEN_MAJOR_VERSION
	std::srand((unsigned int)time(0));
	Renderer::PinholeCamera cam;// (256, 256);
	
	// Cubes camera
	//cam.SetPosition(Eigen::Vector3f(5.50f, 3.0f, 0.0f));
	//cam.LookAt(Eigen::Vector3f(0.0f, 3.0f, 0.0f));

	// Monkey Camera
	/*cam.SetPosition(Eigen::Vector3f(7.0f, 2.3f, 0.0f));
	cam.LookAt(Eigen::Vector3f(0.0f, 2.3f, 0.0f));*/

	// Dragon Camera
	/*Eigen::Vector3f camPosition{-0.9f, 0.03f, -0.46f};
	const Eigen::Vector3f front{ 0.9063077569007874f, 1.4901161193847656e-08f, 0.4226183593273163f };
	const Eigen::Vector3f lookP = camPosition + front;
	cam.SetPosition(camPosition);
	cam.lookAt(lookP);*/


	const std::string filename {"Scenes/Cubes.obj"};
	//const std::string filename {"Scenes/Monkey.obj"};
	//const std::string filename {"Scenes/dragon.obj"};
	Renderer::Scene scene;
	//Renderer::AssimpLoader::Assimp_LoadScene(filename, scene);
	std::cout << "Assimp loaded" << std::endl;
	//loader.ConvertAssimpToScene(scene);
	std::optional<Renderer::SDLInputOptions> inputOptions = Renderer::SDLLoader::SDL_LoadScene("Scenes/sdl_cornell/cornellroom.sdl", scene);
	if (!inputOptions)
	{
		std::cout << "SDL file failed to load. Exiting program.";
		return -1;
	}
	std::cout << "Scene converted" << std::endl;
	scene.BuildSceneTree();
	std::cout << "Scene hierarchy built" << std::endl;

	SaveVerticesInFile(scene, "Log/vertices.txt");
	SaveTrianglesInFile(scene, "Log/triangles.txt");
	//scene.SetCamera(cam);

	// Cubes Ligh:
	//Renderer::Light* newlight = new Renderer::PointLight((Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 40.f * M_1_PI) / 4.f, {-1.0f, 5.0f, 0.0f});

	// Monkey Ligh:
	//Renderer::Light* newlight = new Renderer::PointLight((Eigen::Vector3f{1.0f, 1.0f, 1.0f} * 40.f * M_1_PI) / 4.f, {2.25f, 2.40f, 0.0f});

	//scene.InsertLight(newlight);


	// Dragon Lights
	/*{
		Renderer::Light* newlight = new Renderer::PointLight((Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 25.f * M_1_PI) / 4.f, {0, 0.6f, -0.8f});
		scene.insertLight(newlight);
		newlight = new Renderer::PointLight((Eigen::Vector3f(1.0f, 1.0f, 1.0f) * 25.f * M_1_PI) / 4.f, {-1.f, 0.4f, 0.2f});
		scene.insertLight(newlight);
	}*/

	
	//Renderer::Texture* nm = Renderer::Texture::LoadTextureFromFile("whiteBrick.jpg");
	//Renderer::Object* obj1 = scene.getObjects()[1];
	//obj1->getMaterial()->setAlbedoTexture(nm);

	//obj1 = scene.getObjects()[1];
	//obj1->SetMaterial(new Renderer::RefractiveMaterial(obj1->getMaterial()->getDiffuse()));
	//obj1->getMaterial()->setDiffuse(Eigen::Vector3f::Ones());
	//((Renderer::GlossyMaterial*)obj1->getMaterial())->setRoughness(0.8f);

	std::cout << "rendering" << std::endl;

	//scene.renderSceneOnPPM("img.ppm");
	Renderer::FrameManager RM = Renderer::FrameManager(inputOptions->width, inputOptions->height);
	{
		Renderer::RenderOptions options;
		options.maxDepth = inputOptions->maxDepth;
		options.samplesPerPixel = inputOptions->samplesPerPixel;

		RM.SetOptions(options);
	}


	RM.RenderScene(scene, Renderer::ImageType::PPM);
}

void SaveVerticesInFile(Renderer::Scene& scene, std::string file_path)
{
	std::ofstream file(file_path);

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

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
