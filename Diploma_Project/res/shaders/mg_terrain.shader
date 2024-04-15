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
    }
    else
    {
        gl_Position = projection * transform_ * model * vec4(position.xyz, 1.0);
    }
    texcoord = texCoord;
        
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
uniform int u_steps_w;
uniform int u_steps_t;
uniform vec4 u_terrain_color;
uniform vec4 u_water_color;
uniform vec4 u_outline_color;
uniform vec4 u_terrain_secondary_c;
uniform vec4 u_water_secondary_c;
uniform bool u_use_outline;
uniform bool u_use_secondary_tc;
uniform bool u_use_secondary_wc;
uniform bool u_use_step_gradient_w;
uniform bool u_use_step_gradient_t;
uniform bool u_debug;


#ifdef GL_ES
precision mediump float;
#endif

vec4 sample_terrain(vec2 texcoord)
{
    vec4 t = texture2D(texture1, texcoord);
    vec4 res;
    float terrain_mask = clamp(t.r, 0.0, 1.0);
    if (terrain_mask <= 0.5)
        res = vec4(0.0, 0.0, 0.0, 1.0);
    if (terrain_mask > 0.5)
        res = vec4(1.0, 1.0, 1.0, 1.0);
    return res;
}

vec3 linearToSrgb(vec3 linearColor)
{
    vec3 srgbColor;
    for (int i = 0; i < 3; ++i)
    {
        srgbColor[i] = linearColor[i] <= 0.0031308 ? linearColor[i] * 12.92 : pow(linearColor[i], 1.0 / 2.4) * 1.055 - 0.055;
    }
    return srgbColor;
}


void main()
{   
    vec4 text = texture2D(texture1, texcoord);
        
    //blend water and terrain
    float terrain_mask = clamp(text.r, 0.0, 1.0);
    if (terrain_mask <= 0.5)
    { // WATER
        if (u_use_secondary_wc)
        {
            if (u_use_step_gradient_w)
            {
                float m = 0.f;
                float m2 = 0.f;
                for (int i = 0; i < u_steps_w;++i)
                {
                    m = i * (1.f / u_steps_w);
                    m2 = i * (0.5f / u_steps_w);
                    color = mix(color, mix(u_water_color, u_water_secondary_c, m), step(m2, terrain_mask));
                }
            }
            else
            {
                color = mix(u_water_color, u_water_secondary_c, terrain_mask / 0.5);
            }
        }
        else
        {
            color = u_water_color;
        }
        //color.rgb = vec3(text.a)
         

    }
            
    
    if (terrain_mask > 0.5)// TERRAIN
    {
        if (u_use_secondary_tc)
        {
            if (u_use_step_gradient_t)
            {
                float t = 0.f;
                float t2 = 0.f;
                for (int i = 0; i < u_steps_t; ++i)
                {
                    t = i * (1.f / u_steps_t);
                    t2 = i * (0.5f / u_steps_t)+0.5;
                    color = mix(color, mix(u_terrain_color, u_terrain_secondary_c, t), step(t2, terrain_mask));
                }
            }
            else
            {
                color = mix(u_terrain_color, u_terrain_secondary_c, (terrain_mask - 0.5) / 0.5);
            } 
        }  
        else
        {
            color = u_terrain_color;
        }
            
    }
        
    // OUTLINE
    if (u_use_outline)
    {
        float outlineWidth = u_outline_thickness / 512;
        
        //float hardness = (1 - (sqrt(i * i + j * j) * 2) / brush_size) * brush_hardness;
        //float xf = 0, yf = 0;
        for (float x = -1; x <= 1; x += 0.5)
        {
            for (float y = -1; y <= 1; y += 0.5)
            {
        //xf
                vec2 offset = vec2(x, y * u_BgRes) * outlineWidth;
                vec4 neighborColor = sample_terrain(texcoord + offset);
                vec4 currColor = sample_terrain(texcoord);
                if (neighborColor.r == 1.0f && currColor.r <= 0.5f)
                {
                    color = u_outline_color;
                }

            }
        }
    }
    
    
    
    color.a = 1.0f;
    
    
    if (u_debug)
    {
        color.rgb = vec3(text.r);
    }
};