// Transform.cpp: implementation of the Transform class.


#include "Transform.h"

//Please implement the following functions:

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {
	// YOUR CODE FOR HW1 HERE

	float radians = degrees * pi / 180.0;
	float costh = cos(radians);
	float sinth = sin(radians);
	float x = axis[0], y = axis[1], z = axis[2];
	mat3 rotation_matrix(
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
	
	// You will change this return call
	return rotation_matrix;
}

// Transforms the camera left around the "crystal ball" interface
void Transform::left(float degrees, vec3& eye, vec3& up) {
	// YOUR CODE FOR HW1 HERE
	// rotate eye vector around up vector 
	// it would look like the teapot rotates to the left (clockwise from the top)
	// up vector would remain the same because it's rotating around the up vector
	vec3 axis = glm::normalize(up);
	mat3 rotation_matrix = rotate(degrees, axis);
	eye = rotation_matrix * eye;
	// Q = (M^-1)^T, since M =R here, Q = R.
	up = rotation_matrix * up;
}

// Transforms the camera up around the "crystal ball" interface
void Transform::up(float degrees, vec3& eye, vec3& up) {
	// YOUR CODE FOR HW1 HERE 
	vec3 new_axis = glm::cross(eye, up);
	new_axis = glm::normalize(new_axis); // unit vector
	mat3 rotation_matrix = rotate(degrees, new_axis);
	eye = rotation_matrix * eye;
	up = rotation_matrix * up; 
}

// Your implementation of the glm::lookAt matrix
mat4 Transform::lookAt(vec3 eye, vec3 up) {
	// YOUR CODE FOR HW1 HERE

	vec3 w = glm::normalize(eye);
	vec3 u = glm::cross(up, w);
	u = glm::normalize(u);
	vec3 v = glm::cross(w, u);

	// column-major
	// translation = -eye
	mat4 transformation(
		u[0], v[0], w[0], 0,
		u[1], v[1], w[1], 0,
		u[2], v[2], w[2], 0,
		glm::dot(u, -eye), glm::dot(v, -eye), glm::dot(w, -eye), 1
	);

	// You will change this return call
	return transformation;
}

Transform::Transform()
{

}

Transform::~Transform()
{

}
