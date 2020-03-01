#version 460 core
out vec4 FragColor;

//in vec3 fNorm;
//in flat uint col;
in vec2 rPos;


uniform sampler2D render;

//uniform float colors[9];

void main()
{
   // FragColor = vec4(vec3(0.3, 0.4, 0.5) * dot(fNorm, vec3(1.0, 0.0, 1.0)), 1.0) * 0.1 + vec4(0.1,0.1,0.1,1.0);
    //FragColor = vec4(0.376, 0.502, 0.22, 1.0) + vec4(0.529f, 0.808f, 0.922f, 1.0f) * dot(fNorm, vec3(1.0, 0.0, 1.0)) * 0.1; 
    FragColor = texture(render, rPos);
}