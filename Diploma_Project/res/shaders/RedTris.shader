#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
out vec2 texcoord;



void main()
{
   gl_Position = position;
   texcoord = position.xy;
};


#shader fragment
#version 330 core

in vec2 texcoord;

layout(location = 0) out vec4 color;

uniform sampler2D texture1;

uniform vec4 u_Color;

void main()
{
    color = texture2D(texture1, texcoord)*u_Color;
    //color = u_Color;
};