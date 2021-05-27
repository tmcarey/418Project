#version 460 core
layout(local_size_x = 1, local_size_y = 1) in;
layout (binding = 0, rgba32f) writeonly uniform image2D frame;

uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray01;
uniform vec3 ray10;
uniform vec3 ray11;




vec4 trace(vec3 origin, vec3 dir, vec4 success, vec4 fail){
    int octree = 0xAf;
    int correction = 0;
    if(dir.x < 0){
        correction = correction | 1;
        dir.x = -dir.x;
        origin.x = -origin.x;
    }

    if(dir.y < 0){
        correction = correction | (1 << 1);
        dir.y = -dir.y;
        origin.y = -origin.y;
    }

    if(dir.z < 0){
        correction = correction | (1 << 2);
        dir.z = -dir.z;
        origin.z = -origin.z;
    }

    float xcof = - origin.x / dir.x;
    float xmin = (-1 / dir.x) + xcof;
    float xmax = (1 / dir.x) + xcof;
    //These autofail the trace if we miss the cube
    if(xmax < 0){
        return fail;
    }

    float ycof = - origin.y / dir.y;
    float ymin = (-1 / dir.y) + ycof;
    float ymax = (1 / dir.y) + ycof;
    if(ymax < 0){
        return fail;
    }

    float zcof = - origin.z / dir.z;
    float zmin = (-1 / dir.z) + zcof;
    float zmax = (1 / dir.z) + zcof;
    if(zmax < 0){
        return fail;
    }

    //find extreme plane hits
    float maxmin = xmin;
    if(ymin >= maxmin){
        maxmin = ymin;
    }
    if(zmin >= maxmin){
        maxmin = zmin;
    }

    float minmax = xmax;
    if(ymax <= minmax){
      minmax = ymax;
    }
    if(zmax <= minmax){
      minmax = zmax;
    }
    
    //Check for cube hit
    if(minmax >= maxmin){
        int index = 0;
        //determine octant
        if(maxmin >= xcof){
          index = index | 1;
        }
        if(maxmin >= ycof){
          index = index | (1 << 1);
        }
        if(maxmin >= zcof){
          index = index | (1 << 2);
        }
        vec4 green = vec4(0.0,1.0,0.0,1.0);
        
        while((octree >> (index ^ correction)) % 2 == 0){
            green = vec4(0.0,1.0,0.0,1.0);
            bool found = false;
            float currmin = minmax;
            int currindex = 0;

            if(xcof > 0 && xcof > maxmin && xcof <= minmax && xcof <= currmin){
                found = true;
                currindex = index ^ (1);
                currmin = xcof;
            }

            if(ycof > 0 && ycof > maxmin && ycof <= minmax && ycof <= currmin){
                found = true;
                currindex = index ^ (1 << 1);
                currmin = ycof;
            }

            if(zcof > 0 && zcof > maxmin && zcof <= minmax && zcof <= currmin){
                found = true;
                currindex = index ^ (1 << 2);
                currmin = zcof;
            }

            if(!found){
                return fail;
            }

            index = currindex;
            maxmin = currmin;
        }
        return green;

    }

    return fail;

}



void main()
{
    ivec2 pix = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(frame);

    vec2 pos = vec2(pix) / vec2(size.x - 1, size.y - 1);
    vec3 dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);
    vec4 col = trace(eye, dir, vec4(1.0,1.0,1.0,1.0), vec4(1.0,0.0,0.0,1.0));
    imageStore(frame, pix, col);
}