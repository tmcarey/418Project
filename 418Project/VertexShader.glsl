#version 460 core
layout (location = 0) in vec2 aPos;
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aNorm;
//layout (location = 2) in vec3 aOffset;
//layout (location = 3) in uint col;


//out vec3 fNorm;
//out uint col;

out vec2 rPos;

//uniform mat4 transform;

void main()
{
    //gl_Position = transform * vec4(aPos, 1.0);
    //fNorm = aNorm;
    //col = 0;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    rPos = (aPos * 0.5f) + 0.5f;
}