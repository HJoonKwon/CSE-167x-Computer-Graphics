#include "raytrace.h"


float eps = 1e-4f;

Ray transform_ray(Ray* ray, mat4 transform){
    Ray new_ray;
    vec4 origin = transform * vec4(ray->origin, 1);
    new_ray.origin = vec3(origin / origin.w);
    new_ray.dir = normalize(vec3(transform * vec4(ray->dir, 0)));
    return new_ray;
}

vec3 transform_vec(vec3 vec, mat4 transform){
    vec4 vec_t = transform * vec4(vec, 1);
    return vec3(vec_t / vec_t.w);
}

vec3 transform_normal(vec3 normal, mat4 transform){
    vec4 normal4 = vec4(normal, 0);
    vec4 normal_transformed = transpose(inverse(transform)) * normal4;
    return normalize(vec3(normal_transformed));
}

void ray_thru_pixel(int row, int col, Ray* ray){
    std::vector<vec3> axes = Transform::createAxes(globals->eye, globals->center, globals->up);
    vec3 uhat = axes[0];
    vec3 vhat = axes[1];
    vec3 what = axes[2];
    double cx = col + 0.5; // center of each pixel
    double cy = row + 0.5;
    double fovy_half_rad = globals->fovy / 2 * pi / 180.0;
    double tan_fovy = tan(fovy_half_rad);
    int w = globals->width, h = globals->height;
    double tan_fovx = tan_fovy * w / h;
    float alpha = tan_fovx * (cx - (double)(w/2)) / (w/2); 
    float beta = tan_fovy * ((double)(h/2) - cy) / (h/2);
    vec3 dir = normalize(alpha * uhat + beta * vhat - what);
    ray->origin = globals->eye;
    ray->dir = dir;
}

bool InsideTriangle(vec3 P, vec3 A, vec3 B, vec3 C){
    //https://en.wikipedia.org/wiki/Barycentric_coordinate_system

    vec3 normal = normalize(cross(B-A, C-A));
    bool inside1 = dot(cross(B-A, P-A), normal) >= 0;
    bool inside2 = dot(cross(C-B, P-B), normal) >= 0;
    bool inside3 = dot(cross(A-C, P-C), normal) >= 0;

    return (inside1 && inside2 && inside3);
    }

float Intersect(Ray* ray, Object* obj, IntersectionInfo* hit){
    float t = -1; 
    float min_t = eps;
 
    if (obj->type == tri){
        // counter clockwise
        vec3 A = globals->vertices[obj->tri[0]];
        vec3 B = globals->vertices[obj->tri[1]];
        vec3 C = globals->vertices[obj->tri[2]];

        A = transform_vec(A, obj->transform); 
        B = transform_vec(B, obj->transform);
        C = transform_vec(C, obj->transform);

        // normal inworld space
        vec3 normal = normalize(cross(B-A, C-A)); // AB x AC

        float denom = dot(ray->dir, normal);
        t = (dot(A, normal) - dot(ray->origin, normal)) / denom;

        vec3 P = ray->origin + t * ray->dir; // intersection with the plane 
      
        if (!InsideTriangle(P, A, B, C)) t = -1;
        if (t>min_t && t< hit->distance){
            hit->distance = t;
            hit->P = P;
            hit->obj = obj;
            hit->normal = normal;
        }
    }
    else if (obj->type == sphere){
        vec3 center(obj->sphere[0], obj->sphere[1], obj->sphere[2]);
        float radius = obj->sphere[3];

        mat4 obj2world = obj->transform;
        mat4 world2obj = inverse(obj2world);

        // camera to object space
        Ray ray_obj = transform_ray(ray, world2obj); 

        // object space
        double a = dot(ray_obj.dir, ray_obj.dir);
        double b = 2 * dot(ray_obj.dir, ray_obj.origin - center);
        double c = dot(ray_obj.origin - center, ray_obj.origin - center) - pow(radius, 2);
        double discriminant = pow(b, 2) - 4 * a * c;
        if (discriminant < 0){
            t = -1;
        }
        else if (discriminant >=0){
            double t1 = (-b - pow(discriminant, 0.5)) / 2 / a;
            double t2 = (-b + pow(discriminant, 0.5)) / 2 / a;
            t = (float) std::min(t1, t2);
        }
        if (t>min_t){
            // intersection point in object space 
            vec3 P = ray_obj.origin + t * ray_obj.dir;

            // intersection point in world space 
            vec3 normal_obj = normalize(P - center);
            vec3 normal_world = transform_normal(vec3(normal_obj), obj2world);
            vec3 P_world = transform_vec(P, obj2world);
            
            // if (dot(ray->dir, normal_world) > 0) normal_world = -normal_world;

            t = length(P_world - ray->origin);
            if (t>min_t && t < hit->distance){
            hit->normal = normal_world;
            hit->distance = t;
            hit->P = P_world;
            hit->obj = obj;
            }
        }
    }
    return t;
}

void intersection(Ray* ray, IntersectionInfo* hit){
    hit->obj = nullptr;
    hit->distance = std::numeric_limits<float>::infinity();
    for (int i=0; i < globals->numobjects; i++){
        Object* obj = &(globals->scene[i]);
        float t = Intersect(ray, obj, hit);
    }
}

float attenuation(float d, vec3 coeffs){
    if (d == std::numeric_limits<float>::infinity()) return 1.0f;
    float denom = coeffs[0] + coeffs[1] * d + coeffs[2] * pow(d, 2);
    if (denom < eps) denom = eps;
    return 1.0f / denom;
}

vec3 find_color(Ray* ray, IntersectionInfo* hit, int depth=globals->maxdepth){
    vec3 rgb(0, 0, 0); 
    Object* obj = hit->obj;
    if (obj == nullptr || hit->distance == std::numeric_limits<float>::infinity()) return rgb;
    vec3 diffuse(obj->diffuse[0], obj->diffuse[1], obj->diffuse[2]); // lambertian reflectance
    vec3 specular(obj->specular[0], obj->specular[1], obj->specular[2]); // specular reflectance

    vec3 ambient(obj->ambient[0], obj->ambient[1], obj->ambient[2]);
    vec3 emission(obj->emission[0], obj->emission[1], obj->emission[2]);
    rgb = rgb + emission + ambient;
    vec3 normal = hit->normal;
    assert(!std::isnan(normal.x) && !std::isnan(normal.y) && !std::isnan(normal.z));

    for (int i=0; i < globals->numlights; i++){
        vec4 lightpos = globals->lights[i].pos;
        vec3 L;
        float distance_to_light;
        lightpos[3] == 0 ? L = normalize(vec3(lightpos)) : L = normalize(vec3(lightpos) - hit->P);
        lightpos[3] == 0 ? distance_to_light = std::numeric_limits<float>::infinity() : distance_to_light = length(vec3(lightpos) - hit->P);
        float atten = attenuation(distance_to_light, globals->lights[i].attenuation);
        vec3 P = hit->P + eps * L; // Avoid self-intersection

        Ray shadow_ray;
        shadow_ray.origin = P;
        shadow_ray.dir = L;
        IntersectionInfo shadow_hit;
        intersection(&shadow_ray, &shadow_hit);
        if (shadow_hit.distance > 0 && shadow_hit.distance < distance_to_light){
            // std::cout << "distance to light: " << distance_to_light << " shadow hit distance: " << shadow_hit.distance << std::endl;
            continue;
        }
        else{
            vec3 V = -ray->dir; // view direction
            vec3 H = normalize(L + V); // half vector
            vec3 lightcolor = globals->lights[i].color;
            float NdotL = std::max(dot(normal, L), 0.0f);
            float NdotH = std::max(dot(normal, H), 0.0f);
           
            rgb = rgb + atten * lightcolor * (diffuse * NdotL + specular * pow(NdotH, obj->shininess));
        }
    }

    // recursive reflection until maxdepth
    if (depth > 0){
        vec3 R = normalize(ray->dir - 2 * dot(ray->dir, normal) * normal);
        Ray reflect_ray;
        reflect_ray.origin = hit->P + eps * R;
        reflect_ray.dir = R;
        IntersectionInfo reflect_hit;
        intersection(&reflect_ray, &reflect_hit);
        vec3 reflect_rgb = find_color(&reflect_ray, &reflect_hit, depth-1);
        rgb = rgb + specular * reflect_rgb;
    }
    return rgb;
}

std::vector<BYTE> raytrace(){

    int width = globals->width, height = globals->height;
    std::vector<BYTE> image(width * height * 3);

    int total_pixels = width * height;
    int processed_pixels = 0;
    omp_set_num_threads(6);
    #pragma omp parallel for schedule(dynamic) collapse(1)
    for (int row=0; row < height ; row++){
        for (int col=0; col < width ; col++){

            Ray ray;
            IntersectionInfo hit;
            ray_thru_pixel(row, col, &ray);
            intersection(&ray, &hit);
            vec3 rgb = find_color(&ray, &hit);
           
            int index = (row * width + col) * 3;
            image[index] = static_cast<BYTE>(rgb[2] * 255.0f); // Scale to 0-255
            image[index + 1] = static_cast<BYTE>(rgb[1] * 255.0f);
            image[index + 2] = static_cast<BYTE>(rgb[0] * 255.0f);

            #pragma omp atomic
            processed_pixels++;

            if (processed_pixels % 1000 == 0){ 
                #pragma omp critical
                {
                    float progress = (processed_pixels / (float)total_pixels) * 100.0f;
                    std::cout << "\rProgress: " << progress << "%";
                    std::cout.flush();
                }
            }
        }
    }

    return image;
}

