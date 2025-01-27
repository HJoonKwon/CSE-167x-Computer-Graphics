#ifndef VARIABLES_H
#define VARIABLES_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include "object.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Light{
    vec4 pos;
    vec3 color;
    vec3 attenuation;
};

class GlobalVariables {
public:
    vec3 eye ;
    vec3 up ;
    vec3 center ;

    int width, height;
    float fovy ;

    float sx, sy;
    float tx, ty;

    mat4 projection;
    mat4 modelview;

    vec3 attenuation = vec3(1.0, 0.0, 0.0);

    float ambient[3] = {0.2, 0.2, 0.2};
    float diffuse[3];
    float specular[3];
    float emission[3];
    float shininess;

    int numlights = 0;
    Light lights[10];

    int maxverts;
    std::vector<vec3> vertices;
    int numobjects = 0;
    int maxdepth = 5;

    Object scene[100000];
    std::string output_file = "output.png";
};

// Declare a global instance of GlobalVariables
extern std::shared_ptr<GlobalVariables> globals;

#endif // VARIABLES_H

