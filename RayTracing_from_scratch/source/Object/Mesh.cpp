#include "pch.h"
#include "Object/Mesh.h"
#include <vector>
#include <Eigen/src/Geometry/AlignedBox.h>
#include "RenderHeaders.h"

using namespace Renderer;

Eigen::AlignedBox3f TriangleBounds(const Eigen::Vector3f& v0, const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);

Mesh::Mesh(
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_array,
	std::vector<Eigen::Vector3i, Eigen::aligned_allocator<Eigen::Vector3i>> index_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_normals_array,
	std::vector<Eigen::Vector2f, Eigen::aligned_allocator<Eigen::Vector2f>> texture_coord_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_tangent_array,
	std::vector<Eigen::Vector3f, Eigen::aligned_allocator<Eigen::Vector3f>> vertex_bitangent_array) :
	vertices(vertex_array), triangles(index_array), vNormals(vertex_normals_array), vUV(texture_coord_array),
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
			this->vUV.push_back(texture_coord_array[i]);
		if (vertex_tangent_array)
			this->vTangent.push_back(vertex_tangent_array[i]);
		if (vertex_bitangent_array)
			this->vBitangent.push_back(vertex_bitangent_array[i]);
	}
	for (size_t i = 0; i < num_indices; i++)
	{
		this->triangles.push_back(index_array[i]);
	}
}

float Mesh::PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index, HitInfo& hit_info) const
{
	if (primitive_index >= triangles.size())
		return -1.f;

	bool bUseNormal = this->vNormals.size() > 0;
	bool bUseTextCoord = this->vUV.size() > 0;
	bool bUseTangent = this->vTangent.size() > 0;
	bool bUseBitangent = this->vBitangent.size() > 0;

	const int i0 = this->triangles[primitive_index].x();
	const int i1 = this->triangles[primitive_index].y();
	const int i2 = this->triangles[primitive_index].z();

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

	if (u < 0.0f || v < 0.0f || (u + v) > 1.0f) return -1.f;
	if (t <= 1e-5f) return -1.f;

	Eigen::Vector3f surfNormal, shadeNormal;

	if (bUseNormal)
	{
		const Eigen::Vector3f& n0 = this->vNormals[i0];
		const Eigen::Vector3f& n1 = this->vNormals[i1];
		const Eigen::Vector3f& n2 = this->vNormals[i2];

		//TODO:shadeNormal should be defined by normal texture and tangents and bitangents
		shadeNormal = ((1.f - u - v) * n0 + u * n1 + v * n2).normalized();
		surfNormal = shadeNormal;
	}
	else
	{
		surfNormal = Eigen::Vector3f{n.x(), n.y(), n.z()}.normalized();
		shadeNormal = surfNormal;
	}

	if (!bUseTextCoord)
	{
		hit_info.UvCoord = Eigen::Vector2f::Zero();
	}
	else
	{
		const Eigen::Vector2f uv0 = this->vUV[i0];
		const Eigen::Vector2f uv1 = this->vUV[i1];
		const Eigen::Vector2f uv2 = this->vUV[i2];
		const Eigen::Vector2f uDiff = u * (uv1 - uv0);
		const Eigen::Vector2f vDiff = v * (uv2 - uv0);
		hit_info.UvCoord = uv0 + uDiff + vDiff;
	}

	hit_info.Point = incoming_ray.getOrigin() + t * incoming_ray.getDirection();
	hit_info.surfNormal = surfNormal;
	hit_info.shadNormal = shadeNormal;
	hit_info.U_factor = u;
	hit_info.V_factor = v;
	hit_info.U_vector = Eigen::Vector3f{ v2v0.x(), v2v0.y(), v2v0.z() }.normalized();
	hit_info.V_vector = Eigen::Vector3f{ v1v0.x(), v1v0.y(), v1v0.z() }.normalized(); 
	hit_info.Distance = t;
	hit_info.Material = this->material;

	return t;
}

float Mesh::PrimitiveHitByRay(const Ray& incoming_ray, int primitive_index) const
{
	if (primitive_index >= triangles.size())
		return -1.f;

	const int& i0 = this->triangles[primitive_index].x();
	const int& i1 = this->triangles[primitive_index].y();
	const int& i2 = this->triangles[primitive_index].z();

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

	if (u < 0.0f || v < 0.0f || (u + v) > 1.0f) return -1.f;
	if (t <= 1e-5f) return -1.f;

	return t;
}

std::vector<Eigen::AlignedBox3f> Mesh::GetPrimitivesBounds() const
{
	std::vector<Eigen::AlignedBox3f> primBounds;
	primBounds.reserve(triangles.size());
	for (auto triangle : triangles)
	{
		const Eigen::Vector3f& v0 = vertices[triangle.x()];
		const Eigen::Vector3f& v1 = vertices[triangle.y()];
		const Eigen::Vector3f& v2 = vertices[triangle.z()];

		primBounds.push_back(TriangleBounds(v0, v1, v2));
	}
	return std::move(primBounds);
}

uint32_t Mesh::GetPrimitiveCount() const
{
	return triangles.size();
}

HitInfo Mesh::SamplePrimitivePoint(uint32_t primitiveIndex) const
{
	float u0 = uniform_random_01();
	float u1 = uniform_random_01();

	float squ0 = std::sqrt(u0);
	Eigen::Vector2f barycentric {1 - squ0, u1 * squ0};

	const Eigen::Vector3i& triangle = triangles[primitiveIndex];

	const Eigen::Vector3f& v0 = vertices[triangle.x()];
	const Eigen::Vector3f& v1 = vertices[triangle.y()];
	const Eigen::Vector3f& v2 = vertices[triangle.z()];

	HitInfo pointInfo;
	pointInfo.Point = (1.f - barycentric.x() - barycentric.y()) * v0 + barycentric.x() * v1 + barycentric.y() * v2;

	if (this->vUV.size() > 0)
	{
		const Eigen::Vector2f& uv0 = this->vUV[triangle.x()];
		const Eigen::Vector2f& uv1 = this->vUV[triangle.y()];
		const Eigen::Vector2f& uv2 = this->vUV[triangle.z()];
		pointInfo.UvCoord = (1.f - barycentric.x() - barycentric.y()) * uv0 + barycentric.x() * uv1 + barycentric.y() * uv2;
	}
		

	if (this->vNormals.size() > 0)
	{
		const Eigen::Vector3f& n0 = this->vNormals[triangle.x()];
		const Eigen::Vector3f& n1 = this->vNormals[triangle.y()];
		const Eigen::Vector3f& n2 = this->vNormals[triangle.z()];
		pointInfo.surfNormal = (1.f - barycentric.x() - barycentric.y()) * v0 + barycentric.x() * v1 + barycentric.y() * v2;
	}
	else
	{
		pointInfo.surfNormal = ((v1 - v0).cross(v2 - v0)).normalized();
	}

	return pointInfo;
}

Eigen::AlignedBox3f TriangleBounds(const Eigen::Vector3f& v0, const Eigen::Vector3f& v1, const Eigen::Vector3f& v2)
{
	Eigen::AlignedBox3f bounds{};
	bounds.min().x() = std::min( { v0.x(), v1.x(), v2.x() } );
	bounds.min().y() = std::min( { v0.y(), v1.y(), v2.y() } );
	bounds.min().z() = std::min( { v0.z(), v1.z(), v2.z() } );
								   						    
	bounds.max().x() = std::max( { v0.x(), v1.x(), v2.x() } );
	bounds.max().y() = std::max( { v0.y(), v1.y(), v2.y() } );
	bounds.max().z() = std::max( { v0.z(), v1.z(), v2.z() } );

	return bounds;
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
