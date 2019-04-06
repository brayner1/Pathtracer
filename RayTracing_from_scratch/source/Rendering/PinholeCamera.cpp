#include "pch.h"
#include "Rendering/PinholeCamera.h"
#include "Renderer.h"
//#include "RandUtil.h"

using namespace Renderer;

Eigen::Vector3f PinholeCamera::get_sky_colour(Eigen::Vector3f ray_dir)
{
	//float t = (ray_dir.y() + std::sinf((this->vertical_fov)*M_PI / 180.0f)/1.2f)/std::sinf((this->vertical_fov)*M_PI / 180.0f);
	float t = (ray_dir.y() + 1.0f) / 2.0f;
	return t*Eigen::Vector3f(0.5f, 0.7f, 1.0f) + (1.0f - t)*(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
	//return (t*t)*(Eigen::Vector3f(1.0f, 1.0f, 1.0f)) - (-1.0f - t)*(1.0f - t)*Eigen::Vector3f(0.5f, 0.7f, 1.0f);
}

bool PinholeCamera::renderSceneOnPPM(std::string out_file_path, Scene scene)//std::vector<Object*> scene_objects)
{
	std::cout << "lights: " << scene.getLights().size() << std::endl;
	updateViewMatrix();
	std::ofstream outfile(out_file_path);
	outfile << "P3 " << this->width << " " << this->height << " 255";
	int** frameBuffer[3];
	for (size_t i = 0; i < 3; i++)
	{
		frameBuffer[i] = new int*[width];
		for (size_t j = 0; j < width; j++)
		{
			frameBuffer[i][j] = new int[height];
			for (size_t k = 0; k < height; k++)
			{
				frameBuffer[i][j][k] = 0;
			}
		}
	}
#ifdef paralellism
	omp_set_num_threads(omp_get_max_threads());
	std::cout << "max threads: " << omp_get_max_threads() << std::endl;
	//std::cout << "started parallel region with " << omp_get_num_threads() << " threads" << std::endl;
	#pragma omp parallel for
#endif // paralellism
	for (int y = height / 2; y >= -(height / 2) + ((height + 1) % 2); --y) {    /// Rows
#ifdef paralellism
		#pragma omp parallel for
#endif // paralellism
		for (int x = -(width / 2) + ((width + 1) % 2); x <= width / 2; ++x) {  /// Columns

			Eigen::Vector3f final_color(0.0f, 0.0f, 0.0f);
			if(x == -255 && y == 256)
				std::cout << "started parallel region with " << omp_get_num_threads() << " threads" << std::endl;
			//srand((123456789+(x*y)) / (omp_get_thread_num()+1) + omp_get_max_threads());
			for (int i = 0; i < this->AA_MS; i++) {
				//Eigen::Vector3f ray_offset(uniform_random_01()*0.01f, uniform_random_01()*0.01f, 0.0f);
				Eigen::Vector3f ray_direction = (
					Eigen::Vector3f(
						std::sinf((this->horizontal_fov)*M_PI/180.0f) * ((float)x - 0.5f + uniform_random_01()) / ((float)width),
						std::sinf((this->vertical_fov)*M_PI/180.0f) * ((float)y - 0.5f + uniform_random_01()) / ((float)height),
						1.0f
					).normalized()
				);
				float ray_energy_left = 1.0f;
				Eigen::Vector3f pixel_color(0.0f, 0.0f, 0.0f);
				int numRays = 1;
				std::vector<Ray> rays;
				ray_direction = this->view_matrix.transpose().block<3,3>(0, 0) * ray_direction;
				rays.push_back(Ray(this->position /*+ ray_offset*/, ray_direction));
				std::vector<Object*> scene_objects = scene.getObjects();
				while (!rays.empty()) {
					Ray ray = *rays.begin()._Ptr;
					rays.erase(rays.begin());
					float min_dist = std::numeric_limits<float>::max();
					HitInfo closest_hit = HitInfo::resetStruct();
					

					bool has_hit = false;
					for (int i = 0; i < scene_objects.size(); i++) {
						HitInfo hit_info;
						if (scene_objects[i]->is_hit_by_ray(ray, hit_info)) {
							if (hit_info.Distance <= min_dist) {
								has_hit = true;
								closest_hit = hit_info;
								min_dist = hit_info.Distance;
							}
						}
					}
					closest_hit.x = x; closest_hit.y = y;
					closest_hit.w = width; closest_hit.h = height;
					if (has_hit) {
						if (closest_hit.Material) {
							closest_hit.Material->get_hit_color(scene, closest_hit/*, x + width / 2 - 1, y + height / 2 - 1*/);
							Eigen::Vector3f ray_color = closest_hit.Color * ray_energy_left * (1.0f - closest_hit.Material->getReflectivity());
#if defined DEBUG
							if (x + width / 2 - 1 == 282 && y + height / 2 - 1 == 292) {
								std::cout << "name: " << closest_hit.obj->name << std::endl;
								std::cout << closest_hit.Color.x() << ", " << closest_hit.Color.y() << ", " << closest_hit.Color.z() << std::endl;
								//std::cout << ray_color.x() << ", " << ray_color.y() << ", " << ray_color.z() << std::endl;
								std::cout << closest_hit.Material->getDiffuse().x() << ", " << closest_hit.Material->getDiffuse().y() << ", " << closest_hit.Material->getDiffuse().z() << std::endl;
								std::cout << "ray energy: " << ray_energy_left << std::endl;
							}
#endif
							final_color += ray_color;
							ray_energy_left *= closest_hit.Material->getReflectivity();
#if defined DEBUG
							if (x + width / 2 - 1 == 282 && y + height / 2 - 1 == 292)
								std::cout << "ray energy: " << ray_energy_left << std::endl;
#endif
						}
						else {
							final_color += closest_hit.Color * ray_energy_left;
							ray_energy_left = 0.0f;
						}
						if (ray_energy_left <= 0.0f)
							break;
					}
					else {
						final_color += get_sky_colour(ray_direction) * ray_energy_left;
					}
					/*if (x + width / 2 - 1 == 282 && y + height / 2 - 1 == 328)
						std::cout << "ray energy: " << ray_energy_left << std::endl;*/
				
					for(Ray var : closest_hit.outgoing_rays)
					{
						if (var.getDepth() <= this->maxBounces) {
							rays.push_back(var);
							numRays++;
						}
					}
				}
			}
			
			final_color = (final_color*(1.0f/(float)this->AA_MS)).cwiseMin(Eigen::Vector3f(1.0f, 1.0f, 1.0f));
			//if (x + width / 2 - 1 == 315 && y + height / 2 - 1 == 140) {
			//	std::cout << "rays: " << numRays << std::endl;
			//	std::cout << "cor: " << pixel_color << std::endl;
			//}
#ifdef paralellism
	#pragma omp critical
			{
				frameBuffer[0][x+width/2-1][y+height/2-1] = (int)(final_color.x() * 255);
				frameBuffer[1][x+width/2-1][y+height/2-1] = (int)(final_color.y() * 255);
				frameBuffer[2][x+width/2-1][y+height/2-1] = (int)(final_color.z() * 255);
			}
#else
			 outfile << " " << (int)(final_color.x() * 255) << " " << (int)(final_color.y() * 255) << " " << (int)(final_color.z() * 255);
#endif // paralellism
		}
	}
#ifdef paralellism
	std::cout << "starting to write image" << std::endl;
	for (int y = height-1; y >= 0; y--)
	{
		for (int x = 0; x < width; x++)
		{
			//std::cout << "writing pixel: " << x << ", " << y << std::endl;
			outfile << " " << frameBuffer[0][x][y] << " " << frameBuffer[1][x][y] << " " << frameBuffer[2][x][y];
		}
	}
#endif // paralellism

	outfile.close();
	return false;
}

PinholeCamera::PinholeCamera(int width, int height, float horizontal_field_of_view) : 
	width(width), height(height), horizontal_fov(horizontal_field_of_view)
{
	this->AA_MS = 128;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
	this->vertical_fov = this->horizontal_fov / this->screen_aspect_ratio;
	this->maxBounces = 3;
	this->camera_dirty = false;
	lookAt(this->position + Eigen::Vector3f(0.0f, 0.0f, 1.0f));
}

PinholeCamera::~PinholeCamera()
{
}

const int PinholeCamera::getWidth() const
{
	return this->width;
}

const int PinholeCamera::getHeight() const
{
	return this->height;
}

void PinholeCamera::setScreenSize(int width, int height)
{
	this->width = width;
	this->height = height;
	this->screen_aspect_ratio = ((float)this->width) / ((float)this->height);
}

Eigen::Vector3f PinholeCamera::getPosition() const
{
	return Eigen::Vector3f(this->position);
}

void PinholeCamera::setPosition(Eigen::Vector3f new_position)
{
	this->camera_dirty = true;
	this->position = new_position;
}

void PinholeCamera::goForward(float amount)
{
	this->camera_dirty = true;
	this->position += this->front * amount;
}

void PinholeCamera::lookAt(Eigen::Vector3f target)
{
	this->camera_dirty = true;
	this->front = (target - this->position).normalized();
	//std::cout << "front vec: " << std::endl << this->front << std::endl;
	//this->front = (this->position - target).normalized();
	this->right = (Eigen::Vector3f(0.0, 1.0, 0.0).cross(this->front)).normalized();
	//std::cout << "right vec: " << std::endl << this->right << std::endl;
	//this->right = (this->front.cross(Eigen::Vector3f(0.0, 1.0, 0.0))).normalized();
	this->up = (this->front.cross(this->right)).normalized();
	//std::cout << "up vec: " << std::endl << this->up << std::endl;
	//this->up = (this->right.cross(this->front)).normalized();
}

void PinholeCamera::updateViewMatrix() {
	if (!this->camera_dirty)
		return;
	// Calcula a matriz de visualização utilizando a matriz de mudança de coordenadas
	const float viewMat[] = {
		this->right.x()                         , this->up.x()                         , this->front.x()                       , 0,
		this->right.y()							, this->up.y()					       , this->front.y()		               , 0,
		this->right.z()                         , this->up.z()					       , this->front.z()                       , 0,
		-this->right.dot(this->position)		,-this->up.dot(this->position)		   , -this->front.dot(this->position)	   , 1
	};
	//this->view_matrix.
	this->view_matrix = Eigen::Matrix4f(viewMat);
}
