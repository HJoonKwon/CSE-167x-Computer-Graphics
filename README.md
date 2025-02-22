# Implementations for the assignments given in CSE-167x Computer Graphics course. 

## Summary of Contents
- hw0, hw1, hw2 - openGL and GLSL (all passed with full score) 

- hw3 - openmp + raytracing from scratch (passed with full score)

| Scene | Image |
| --- | --- |
| Scene 1 | ![hw3-scene1](hw3/scene1-camera4.png) |
| Scene 2 | ![hw3-scene2](hw3/scene2-camera3.png) |
| Scene 3 | ![hw3-scene3](hw3/scene3.png) |
| Scene 4 | ![hw3-scene4](hw3/scene4-diffuse.png) |
| Scene 5 | ![hw3-scene5](hw3/scene5.png) |
| Scene 6 | ![hw3-scene6](hw3/scene6.png) |
| Scene 7 | ![hw3-scene7](hw3/scene7.png) |

## Environment 
- Macbook Air M2
- Libraries were installed through homebrew
- Hw3-scene7 took ~2.5 hrs since the implemented algorithm is not an optimized version. Other scenes took only a matter of seconds/minutes with openmp enabled. 

## What I learned 

- Transformations
  - It was a great chance for me to review transformations such as rotation, translation, scaling, and shearing. And, of course, the homogeneous coordinates and building a 4x4 transformation matrix with combinations of those transformations. I also reviewed Rodrigues' formula and how to derive it given a random rotation axis and angle of rotation. I practically used it to implement the viewer-user interactions, for example, to control the view using keyboard inputs with respect to a given axis. It was fun to build my own model-to-world transformation matrix (so-called modelview in this course) and use it instead of openGL built-in matrix and compare the results, as well as the perspective transformation matrix with the near/far plane, aspect ratio, and the field-of-view. It was also meaningful to separate the openGL conventions and mathematical derivations to understand WHY rather than just accepting them. I used the same matrix for implementing the raytraycing algorithm from scratch without OpenGL. 
- Geometry
  - In computer graphics, every geometry is made up of predefined geometry primitives, which are composed of vertices of which each has attributes like coordinates, color, and normal. The most fundamental and the simplest primitive is a triangle with three vertices for many reasons. For example, it only takes one vertex to make a new triangle next to an existing one, which makes it efficient. It is also easy and takes less computational effort to do geometric calculations such as calculating intersections, normals, interpolations, etc. Many of most geometric objects, thesedays, are made up of millions of triangles. This is why it is exponentially more expensive to create 3D objects, but recent advancements in generative AI is trying to fill in the gap making this process relatively cheaper. 
- Shading and Lights 
  - There are two types of lights: point and directional. The point light has its location (so that we can calculate the distance from the light source to the object) and intensity(colors), while the directional light has only the direction, which is represented as a vector in the world coordinate system. To calculate the effects of these light sources, we first transform the geometry vertices or lights so that they are aligned in the same coordinate system (either that be the world or camera or object coordinate system), and apply the light equations such as Phong illumination using the given parameters like diffuse, specular, and ambient. In this process, the normal of the vertices and the light directions matter. Also, there is an attenuation model that estimates how much of the intensity of the light weakens based on the distance, which only applies to the point light. 
- OpenGL 
  - I learned the graphics pipeline in openGL, from the camera view transformation up to the fragment shader, and the rendering stage. The lecture is focused on view transformations, programming the shaders(especially the fragment shader for lighting), and openGL conventions (how to initialize parameters, buffers, and options like depth test). The framework is internally parallelized, which makes the programming easier because we don't have to care about the parallel computing, but only the geometry and shading. However, it limits developers to customize the rasterization or other intermediate stages since they are compiled and fixed. 
- Raytraycing 
  - It was the most interesting part of the course since I could implement everything from scratch without openGL(though I borrowed some helper functions like parser from the previous homework). I learned how to cast each ray for each pixel to trace it, calculate the intersections with the objects(more strictly, primitives such as triangles or spheres), the normals of the hitting point, tracing the shadow ray again, calculate the lights using Phong illumination, accumulating the light intensities(RGB values) with the recursions. Because it was pixel-by-pixel calculation which makes it very slow if not parallelized, I utilized openmp library to at least parallelize the nested for-loop that loops over every pixel of the image to calculate the corresponding RGB values. There are so many other techniques developed on top of the raw raytracing algorithm through decades of research to make it faster, though I first only implemented the raw one.   