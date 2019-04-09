#include "pch.h"
#include "Object/Triangle.h"
//#include "RandUtil.h"
using namespace Renderer;


Triangle::Triangle(Eigen::Vector3f p0, Eigen::Vector3f p1, Eigen::Vector3f p2, Eigen::Vector3f color) : P0(p0), P1(p1), P2(p2)
{
	this->u = (P1 - P0);
	this->v = (P2 - P0);
	this->normal = v.cross(u).normalized();
	this->Color = Eigen::Vector3f(0.8f, 1.0f, 0.7f);
}

Triangle::~Triangle()
{
}

bool Triangle::is_hit_by_ray(Ray* incoming_ray, HitInfo& hit_info)
{

	if(incoming_ray->getDirection().dot(this->normal) > 0)
		return false;

	Eigen::Vector3f r = incoming_ray->getDirection();
	Eigen::Vector3f o = incoming_ray->getOrigin();

	/*const float u_factor = (-(o.x() - P0.x())*(r.y()*v.z() - r.z() * v.y()) + (o.y() - P0.y())*(r.x()*v.z() - r.z() * v.x()) - (o.z() - P0.z())*(r.x()*v.y() - r.y() * v.x())) /
		(r.x()*u.y()*v.z() - r.x() * u.z()*v.y() - r.y() * u.x()*v.z() + r.y() * u.z()*v.x() + r.z() * u.x()*v.y() - r.z() * u.y()*v.x());
	const float v_factor = ((o.x() - P0.x())*(r.y()*u.z() - r.z() * u.y()) - (o.y() - P0.y())*(r.x()*u.z() - r.z() * u.x()) + (o.z() - P0.z())*(r.x()*u.y() - r.y() * u.x())) /
		(r.x()*u.y()*v.z() - r.x() * u.z()*v.y() - r.y() * u.x()*v.z() + r.y() * u.z()*v.x() + r.z() * u.x()*v.y() - r.z() * u.y()*v.x());
	const float dist_factor = (-(o.x() - P0.x())*(u.y()*v.z() - u.z() * v.y()) + (o.y() - P0.y())*(u.x()*v.z() - u.z() * v.x()) - (o.z() - P0.z())*(u.x()*v.y() - u.y() * v.x())) /
		(r.x()*u.y()*v.z() - r.x() * u.z()*v.y() - r.y() * u.x()*v.z() + r.y() * u.z()*v.x() + r.z() * u.x()*v.y() - r.z() * u.y()*v.x());

	if (u_factor + v_factor > 1.0f ||
		u_factor < 0 || v_factor < 0 ||
		dist_factor < 0)
		return false;*/

	Eigen::Vector3f tvec = o - P0;
	Eigen::Vector3f pvec = r.cross(v);
	float det = pvec.dot(u);
	float inv_det = 1.0f / det;

	const float u_factor = pvec.dot(tvec)* inv_det;
	if (u_factor < 0.0f || u_factor > 1.0f)
		return false;

	Eigen::Vector3f qvec = tvec.cross(u);
	const float v_factor = qvec.dot(r) * inv_det;
	if (v_factor < 0.0f || u_factor + v_factor > 1.0f)
		return false;

	const float dist_factor = qvec.dot(v) * inv_det;
	if (dist_factor < 0)
		return false;
	float hit_distance = (incoming_ray->getDirection()*dist_factor).norm();
	/// Without a little slack, a reflected ray sometimes hits the same
	/// object again (machine precision..)
	if (hit_info.Distance <= 1e-6f)
		return false;

	// Perfect reflection
	hit_info.Point = P0 + u * u_factor + v * v_factor;
	hit_info.Material = this->Material;
	hit_info.Normal = this->normal;
	hit_info.U_factor = u_factor;
	hit_info.V_factor = v_factor;
	hit_info.Distance = hit_distance;
	return true;

}

bool Triangle::triangle_hit_by_ray(const TriangleStruct triangle, Ray* incoming_ray, HitInfo& hit_info)
{
	Eigen::Vector3f u = *triangle.P1 - *triangle.P0;
	Eigen::Vector3f v = *triangle.P2 - *triangle.P0;

	Eigen::Vector3f r = incoming_ray->getDirection();
	Eigen::Vector3f o = incoming_ray->getOrigin();

	Eigen::Vector3f tvec = o - *triangle.P0;
	Eigen::Vector3f pvec = r.cross(v);
	float det = pvec.dot(u);
	float inv_det = 1.0f / det;

	const float u_factor = pvec.dot(tvec)* inv_det;
	if (u_factor < 0.0f || u_factor > 1.0f)
		return false;

	Eigen::Vector3f qvec = tvec.cross(u);
	const float v_factor = qvec.dot(r) * inv_det;
	if (v_factor < 0.0f || u_factor + v_factor > 1.0f)
		return false;

	const float dist_factor = qvec.dot(v) * inv_det;
	if (dist_factor < 0)
		return false;
	float hit_distance = (incoming_ray->getDirection()*dist_factor).norm();
	/// Without a little slack, a reflected ray sometimes hits the same
	/// object again (machine precision..)
	if (hit_distance <= 1e-6f)
		return false;

	Eigen::Vector3f normal_du;
	Eigen::Vector3f normal_dv;
	Eigen::Vector3f normal;
	if (triangle.N0 == nullptr || triangle.N1 == nullptr || triangle.N2 == nullptr) {
		normal = v.cross(u).normalized();
	}
	else {
		normal_du = *triangle.N1 - *triangle.N0;
		normal_dv = *triangle.N2 - *triangle.N0;
		normal = (*triangle.N0 + u_factor * normal_du + v_factor * normal_dv).normalized();
	}
	if (r.dot(normal) > 0)
		return false;
	
	hit_info.Point = *triangle.P0 + u * u_factor + v * v_factor;
	hit_info.Normal = normal;
	hit_info.U_factor = u_factor;
	hit_info.V_factor = v_factor;
	hit_info.U_vector = u;
	hit_info.V_vector = v;
	hit_info.Distance = hit_distance;
	return true;
}
