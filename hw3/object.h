#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

using glm::mat4;
using glm::vec3;

// Define Shape as a standalone enum
enum Shape {tri, sphere};

struct Object {
    Shape type; // Use the standalone Shape enum

    float ambient[3]; 
    float diffuse[3];
    float specular[3];
    float emission[3];
    float shininess;

    int tri[3];
    float sphere[4];
    vec3 normal;
    mat4 transform;
};

#endif 