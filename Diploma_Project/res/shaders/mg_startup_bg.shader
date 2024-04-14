#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 texcoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(position.xyz, 1.0);
    texcoord = texCoord;
};


#shader fragment
#version 330 core

in vec2 texcoord;
//in vec4 gl_FragCoord;

out vec4 color;

//uniform vec2 u_resolution;
//uniform vec2 u_pos;
uniform sampler2D bg_texture;


#ifdef GL_ES
precision mediump float;
#endif


float getColorBasedOnDistance(vec2 fragCoord)
{
    
    vec2 center = vec2(0.0, 0.0);
    float distance = length(fragCoord - center);

    float normalizedDistance = distance / sqrt(2.0); 

    float color = mix(0.0, 1.0, normalizedDistance);

    return color;
}

void main()
{   
    float brightness = getColorBasedOnDistance(texcoord);
    vec4 t = texture2D(bg_texture, texcoord);
    color = t * brightness; 
};