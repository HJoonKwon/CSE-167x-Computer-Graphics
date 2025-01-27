#include <FreeImage.h>
#include <algorithm>
#include <limits>
#include <omp.h>
#include "Transform.h" 
#include "variables.h"
#include "object.h"

#ifndef RAYTRACE_H
#define RAYTRACE_H

struct IntersectionInfo{
    float distance;
    vec3 P;
    Object* obj; 
    vec3 normal;
};

struct Ray{
    vec3 origin;
    vec3 dir;
};

void ray_thru_pixel(int x, int y, Ray* ray);
bool InsideTriangle(vec3 P, vec3 A, vec3 B, vec3 C);
float Intersect(Ray* ray, Object* obj);
void intersection(Ray* ray, IntersectionInfo* hit);
vec3 find_color(IntersectionInfo* hit);
std::vector<BYTE> raytrace();
Ray transform_ray(Ray* ray, mat4 transform);
vec3 transform_vec(vec3 vec, mat4 transform);
vec3 transform_normal(vec3 normal, mat4 transform);

#endif // RAYTRACE_H