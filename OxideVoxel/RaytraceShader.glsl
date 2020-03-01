#version 460 core
layout(local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, rgba32f) writeonly uniform image2D frame;

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;

uniform int octree;



vec4 trace(vec3 origin, vec3 dir){
    /*if(dir.x == 0 || dir.y == 0 || dir.z == 0){
        return vec4(0.0, 0.0, 0.0, 1.0);
    }

    float xv = 1 / (dir.x);
    float yv = 1/ (dir.y);
    float zv = 1 / (dir.z);

    float xc = origin.x * xv;
    float yc = origin.y * yv;
    float zc = origin.z * zv;

        
    float t_y_0 = -yc;
    float t_min = max(x-;
    float t_max = max(max(xv - xc, yv - yc), zv - zc);
    
   // t_min = fmaxf(t_min, 0.0f);
   // t_max = fminf(t_max, 1.0f);
    if(t_min <= t_max){
      return vec4(0.0,1.0,0.0,1.0);
    }
    //if(t_y_0 > 0){
    //    return vec4(0.0,1.0,0.0,1.0);
    //}*/

    vec4 red = vec4(1.0,0.0,0.0,1.0);
    vec4 green = vec4(0.0,1.0,0.0,1.0);

    float tmin = (0 - origin.x) / dir.x; 
    float tmax = (1 - origin.x) / dir.x; 
 
    if (tmin > tmax){
        float h = tmax;
        tmax = tmin;
        tmin = h;
    } 
     
    float tymin = (0 - origin.y) / dir.y; 
    float tymax = (1 - origin.y) / dir.y; 
 
    if (tymin > tymax){
        float h = tymax;
        tymax = tymin;
        tymin = h;
    } 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return red; 
 
    if (tymin > tmin) 
        tmin = tymin; 
 
    if (tymax < tmax) 
        tmax = tymax; 
 
    float tzmin = (0 - origin.z) / dir.z; 
    float tzmax = (1 - origin.z) / dir.z; 
 
    if (tzmin > tzmax){
        float h = tzmax;
        tzmax = tzmin;
        tzmin = h;
    } 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return red; 
 
    if (tzmin > tmin) 
        tmin = tzmin; 
 
    if (tzmax < tmax) 
        tmax = tzmax; 

    return green;
}

void main()
{
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(frame);

    vec2 pos = vec2(pix) / vec2(size.x - 1, size.y - 1);
    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
    vec4 col = trace(eye, dir);
    imageStore(frame, pix, col);
}