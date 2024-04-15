#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
out vec2 texcoord;



void main()
{
    gl_Position = position;
    texcoord = texCoord;
};


#shader fragment
#version 330 core

in vec2 texcoord;

out vec4 color;

uniform sampler2D texture1;

uniform vec4 u_Color;
uniform float u_BgRes;
uniform float u_complexity;
uniform float u_scale1;
uniform float u_scale2;


#ifdef GL_ES
precision mediump float;
#endif


vec2 randomGradient(vec2 p)
{
    p = p + 0.02;
    float x = dot(p, vec2(123.4, 234.5));
    float y = dot(p, vec2(234.5, 345.6));
    vec2 gradient = vec2(x, y);
    gradient = sin(gradient);
    gradient = gradient * 43758.5453;

    gradient = sin(gradient);
    return gradient;

}

float sdfCircle(in vec2 p, in float r)
{
    return length(p) - r;
}


float sdfOrientedBox(in vec2 p, in vec2 a, in vec2 b, float th)
{
    float l = length(b - a);
    vec2 d = (b - a) / l;
    vec2 q = (p - (a + b) * 0.5);
    q = mat2(d.x, -d.y, d.y, d.x) * q;
    q = abs(q) - vec2(l, th) * 0.5;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

vec2 cubic(vec2 p)
{
    return p * p * (3.0 - p * 2.0);
}


float perlin_noise(vec2 uv)
{
    vec3 black = vec3(0.0);
    vec3 white = vec3(1.0);
    vec3 color = black;

    uv = uv * 4.0;
    vec2 gridId = floor(uv);
    vec2 gridUv = fract(uv);
    color = vec3(gridId, 0.0);
    color = vec3(gridUv, 0.0);

    vec2 bl = gridId + vec2(0.0, 0.0);
    vec2 br = gridId + vec2(1.0, 0.0);
    vec2 tl = gridId + vec2(0.0, 1.0);
    vec2 tr = gridId + vec2(1.0, 1.0);

    vec2 gradBl = randomGradient(bl);
    vec2 gradBr = randomGradient(br);
    vec2 gradTl = randomGradient(tl);
    vec2 gradTr = randomGradient(tr);

    vec2 distFromPixelToBl = gridUv - vec2(0.0, 0.0);
    vec2 distFromPixelToBr = gridUv - vec2(1.0, 0.0);
    vec2 distFromPixelToTl = gridUv - vec2(0.0, 1.0);
    vec2 distFromPixelToTr = gridUv - vec2(1.0, 1.0);

    float dotBl = dot(gradBl, distFromPixelToBl);
    float dotBr = dot(gradBr, distFromPixelToBr);
    float dotTl = dot(gradTl, distFromPixelToTl);
    float dotTr = dot(gradTr, distFromPixelToTr);
    gridUv = cubic(gridUv);
    float b = mix(dotBl, dotBr, gridUv.x);
    float t = mix(dotTl, dotTr, gridUv.x);
    float perlin = mix(b, t, gridUv.y);
    return perlin;
}

float fBm(vec2 p)
{
    float f = 0.0;
    float w = 0.5;
    for (int i = 0; i < u_complexity; i++)
    {
        f += w * perlin_noise(p);
        p *= 2.0;
        w *= 0.5;
    }
    return f;
}


void main()
{
    //get the blend mask
    vec4 proxy = texture2D( texture1, texcoord);
    
    //vec4 proxyRight = texture2D(texture1, vec2(dFdx(texcoord.x), 0.0));
    //vec4 proxyUp = texture2D(texture1, texcoord + vec2(0.0, dFdy(texcoord.y)));

    //get the noise
    vec4 water_noise = vec4(vec3(fBm(vec2(texcoord.x * u_BgRes, texcoord.y) * u_Color.x)) + 0.5f, 1.f) * proxy.z;
    float terrain_noise_small = clamp(fBm(vec2(texcoord.x * u_BgRes, texcoord.y) * u_scale1) + 0.5, 0, 1);
    //float voronoi = clamp(voronoi2d(vec2(texcoord.x * u_BgRes, texcoord.y) * u_scale1*10), 0, 1);

    //float terrain_noise_small = perlin_noise(vec2(texcoord.x * u_BgRes, texcoord.y) * u_scale1); //figure out why whole noise is less than 0.5
    float terrain_noise_big = clamp(fBm(vec2(texcoord.x * u_BgRes, texcoord.y) * u_scale2) + 0.5, 0, 1);
    
    float terrain = clamp(proxy.x, 0.0, 1.0);
    //initialize the water and terrain mask
    float terrain_mask = clamp((terrain_noise_small * terrain_noise_big), 0, 1.0) * (1 - proxy.z);
    terrain_mask = mix(terrain_mask, terrain, terrain);
    float water_mask = clamp((terrain_noise_small + terrain_noise_big * 4) / 10, 0.0, 0.5) * proxy.z;

    color.r = terrain_mask;
    color.b = water_mask;
    color.g = terrain_noise_big;
    color.a = terrain_noise_small;
};
