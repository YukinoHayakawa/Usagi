#version 450 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_ColorPointSize;

layout(push_constant) uniform PushConstant {
    mat4 u_MvpMatrix;
} pc;

layout(location = 0) out vec4 v_Color;

void main()
{
    gl_Position  = pc.u_MvpMatrix * vec4(in_Position, 1.0);
    gl_PointSize = in_ColorPointSize.w;
    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);
}
