/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  


// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 

/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h" 

using namespace std;
#include "object.h"
#include "variables.h" 
#include "readfile.h"
#include "raytrace.h"

// The function below applies the appropriate transform to a 4-vector
void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
  mat4 &T = transfstack.top(); 
  T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values) 
{
  for (int i = 0; i < numvals; i++) {
    s >> values[i]; 
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n"; 
      return false;
    }
  }
  return true; 
}

void readfile(const char* filename) 
{
  string str, cmd; 
  ifstream in;
  in.open(filename); 
  if (in.is_open()) {

    // I need to implement a matrix stack to store transforms.  
    // This is done using standard STL Templates 
    stack <mat4> transfstack; 
    transfstack.push(mat4(1.0));  // identity

    getline (in, str); 
    while (in) {
      if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
        // Ruled out comment and blank lines 

        stringstream s(str);
        s >> cmd; 
        int i; 
        float values[10]; // Position and color for light, colors for others
        // Up to 10 params for cameras.  
        bool validinput; // Validity of input 

        // read size (w, h)
        if (cmd == "size") {
          validinput = readvals(s,2,values); 
          if (validinput) { 
            globals->width = (int) values[0]; globals->height = (int) values[1]; 
            std::cout << "size" << std::endl;
          } 
        }
        else if (cmd == "output") {
          s >> globals->output_file;
          std::cout << "output: " << globals->output_file << std::endl;
        }
        else if (cmd == "maxdepth") {
          validinput = readvals(s, 1, values);
          if (validinput){
            globals->maxdepth = values[0];
          }
        }
        // read camera (eyeloc, eyecenter, upvector, fovy)
        else if (cmd == "camera") {
          validinput = readvals(s,10,values); // 10 values eye cen up fov
          if (validinput) {

            std::cout << "camera" << std::endl;
            // Set eyeinit upinit center fovy in variables.h 
            for (i = 0; i < 3; i++) globals->eye[i] = values[i];
            for (i = 0; i < 3; i++) globals->center[i] = values[i+3];
            for (i = 0; i < 3; i++) globals->up[i] = values[i+6];

            globals->fovy = values[9];
            globals->modelview = Transform::lookAt(globals->eye, globals->center, globals->up);
          }
        }
        else if (cmd == "attenuation"){
          validinput = readvals(s, 3, values);
          if (validinput){
            for (i = 0; i < 3; i++) globals->attenuation[i] = values[i];
          }
        }

        //light commands 
        else if (cmd == "point") {
          validinput = readvals(s, 6, values);
          if (validinput){
            Light light;
            for (i = 0; i< 3; i++) light.pos[i] = values[i];
            light.pos[3] = 1.0;
            for (i = 0; i< 3; i++) light.color[i] = values[i+3];
            light.attenuation = globals->attenuation;
            globals->lights[globals->numlights] = light;
            globals->numlights++;
            std::cout << "point" << std::endl;
            std::cout << "globals->numlights: " << globals->numlights << std::endl;
          }
        }

        else if (cmd == "directional") {
          validinput = readvals(s, 6, values);
          if (validinput){
            Light light;
            for (i = 0; i< 3; i++) light.pos[i] = values[i];
            light.pos[3] = 0.0;
            for (i = 0; i< 3; i++) light.color[i] = values[i+3];
            light.attenuation = vec3(1.0, 0.0, 0.0);
            globals->lights[globals->numlights] = light;
            globals->numlights++;
            std::cout << "directional" << std::endl;
            std::cout << "globals->numlights: " << globals->numlights << std::endl;
          }
        }

        // Material Commands 
        // Ambient, diffuse, specular, shininess properties for each object.
        // Note that no transforms/stacks are applied to the colors. 

        else if (cmd == "ambient") {
          validinput = readvals(s, 3, values); // colors 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              globals->ambient[i] = values[i]; 
            }
          }
        } else if (cmd == "diffuse") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              globals->diffuse[i] = values[i]; 
            }
          }
        } else if (cmd == "specular") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              globals->specular[i] = values[i]; 
            }
          }
        } else if (cmd == "emission") {
          validinput = readvals(s, 3, values); 
          if (validinput) {
            for (i = 0; i < 3; i++) {
              globals->emission[i] = values[i]; 
            }
          }
        } else if (cmd == "shininess") {
          validinput = readvals(s, 1, values); 
          if (validinput) {
            globals->shininess = values[0]; 
          }
        } else if (cmd == "maxverts") {
          validinput = readvals(s, 1, values);
          if (validinput){
            globals->maxverts = values[0];
          }
        } else if (cmd == "vertex") {
          validinput = readvals(s, 3, values);
          if (validinput){
            globals->vertices.push_back(glm::vec3(values[0], values[1], values[2]));
          }
        } 

        else if (cmd == "sphere" || cmd == "tri") {
            if (cmd == "sphere") validinput = readvals(s, 4, values);
            if (cmd == "tri") validinput = readvals(s, 3, values);
            if (validinput) {
              Object * obj = &(globals->scene[globals->numobjects]); 

              // Set the object's light properties
              for (i = 0; i < 3; i++) {
                (obj->ambient)[i] = globals->ambient[i]; 
                (obj->diffuse)[i] = globals->diffuse[i]; 
                (obj->specular)[i] = globals->specular[i]; 
                (obj->emission)[i] = globals->emission[i];
              }
              obj->shininess = globals->shininess; 

              // Set the object's transform
              obj->transform = transfstack.top(); 
              
              // Set the object's type
              if (cmd == "sphere") {
                obj->type = sphere; 
                for (i = 0; i< 4; i++) obj->sphere[i] = values[i];

              } else if (cmd == "tri") {
                obj->type = tri; 
                for (i = 0; i<3; i++) obj->tri[i] = values[i];
              } 
              globals->numobjects++;
            }
        }

        else if (cmd == "translate") {
          validinput = readvals(s,3,values); 
          if (validinput) {
            mat4 transmatrix = Transform::translate(values[0], values[1], values[2]);
            rightmultiply(transmatrix, transfstack);

          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values); 
          if (validinput) {
            mat4 scalematrix = Transform::scale(values[0], values[1], values[2]);
            rightmultiply(scalematrix, transfstack);

          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values); 
          if (validinput) {
            vec3 axis = vec3(values[0], values[1], values[2]);
            mat3 rotatematrix3 = Transform::rotate(values[3], axis);
            mat4 rotatematrix4 = mat4(rotatematrix3); 
            rightmultiply(rotatematrix4, transfstack);

          }
        }

        // I include the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform") {
          transfstack.push(transfstack.top()); 
        } else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) {
            cerr << "Stack has no elements.  Cannot Pop\n"; 
          } else {
            transfstack.pop(); 
          }
        }

        else {
          cerr << "Unknown Command: " << cmd << " Skipping \n"; 
        }
      }
      getline (in, str); 
    }

  } else {
    cerr << "Unable to Open Input Data File " << filename << "\n"; 
    throw 2; 
  }
  std::cout << "Numobjects: " << globals->numobjects << std::endl;
}
