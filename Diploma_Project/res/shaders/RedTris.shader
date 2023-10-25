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

void main()
{
    color = texture2D( texture1, texcoord) * u_Color;
    //color = u_Color;
};