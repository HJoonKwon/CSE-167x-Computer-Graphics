// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
  mat3 ret;
  float radians = degrees * pi / 180.0;
	float costh = cos(radians);
	float sinth = sin(radians);
	float x = axis[0], y = axis[1], z = axis[2];
	ret = glm::mat3(
		costh + (1 - costh) * pow(x,2),
		(1 - costh) * x * y + sinth * z,
		(1 - costh) * x * z - sinth * y,
		(1 - costh) * x * y - sinth * z,
		costh + (1 - costh) * pow(y,2),
		(1 - costh) * y * z + sinth * x,
		(1 - costh) * x * z + sinth * y,
		(1 - costh) * y * z - sinth * x,
		costh + (1 - costh) * pow(z,2)
  ); 
  return ret;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
  // YOUR CODE FOR HW2 HERE
  // Likely the same as in HW 1.  
  vec3 axis = glm::normalize(up);
	mat3 rotation_matrix = rotate(degrees, axis);
	eye = rotation_matrix * eye;
	// Q = (M^-1)^T, since M =R here, Q = R.
	up = rotation_matrix * up;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
  vec3 new_axis = glm::cross(eye, up);
	new_axis = glm::normalize(new_axis); // unit vector
	mat3 rotation_matrix = rotate(degrees, new_axis);
	eye = rotation_matrix * eye;
	up = rotation_matrix * up; 
}

std::vector<vec3> Transform::createAxes(vec3 eye, vec3 center, vec3 up){
  vec3 w = glm::normalize(eye-center);
	vec3 u = glm::cross(up, w);
	u = glm::normalize(u);
	vec3 v = glm::cross(w, u);
  v = glm::normalize(v);
  std::vector<vec3> ret;
  ret.push_back(u);
  ret.push_back(v);
  ret.push_back(w);
  return ret;
}

vec4 Transform::transformVec(const mat4 transmat, const vec4 input){
  vec4 output = transmat * input;
  return output;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
  mat4 ret;
  vec3 eyecenter = eye - center;
  vec3 w = glm::normalize(eyecenter);
	vec3 u = glm::cross(up, w);
	u = glm::normalize(u);
	vec3 v = glm::cross(w, u);

	// column-major
	ret = glm::mat4(
		u[0], v[0], w[0], 0,
		u[1], v[1], w[1], 0,
		u[2], v[2], w[2], 0,
		glm::dot(u, -eye), glm::dot(v, -eye), glm::dot(w, -eye), 1
  );
  return ret;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
  mat4 ret;
  float theta = fovy / 2 * pi / 180.0f; // degrees to radians
  float d = 1 / glm::tan(theta);
  float A = - (zFar + zNear) / (zFar - zNear);
  float B = - 2 * zFar * zNear / (zFar - zNear);
  ret = glm::mat4(
    d/aspect, 0, 0, 0,
    0, d, 0, 0,
    0, 0, A, -1,
    0, 0, B, 0
  );
  return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
  mat4 ret;
  ret = glm::mat4(
    sx, 0, 0, 0,
    0, sy, 0, 0,
    0, 0, sz, 0,
    0, 0, 0, 1
  );
  return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  mat4 ret;
  ret = glm::mat4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    tx, ty, tz, 1
  );
  return ret;
}

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
