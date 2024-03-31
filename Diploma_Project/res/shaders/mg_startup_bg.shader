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

out vec4 color;

//uniform vec2 u_resolution;
//uniform vec2 u_pos;
uniform sampler2D bg_texture;


#ifdef GL_ES
precision mediump float;
#endif


void main()
{   
    vec4 t = texture2D(bg_texture, texcoord);
    color = t; //vec4(1.0f);
    
};