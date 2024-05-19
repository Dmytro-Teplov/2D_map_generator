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
uniform float u_brush_hardness;
uniform float u_brush_opacity;
uniform int u_brush_height;
uniform bool u_explicit_height;
uniform int u_brush;

//precision highpfloat;

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
float gradientNoise(vec2 uv)
{
    return fract(52.9829189 * fract(dot(uv, vec2(0.06711056, 0.00583715))));
}

void main()
{   
    vec2 radius = (gl_FragCoord.xy - u_pos) * 2.0;

    if (length(radius) < u_circle_size)
    {
        if (u_brush == 1)
            if (u_explicit_height)
                color = vec4(u_brush_height * (1.0 / 255.0), 0.0, 1.0 - u_brush_height * (1.0 / 255.0), (1.0 - ((length(radius) / u_circle_size) - u_brush_hardness) / (1 - u_brush_hardness)) * u_brush_opacity);
            else
                color = vec4(1.0, 0.0, 0.0, (1.0 - ((length(radius) / u_circle_size) - u_brush_hardness) / (1 - u_brush_hardness)) * u_brush_opacity);
        else
            color = vec4(0.0, 0.0, 1.0, (1.0 - ((length(radius) / u_circle_size) - u_brush_hardness) / (1 - u_brush_hardness)) * u_brush_opacity);

            //color = vec4(0.0, 0.0, 1.0 * u_brush_opacity, 1.0 - ((length(radius) / u_circle_size) - u_brush_hardness) / (1 - u_brush_hardness));
        //color += (1.0 / 127.0) * gradientNoise(gl_FragCoord.xy + u_pos) - (0.5 / 127.0);
    }
    else
    {
        discard;
    }
    //color = vec4(1.0);
    
};