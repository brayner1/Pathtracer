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

bool Mesh::is_hit_by_ray(const Ray& incoming_ray, HitInfo& hit_info) {
	/*if (!this->is_bounds_hit(incoming_ray))
		return false;*/

	int nIndices = this->indices.size();
	bool bUseNormal = this->vNormals.size() > 0;
	bool bUseTextCoord = this->textCoord.size() > 0;
	bool bUseTangent = this->vTangent.size() > 0;
	bool bUseBitangent = this->vBitangent.size() > 0;
	int triangle = -1;
	float minDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < nIndices; i++)
	{
		const int i0 = this->indices[i].x();
		const int i1 = this->indices[i].y();
		const int i2 = this->indices[i].z();

		/*const Eigen::Vector3f v0 = this->vertices[i0];
		const Eigen::Vector3f v1 = this->vertices[i1];
		const Eigen::Vector3f v2 = this->vertices[i2];*/

		const Eigen::Vector4f v0 = { this->vertices[i0].x(), this->vertices[i0].y(), this->vertices[i0].z(), 1.f };
		const Eigen::Vector4f v1 = { this->vertices[i1].x(), this->vertices[i1].y(), this->vertices[i1].z(), 1.f };
		const Eigen::Vector4f v2 = { this->vertices[i2].x(), this->vertices[i2].y(), this->vertices[i2].z(), 1.f };
		
		const Eigen::Vector4f v1v0 = v1 - v0;
		const Eigen::Vector4f v2v0 = v2 - v0;
		const Eigen::Vector4f n = v1v0.cross3(v2v0);
		const Eigen::Vector4f rayOrig = Eigen::Vector4f{ incoming_ray.getOrigin().x(), incoming_ray.getOrigin().y(), incoming_ray.getOrigin().z(), 1.f };
		const Eigen::Vector4f origv0 = rayOrig - v0;
		const Eigen::Vector4f rayDir = Eigen::Vector4f{ incoming_ray.getDirection().x(), incoming_ray.getDirection().y(), incoming_ray.getDirection().z(), 1.f };;
		const Eigen::Vector4f q = origv0.cross3(rayDir);

		const float d = 1.f / (rayDir.dot(n));
		const float t = d * (-n).dot(origv0);
		const float u = d * (-q).dot(v2v0);
		const float v = d * q.dot(v1v0);

		// Check if points are points and vectors are vectors
		assert((v0.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex1 w() is not 1.f");
		assert((v1.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex2 w() is not 1.f");
		assert((v2.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex3 w() is not 1.f");
		assert((rayOrig.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "rayOrig w() is not 1.f");
		assert(rayDir.w() <= std::numeric_limits<float>::epsilon() && "rayDir w() is not 0.f");

		if (u < 0.0f || v < 0.0f || (u + v) > 1.0f) continue;
		if (t > minDistance || t <= 1e-5f)
			continue;

		{

		//const Eigen::Vector3f u = v1 - v0;
		//const Eigen::Vector3f v = v2 - v0;

		//const Eigen::Vector3f d = incoming_ray.getDirection();
		//const Eigen::Vector3f o = incoming_ray.getOrigin();

		//const Eigen::Vector3f pvec = d.cross(v);
		//const float inv_det = 1.0f / pvec.dot(u);

		//const Eigen::Vector3f tvec = o - v0;
		//const float u_factor = pvec.dot(tvec) * inv_det;
		//const Eigen::Vector3f qvec = tvec.cross(u);
		///*if ((u_factor < 0.0f) | (u_factor > 1.0f))
		//	continue;*/

		//const float v_factor = qvec.dot(d) * inv_det;
		//if ((u_factor < 0.0f) || (u_factor > 1.0f) || (v_factor < 0.0f) || (u_factor + v_factor > 1.0f))
		//	continue;

		//const float dist_factor = qvec.dot(v) * inv_det;
		//if (dist_factor < 0)
		//	continue;
		//const float hit_distance = TriangleIntersect(incoming_ray, v0, v1, v2);//(d * dist_factor).norm();
		/// Without a little slack, a reflected ray sometimes hits the same
		/// object again (machine precision..)
		//if (hit_distance <= 1e-6f)
		//	continue;

		}
		
		Eigen::Vector3f surfNormal = Eigen::Vector3f{n.x(), n.y(), n.z()}.normalized();
		if (incoming_ray.getDirection().dot(surfNormal) > 0.f)
		{
			/*if (!incoming_ray.getIsBackfaceHit())
				continue;*/

			hit_info.hitBackface = true;
			surfNormal = -surfNormal;
		}


		Eigen::Vector3f shadNormal;
		if (bUseNormal)
		{
			const Eigen::Vector3f n0 = this->vNormals[i0];
			const Eigen::Vector3f n1 = this->vNormals[i1];
			const Eigen::Vector3f n2 = this->vNormals[i2];
			//const Eigen::Vector3f normal_du = n1 - n0;
			//const Eigen::Vector3f normal_dv = n2 - n0;
			//normal = (n0 + u * normal_du + v * normal_dv).normalized();
			shadNormal = ((1.f - u - v) * n0 + u * n1 + v * n2).normalized();
			if (incoming_ray.getDirection().dot(surfNormal) > 0)
				shadNormal = -shadNormal;
		}
		else
			shadNormal = surfNormal;
		

		if (!bUseTextCoord)
		{
			hit_info.TextureCoord = Eigen::Vector2f::Zero();
		}
		else
		{
			const Eigen::Vector2f uv0 = this->textCoord[i0];
			const Eigen::Vector2f uv1 = this->textCoord[i1];
			const Eigen::Vector2f uv2 = this->textCoord[i2];
			const Eigen::Vector2f uDiff = u * (uv1 - uv0);
			const Eigen::Vector2f vDiff = v * (uv2 - uv0);
			hit_info.TextureCoord = uv0 + uDiff + vDiff;
		}
		triangle = i;
		minDistance = t;
		hit_info.Point = incoming_ray.getOrigin() + t * incoming_ray.getDirection();
		hit_info.surfNormal = surfNormal;
		hit_info.shadNormal = shadNormal;
		hit_info.U_factor = u;
		hit_info.V_factor = v;
		hit_info.U_vector = Eigen::Vector3f{ v2v0.x(), v2v0.y(), v2v0.z() }.normalized();
		hit_info.V_vector = Eigen::Vector3f{ v1v0.x(), v1v0.y(), v1v0.z() }.normalized(); 
		hit_info.Distance = t;
		hit_info.Material = this->material;
	}

	return triangle == -1? false : true;
}

float Mesh::is_hit_by_ray(const Ray& incoming_ray)
{
	int nIndices = this->indices.size();
	bool hasHit = false;
	float minDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < nIndices; i++)
	{
		const int i0 = this->indices[i].x();
		const int i1 = this->indices[i].y();
		const int i2 = this->indices[i].z();

		const Eigen::Vector4f v0 = { this->vertices[i0].x(), this->vertices[i0].y(), this->vertices[i0].z(), 1.f };
		const Eigen::Vector4f v1 = { this->vertices[i1].x(), this->vertices[i1].y(), this->vertices[i1].z(), 1.f };
		const Eigen::Vector4f v2 = { this->vertices[i2].x(), this->vertices[i2].y(), this->vertices[i2].z(), 1.f };
		
		const Eigen::Vector4f v1v0 = v1 - v0;
		const Eigen::Vector4f v2v0 = v2 - v0;
		const Eigen::Vector4f n = v1v0.cross3(v2v0);
		const Eigen::Vector4f rayOrig = Eigen::Vector4f{ incoming_ray.getOrigin().x(), incoming_ray.getOrigin().y(), incoming_ray.getOrigin().z(), 1.f };
		const Eigen::Vector4f origv0 = rayOrig - v0;
		const Eigen::Vector4f rayDir = Eigen::Vector4f{ incoming_ray.getDirection().x(), incoming_ray.getDirection().y(), incoming_ray.getDirection().z(), 1.f };;
		const Eigen::Vector4f q = origv0.cross3(rayDir);

		const float d = 1.f / (rayDir.dot(n));
		const float t = d * (-n).dot(origv0);
		const float u = d * (-q).dot(v2v0);
		const float v = d * q.dot(v1v0);

		// Check if points are points and vectors are vectors
		assert((v0.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex1 w() is not 1.f");
		assert((v1.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex2 w() is not 1.f");
		assert((v2.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex3 w() is not 1.f");
		assert((rayOrig.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "rayOrig w() is not 1.f");
		assert(rayDir.w() <= std::numeric_limits<float>::epsilon() && "rayDir w() is not 0.f");

		if (u < 0.0f || v < 0.0f || (u + v) > 1.0f) continue;
		if (t > minDistance || t <= 1e-5f)
			continue;

		hasHit = true;
		minDistance = t;
	}

	return hasHit? minDistance : -1.f;
}

float TriangleIntersect(const Ray& ray, const Eigen::Vector4f& v0, const Eigen::Vector4f& v1, const Eigen::Vector4f& v2)
{
	const Eigen::Vector4f rayOrig = Eigen::Vector4f{ ray.getOrigin().x(),ray.getOrigin().y(), ray.getOrigin().z(), 1.f };
	const Eigen::Vector4f rayDir = Eigen::Vector4f{ ray.getDirection().x(),ray.getDirection().y(), ray.getDirection().z(), 1.f };;

	// Check if points are points and vectors are vectors
	assert((v0.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex1 w() is not 1.f");
	assert((v1.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex2 w() is not 1.f");
	assert((v2.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "vertex3 w() is not 1.f");
	assert((rayOrig.w() - 1.f) <= std::numeric_limits<float>::epsilon() && "rayOrig w() is not 1.f");
	assert(rayDir.w() <= std::numeric_limits<float>::epsilon() && "rayDir w() is not 0.f");

	const Eigen::Vector4f v1v0 = v1 - v0;
	const Eigen::Vector4f v2v0 = v2 - v0;
	const Eigen::Vector4f origv0 = rayOrig - v0;
	const Eigen::Vector4f normal = v1v0.cross3(v2v0);
	const Eigen::Vector4f q = origv0.cross3(rayDir);

	const float d = 1.f / (rayDir.dot(normal));
	const float u = d * (-q).dot(v2v0);
	const float v = d * q.dot(v1v0);
	const float t = d * (-normal).dot(origv0);

	if (u < 0.0f || u > 1.0f || v < 0.0f || (u + v) > 1.0f) return -1.f;

	return t;
}