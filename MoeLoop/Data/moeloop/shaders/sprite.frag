#version 450 core

layout(location = 0) in vec2 tex_coords;

layout(set=0, binding=0) uniform sampler sprite_sampler;
layout(set=1, binding=0) uniform texture2D sprite_texture;

layout(push_constant) uniform PushConstantF {
    float fade;
} pc;

layout(location = 0) out vec4 out_FragColor;

void main()
{
    out_FragColor = texture(
        sampler2D(sprite_texture, sprite_sampler), 
        tex_coords
    );
    out_FragColor.a *= pc.fade;
}
