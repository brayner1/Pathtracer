#include "pch.h"
#include "Object/Mesh.h"
#include "RenderHeaders.h"
using namespace Renderer;

Mesh::Mesh(
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_array,
	std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> index_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_normals_array,
	std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>> texture_coord_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_tangent_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_bitangent_array) :
	vertices(vertex_array), indices(index_array), vNormals(vertex_normals_array), textCoord(texture_coord_array),
	vTangent(vertex_tangent_array), vBitangent(vertex_bitangent_array)
{
	//this->Material = new DiffuseMaterial(Eigen::Vector3f(0.8f, 0.8f, 0.8f), Eigen::Vector3f(0.5f, 0.7f, 0.8f), 0.5f, 128.0f, 0.005f);
}

Mesh::Mesh(
	Eigen::Vector3f * vertex_array, 
	Eigen::Vector3i * index_array,
	int num_vertices, int num_indices,
	Eigen::Vector3f * vertex_normals_array, 
	Eigen::Vector2f * texture_coord_array, 
	Eigen::Vector3f * vertex_tangent_array, 
	Eigen::Vector3f * vertex_bitangent_array)
{
	//this->Material = new DiffuseMaterial(Eigen::Vector3f(0.8f, 0.5f, 0.7f), Eigen::Vector3f(0.5f, 0.7f, 0.8f), 0.5f, 128.0f, 0.0f);
	for (size_t i = 0; i < num_vertices; i++)
	{
		this->vertices.push_back(vertex_array[i]);
		if (vertex_normals_array)
			this->vNormals.push_back(vertex_normals_array[i]);
		if (texture_coord_array)
			this->textCoord.push_back(texture_coord_array[i]);
		if (vertex_tangent_array)
			this->vTangent.push_back(vertex_tangent_array[i]);
		if (vertex_bitangent_array)
			this->vBitangent.push_back(vertex_bitangent_array[i]);
	}
	for (size_t i = 0; i < num_indices; i++)
	{
		this->indices.push_back(index_array[i]);
	}
}

bool Mesh::is_hit_by_ray(Ray& incoming_ray, HitInfo& hit_info) {
	for (size_t i = 0; i < this->indices.size(); i++)
	{
		Triangle::TriangleStruct tri;
		tri.P0 = this->vertices[this->indices[i].x()];
		tri.P1 = this->vertices[this->indices[i].y()];
		tri.P2 = this->vertices[this->indices[i].z()];
		tri.Material = this->material;
		if (this->vNormals.size() > 0) {
			tri.N0 = &this->vNormals[this->indices[i].x()];
			tri.N1 = &this->vNormals[this->indices[i].y()];
			tri.N2 = &this->vNormals[this->indices[i].z()];
		}
		else {
			tri.N0 = tri.N1 = tri.N2 = nullptr;
		}
		if (this->textCoord.size()) {
			tri.UV0 = &this->textCoord[this->indices[i].x()];
			tri.UV1 = &this->textCoord[this->indices[i].y()];
			tri.UV2 = &this->textCoord[this->indices[i].z()];
		}
		else {
			tri.UV0 = tri.UV1 = tri.UV2 = nullptr;
		}
		if (this->vTangent.size()) {
			tri.T0 = &this->vTangent[this->indices[i].x()];
			tri.T1 = &this->vTangent[this->indices[i].y()];
			tri.T2 = &this->vTangent[this->indices[i].z()];
		}
		else {
			tri.T0 = tri.T1 = tri.T2 = nullptr;
		}
		if (this->vBitangent.size()) {
			tri.B0 = &this->vBitangent[this->indices[i].x()];
			tri.B1 = &this->vBitangent[this->indices[i].y()];
			tri.B2 = &this->vBitangent[this->indices[i].z()];
		}
		else {
			tri.B0 = tri.B1 = tri.B2 = nullptr;
		}
		if (Triangle::triangle_hit_by_ray(tri, incoming_ray, hit_info)) {
			hit_info.obj = this;
			hit_info.Material = this->material;

			if (this->textCoord.size())
			{
				Eigen::Vector2f uDiff = hit_info.U_factor *  (*tri.UV1 - *tri.UV0);
				Eigen::Vector2f vDiff = hit_info.V_factor * (*tri.UV2 - *tri.UV0);
				hit_info.TextureCoord = *tri.UV0 + uDiff + vDiff;
			}

			return true;
		}
	}
	return false;
}

Mesh::~Mesh()
{
}
