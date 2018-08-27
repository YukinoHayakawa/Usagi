#version 450 core

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_TexCoords;
layout(location = 2) in vec3 in_Color;

layout(push_constant) uniform PushConstant {
    vec2 u_screenDimensions;
} pc;

layout(location = 0) out vec2 v_TexCoords;
layout(location = 1) out vec4 v_Color;

void main()
{
    // Map to normalized clip coordinates:
    float x = ((2.0 * (in_Position.x - 0.5)) / pc.u_screenDimensions.x) - 1.0;
    float y = ((2.0 * (in_Position.y - 0.5)) / pc.u_screenDimensions.y) - 1.0;

    gl_Position = vec4(x, y, 0.0, 1.0);
    v_TexCoords = in_TexCoords;
    v_Color     = vec4(in_Color, 1.0);
}
