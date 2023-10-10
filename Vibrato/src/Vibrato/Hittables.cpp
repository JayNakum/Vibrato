#include "Hittables.h"

#include <iostream>

namespace Vibrato
{
	bool Sphere::intersect(const Ray& ray, HitPayload& payload) const
	{
		glm::vec3 origin = ray.origin - position;

		float a = glm::dot(ray.direction, ray.direction);
		float half_b = glm::dot(origin, ray.direction);
		float c = glm::dot(origin, origin) - (radius * radius);

		float discriminant = half_b * half_b - a * c;

		if (discriminant < 0.0f)
		 	return false;

		// float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
		float closestT = ((-half_b - glm::sqrt(discriminant)) / a);

		if (!(closestT > 0.0f && closestT < ray.hitDistance))
			return false;
		
		ray.hitDistance = closestT;
		payload.hitDistance = ray.hitDistance;
		payload.position = ray.origin + ray.direction * ray.hitDistance;

		glm::vec3 outwardNormal = (payload.position - position) / radius;
		payload.frontFace = glm::dot(ray.direction, outwardNormal) < 0;
		payload.normal = payload.frontFace ? outwardNormal : -outwardNormal;

		return true;
	}


	Triangle::Triangle()
	{}

	Triangle::Triangle(Vertex _v0, Vertex _v1, Vertex _v2)
	{
		v0 = _v0.position;
		v1 = _v1.position;
		v2 = _v2.position;

		e1 = v1 - v0;
		e2 = v2 - v1;

		n = glm::normalize(glm::cross(e1, e2));

		uv[0] = _v0.uv;
		uv[1] = _v1.uv;
		uv[2] = _v2.uv;

		N[0] = _v0.normal;
		N[1] = _v1.normal;
		N[2] = _v2.normal;
	}
	
	bool Triangle::intersect(const Ray& ray, HitPayload& payload) const
	{
		float u, v, t = 0.0f;

		glm::vec3 pvec = glm::cross(ray.direction, e2);
		float det = glm::dot(e1, pvec);
		
		if (det == 0.0f) return false;
		
		float inv_det = 1.0f / det;
		glm::vec3 tvec = ray.origin - v0;
		u = glm::dot(tvec, pvec) * inv_det;
		
		if (u < 0.0f || u > 1.0f) return false;
		
		glm::vec3 qvec = glm::cross(tvec, e1);
		v = glm::dot(ray.direction, qvec) * inv_det;
		
		if (v < 0.0f || u + v > 1.0f) return false;
		
		t = glm::dot(e2, qvec) * inv_det;

		if (!(t > 0.0f && t < ray.hitDistance))
			return false;

		ray.hitDistance = t;
		payload.hitDistance = ray.hitDistance;
		payload.position = ray.origin + ray.direction * ray.hitDistance;

		glm::vec3 outward_normal = this->n;
		payload.normal = this->n;

		glm::vec3 bary = getBaryCentric(payload.position);
		payload.normal = glm::normalize((bary.x * N[0]) + (bary.y * N[1]) + bary.z * N[2]);

		return true;
	}

	glm::vec3 Triangle::getBaryCentric(glm::vec3& p) const
	{
		glm::vec3 v2_ = p - v0;
		float d00 = glm::dot(e1, e1);
		float d01 = glm::dot(e1, e2);
		float d11 = glm::dot(e2, e2);
		float d20 = glm::dot(v2_, e1);
		float d21 = glm::dot(v2_, e2);
		float d = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / d;
		float w = (d00 * d21 - d01 * d20) / d;
		float u = 1 - v - w;
		return glm::vec3(u, v, w);
	}

	//TriangleMesh::TriangleMesh(const char* filepath)
	//{
	//	std::string inputfile = filepath;
	//	unsigned long pos = inputfile.find_last_of("/");
	//	std::string mtlbasepath = inputfile.substr(0, pos + 1);

	//	tinyobj::attrib_t attributes;
	//	std::vector<tinyobj::shape_t> shapes;
	//	std::vector<tinyobj::material_t> materials;
	//	std::string warnings;
	//	std::string errors;

	//	bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings, &errors, inputfile.c_str(), mtlbasepath.c_str());
	//	if (!warnings.empty())
	//	{
	//		std::cout << "Warning: " << warnings << std::endl;
	//	}

	//	if (!errors.empty())
	//	{
	//		std::cerr << "Error: " << errors << std::endl;
	//	}

	//	if (!ret)
	//	{
	//		exit(1);
	//	}

	//	std::vector<Vertex> vertices;

	//	// Loop over shapes
	//	for (size_t s = 0; s < shapes.size(); s++)
	//	{
	//		// Loop over faces(triangles)
	//		size_t index_offset = 0;
	//		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
	//		{
	//			int fv = shapes[s].mesh.num_face_vertices[f];

	//			// Loop over vertices in the face.
	//			for (size_t v = 0; v < fv; v++)
	//			{
	//				// access to vertex
	//				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
	//				tinyobj::real_t vx = attributes.vertices[3 * idx.vertex_index + 0];
	//				tinyobj::real_t vy = attributes.vertices[3 * idx.vertex_index + 1];
	//				tinyobj::real_t vz = attributes.vertices[3 * idx.vertex_index + 2];
	//				tinyobj::real_t nx = attributes.normals[3 * idx.normal_index + 0];
	//				tinyobj::real_t ny = attributes.normals[3 * idx.normal_index + 1];
	//				tinyobj::real_t nz = attributes.normals[3 * idx.normal_index + 2];
	//				tinyobj::real_t tx = attributes.texcoords[2 * idx.texcoord_index + 0];
	//				tinyobj::real_t ty = attributes.texcoords[2 * idx.texcoord_index + 1];

	//				Vertex vert;
	//				vert.position = glm::vec3(vx, vy, vz);
	//				vert.normal = glm::vec3(nx, ny, nz);
	//				vert.uv = glm::vec2(tx, ty);
	//				vertices.push_back(vert);

	//			}
	//			index_offset += fv;


	//		}
	//	}

	//	// Loops vertices
	//	for (int i = 0; i < vertices.size() / 3; ++i)
	//	{
	//		tris.push_back(std::make_shared<Triangle>(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]));
	//	}

	//	std::cout << "> Successfully opened " << inputfile << "! \n\n";

	//	shapes.clear();
	//	materials.clear();
	//}

	//bool TriangleMesh::intersect(const Ray& r, HitPayload& payload) const
	//{
	//	bool hit_tri = false;
	//	long tri_idx;
	//	float t = r.hitDistance;
	//	HitPayload temp;
	//	long triangles_size = tris.size();
	//	for (long i = 0; i < triangles_size; ++i)
	//	{
	//		if (tris[i]->intersect(r, temp))
	//		{
	//			hit_tri = true;
	//			tri_idx = i;
	//			r.hitDistance = temp.hitDistance;
	//			payload = temp;
	//		}
	//	}
	//	return hit_tri;
	//}
}
