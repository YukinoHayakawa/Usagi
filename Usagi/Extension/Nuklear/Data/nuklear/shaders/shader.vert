#version 450 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec4 Color;

layout(push_constant) uniform PushConstant {
    vec2 scale;
    vec2 translate;
} pc;


layout(location = 0) out vec2 Frag_UV;
layout(location = 1) out vec4 Frag_Color;

void main()
{
    Frag_UV = TexCoord;
    Frag_Color = Color;
    gl_Position = vec4(Position.xy * pc.scale + pc.translate, 0, 1);
}
