#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 assetPos;
layout(location = 3) in int asset_ID;

out vec2 texcoord;
out vec2 global_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 u_asset_view;
uniform float u_asset_scale;
uniform int u_asset_type;
uniform int u_isFB;
uniform int u_assets_amount;

void main()
{
    vec3 pos = position.xyz * u_asset_scale;
    //gl_Position = projection * view * model * vec4(pos + vec3(assetPos.xy,0), 1.0);
    vec4 something = projection * u_asset_view * model * vec4(pos + assetPos, 1.0);
    //gl_InstanceID = assetPos.z;
    int asset_num = asset_ID % u_assets_amount;
    texcoord = texCoord * vec2(0.125) + vec2(asset_num * 0.125, 0.875 - 0.125 * u_asset_type);
    global_texcoord = vec2(something.x / 2 + 0.5, something.y / 2 + 0.5);    
    if (u_isFB == 0)
    {
        gl_Position = projection * view * model * vec4(pos + vec3(assetPos.xy, 0), 1.0);
    }
    else
    {
        gl_Position = projection * u_asset_view * model * vec4(pos + vec3(assetPos.xy, 0), 1.0);
    }
};


#shader fragment
#version 330 core

in vec2 texcoord;
in vec2 global_texcoord;

out vec4 color;

uniform sampler2D asset_texture;
uniform sampler2D background;

uniform float u_BgRes;
uniform vec4 u_water_color;

#ifdef GL_ES
precision mediump float;
#endif

void main()
{
    vec4 asset_color = texture2D(asset_texture, texcoord);
    vec4 bg_color = texture2D(background, global_texcoord);
    
    if (bg_color.rba == vec3(0.0))
    {
        discard;
    }
    else
    {
        color = vec4(asset_color);
    }
};