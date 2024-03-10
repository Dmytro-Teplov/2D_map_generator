#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 texcoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 transform_;

uniform int u_isFb;

void main()
{
    if (u_isFb==0)
    {
        gl_Position = projection * view * model * vec4(position.xyz, 1.0);
        texcoord = texCoord;
    }
    else
    {
        gl_Position = projection * transform_ * model * vec4(position.xyz, 1.0);
        texcoord = texCoord;
    }
        
};


#shader fragment
#version 330 core

in vec2 texcoord;

out vec4 color;

uniform sampler2D texture1;

uniform vec4 u_Color;
uniform vec3 u_sun_pos;
uniform float u_outline_thickness;
uniform float u_outline_hardness;
uniform float u_BgRes;
uniform vec4 u_terrain_color;
uniform vec4 u_water_color;
uniform vec4 u_outline_color;
uniform vec4 u_terrain_secondary_c;
uniform vec4 u_water_secondary_c;
uniform bool u_use_outline;
uniform bool u_use_secondary_tc;
uniform bool u_use_secondary_wc;


#ifdef GL_ES
precision mediump float;
#endif

vec4 sample_terrain(vec2 texcoord)
{
    vec4 t = texture2D(texture1, texcoord);
    vec4 res;
    float terrain_mask = clamp(t.r + t.b, 0.0, 1.0);
    if (terrain_mask <= 0.5)
        res = vec4(0.0, 0.0, 0.0, 1.0);
    if (terrain_mask > 0.5)
        res = vec4(1.0, 1.0, 1.0, 1.0);
    return res;
}

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
    float terrain_mask = clamp(text.r + text.b, 0.0, 1.0);
    if (terrain_mask <= 0.5)//water
        if (u_use_secondary_wc)
            color = mix(u_water_color, u_water_secondary_c, terrain_mask / 0.5);
        else
            color = u_water_color;

    if (terrain_mask > 0.25 && terrain_mask < 0.3 && (text.g > 0.05 || text.a > 0.15))
        color =  vec4(0.9, 0.9, 0.9, 1.0);
    
    if (terrain_mask > 0.5)//terrain
        if (u_use_secondary_tc)
            color = mix(u_terrain_color, u_terrain_secondary_c, (terrain_mask - 0.6) / 0.35);
        else
            color = u_terrain_color;
    //color = mix(u_water_color, u_terrain_color, text.r);
    if (u_use_outline)
    {
        float outlineWidth = u_outline_thickness / 512;
        
        //float hardness = (1 - (sqrt(i * i + j * j) * 2) / brush_size) * brush_hardness;
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                vec2 offset = vec2(x, y * u_BgRes) * outlineWidth;
                vec4 neighborColor = sample_terrain(texcoord + offset);
                vec4 currColor = sample_terrain(texcoord);
                if (neighborColor.r == 1.0f && currColor.r < 0.5f)
                {
                    
                    color = u_outline_color;
                }

            }
        }
    }
    
};