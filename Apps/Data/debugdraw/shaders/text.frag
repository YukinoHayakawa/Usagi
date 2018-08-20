#version 450 core

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in vec4 v_Color;

layout(set=0, binding=0) uniform sampler u_glyphSampler;
layout(set=0, binding=1) uniform texture2D u_glyphTexture;

layout(location = 0) out vec4 out_FragColor;

void main()
{
    out_FragColor = v_Color;
    out_FragColor.a = texture(sampler2D(u_glyphTexture, u_glyphSampler), v_TexCoords).r;
}
