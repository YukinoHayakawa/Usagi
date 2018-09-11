#version 450 core

layout(location = 0) in vec2 Frag_UV;
layout(location = 1) in vec4 Frag_Color;

layout(set=0, binding=0) uniform sampler s;
layout(set=1, binding=0) uniform texture2D t;

layout(location = 0) out vec4 Out_Color;

void main()
{
    Out_Color = Frag_Color * texture(sampler2D(t, s), Frag_UV);
}
