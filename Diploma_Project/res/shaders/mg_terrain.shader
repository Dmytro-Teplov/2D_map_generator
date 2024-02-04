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

uniform sampler2D texture1;

uniform vec4 u_Color;
uniform vec3 u_sun_pos;
uniform float u_width;
uniform float u_height;
uniform float u_BgRes;


#ifdef GL_ES
precision mediump float;
#endif


void main()
{   
    vec4 text = texture2D(texture1, texcoord);
    
    ////blend water and terrain
    //if (text.a < 0.45)//water
    //    color = mix(vec4(0.2, 0.5, 0.5, 1.0), vec4(0.9, 0.8, 0.7, 1.0), text.a / 0.45);
    //if (text.a >= 0.45)//send shore
    //    color = mix(vec4(0.9, 0.8, 0.7, 1.0), vec4(0.7, 0.6, 0.5, 1.0), (text.a - 0.45) / 0.15);
    ////if (text.a >= 0.57)//outline
    ////    color = vec4(0.2, 0.2, 0.2, 1.0);
    //if (text.a > 0.6)//terrain
    //    color = mix(vec4(0.5, 0.6, 0.3, 1.0), vec4(0.7, 0.8, 0.5, 1.0), (text.a - 0.6) / 0.35);
    //if (text.a > 0.95)//rocky mountains
    //    color = vec4(0.9, 0.9, 0.9, 1.0);
    //int outlineWidth = 10;
    
    //blend water and terrain
    if (text.a <= 0.6)//water
        color = vec4(0.2, 0.5, 0.5, 1.0);
    if (text.a > 0.6)//terrain
        color = vec4(0.5, 0.6, 0.3, 1.0);

    int outlineWidth = 10;
    
    
    // apply an outline by checking the neighboring pixels
    
    

    //color = vec4(1.0);
    //color = vec4(text.a);
};