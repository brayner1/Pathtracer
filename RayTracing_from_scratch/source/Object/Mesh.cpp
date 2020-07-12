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
	/*if (!this->is_bounds_hit(incoming_ray))
		return false;*/

	int nIndices = this->indices.size();
	bool bUseNormal = this->vNormals.size() > 0;
	bool bUseTextCoord = this->textCoord.size() > 0;
	bool bUseTangent = this->vTangent.size() > 0;
	bool bUseBitangent = this->vBitangent.size() > 0;

	for (size_t i = 0; i < nIndices; i++)
	{
		//Triangle::TriangleStruct tri;
		int i0 = this->indices[i].x();
		int i1 = this->indices[i].y();
		int i2 = this->indices[i].z();

		Eigen::Vector3f v0 = this->vertices[i0];
		Eigen::Vector3f v1 = this->vertices[i1];
		Eigen::Vector3f v2 = this->vertices[i2];

		Eigen::Vector3f u = v1 - v0;
		Eigen::Vector3f v = v2 - v0;

		Eigen::Vector3f d = incoming_ray.getDirection();
		Eigen::Vector3f o = incoming_ray.getOrigin();

		Eigen::Vector3f tvec = o - v0;
		Eigen::Vector3f pvec = d.cross(v);
		float inv_det = 1.0f / pvec.dot(u);

		const float u_factor = pvec.dot(tvec) * inv_det;
		if (u_factor < 0.0f || u_factor > 1.0f)
			continue;

		Eigen::Vector3f qvec = tvec.cross(u);
		const float v_factor = qvec.dot(d) * inv_det;
		if (v_factor < 0.0f || u_factor + v_factor > 1.0f)
			continue;

		const float dist_factor = qvec.dot(v) * inv_det;
		if (dist_factor < 0)
			continue;
		float hit_distance = (d * dist_factor).norm();
		/// Without a little slack, a reflected ray sometimes hits the same
		/// object again (machine precision..)
		if (hit_distance <= 1e-6f)
			continue;

		
		Eigen::Vector3f normal;
		if (!bUseNormal) {
			normal = v.cross(u).normalized();
		}
		else {
			Eigen::Vector3f normal_du;
			Eigen::Vector3f normal_dv;
			Eigen::Vector3f n0 = this->vNormals[i0];
			Eigen::Vector3f n1 = this->vNormals[i1];
			Eigen::Vector3f n2 = this->vNormals[i2];
			normal_du = n1 - n0;
			normal_dv = n2 - n0;
			normal = (n0 + u_factor * normal_du + v_factor * normal_dv).normalized();
		}

		if (d.dot(normal) > 0)
			if (!incoming_ray.getIsBackfaceHit())
				continue;
			else
			{
				hit_info.hitBackface = true;
				normal = -normal;
			}

		if (!bUseTextCoord)
		{
			hit_info.TextureCoord = Eigen::Vector2f::Zero();
		}
		else
		{
			Eigen::Vector2f uv0 = this->textCoord[i0];
			Eigen::Vector2f uv1 = this->textCoord[i1];
			Eigen::Vector2f uv2 = this->textCoord[i2];
			Eigen::Vector2f uDiff = u_factor * (uv1 - uv0);
			Eigen::Vector2f vDiff = v_factor * (uv2 - uv0);
			hit_info.TextureCoord = uv0 + uDiff + vDiff;
		}

		hit_info.Point = v0 + u * u_factor + v * v_factor;
		hit_info.Normal = normal;
		hit_info.U_factor = u_factor;
		hit_info.V_factor = v_factor;
		hit_info.U_vector = u;
		hit_info.V_vector = v;
		hit_info.Distance = hit_distance;
		hit_info.Material = this->material;

		//std::cout << "hit" << std::endl;

		return true;
		/*tri.P0 = &this->vertices[v1];
		tri.P1 = &this->vertices[v2];
		tri.P2 = &this->vertices[v3];
		tri.Material = this->material;
		if (bUseNormal) {
			tri.N0 = &this->vNormals[this->indices[i].x()];
			tri.N1 = &this->vNormals[this->indices[i].y()];
			tri.N2 = &this->vNormals[this->indices[i].z()];
		}
		else {
			tri.N0 = tri.N1 = tri.N2 = nullptr;
		}
		if (bUseTextCoord) {
			tri.UV0 = &this->textCoord[this->indices[i].x()];
			tri.UV1 = &this->textCoord[this->indices[i].y()];
			tri.UV2 = &this->textCoord[this->indices[i].z()];
		}
		else {
			tri.UV0 = tri.UV1 = tri.UV2 = nullptr;
		}
		if (bUseTangent) {
			tri.T0 = &this->vTangent[this->indices[i].x()];
			tri.T1 = &this->vTangent[this->indices[i].y()];
			tri.T2 = &this->vTangent[this->indices[i].z()];
		}
		else {
			tri.T0 = tri.T1 = tri.T2 = nullptr;
		}
		if (bUseBitangent) {
			tri.B0 = &this->vBitangent[this->indices[i].x()];
			tri.B1 = &this->vBitangent[this->indices[i].y()];
			tri.B2 = &this->vBitangent[this->indices[i].z()];
		}
		else {
			tri.B0 = tri.B1 = tri.B2 = nullptr;
		}
		hit_info.hitBackface = false;
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
		}*/
	}
	return false;
}

Mesh::~Mesh()
{
}
