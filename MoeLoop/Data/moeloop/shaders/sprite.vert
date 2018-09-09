#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec2 in_uv0;
layout(location = 2) in vec2 in_uv1;

layout(push_constant) uniform PushConstant {
    mat4 mvp_matrix;
} pc;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) out vec2 out_uv0;
layout(location = 1) out vec2 out_uv1;

void main()
{
    gl_Position = pc.mvp_matrix * vec4(in_pos, 1.0);
    out_uv0 = in_uv0;
    out_uv1 = in_uv1;
}
