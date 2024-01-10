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

uniform vec2 u_resolution;
uniform vec2 u_pos;
uniform float u_circle_size;





#ifdef GL_ES
precision mediump float;
#endif

float circle(vec2 pos, float r)
{
    if (length(pos)<r)
        return 1.0;
    else
        return 0.0;
    
}

void main()
{   
    vec2 radius = (gl_FragCoord.xy - u_pos)*2.0;
    //radius.y = -radius.y;
    if (length(radius) < u_circle_size && length(radius) > u_circle_size-3)
    {
        color = vec4(1.0);
    }
    else
    {
        discard;
        //color = vec4(0.0);
    }
    //float circle3 = circle(position, u_circle_size);
    //color = vec4(circle3);
    
};