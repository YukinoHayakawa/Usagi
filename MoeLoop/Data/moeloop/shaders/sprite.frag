#version 450 core

layout(location = 0) in vec2 uv0;
layout(location = 1) in vec2 uv1;

layout(set=0, binding=0) uniform sampler sprite_sampler;
// todo array
// todo only one texture?
layout(set=1, binding=0) uniform texture2D tex0;
layout(set=1, binding=1) uniform texture2D tex1;

struct Layer
{
    vec2 uv_begin;
    vec2 uv_end;
    float factor;
};

layout(push_constant) uniform PushConstant {
    // excluded from push constant range for this stage
    mat4 padding;
    // todo array support
    Layer layer0;
    Layer layer1;
} pc;

layout(location = 0) out vec4 out_FragColor;

// code from:
// https://stackoverflow.com/questions/12751080/glsl-point-inside-box-test
// explanation:
// call top_left v0 and bottom_right v1, v0 < v1 so they form a box.
// when v is compared against v0, four values can result and the results
// of comparing v and v1 are restricted by it:
// (0, 0) if v.x < v0.x && v.y < v0.y
//  in this case the second compare can only result in (0, 0) since v < v0 < v1
// (0, 1) if v.x < v0.x && v0.y < v.y
//  in this case the second compare can result in (0, 0/1)
// (1, 0) if v0.x < v.x && v.y < v0.y
//  this case (0/1, 0)
// (1, 1) if v0.x < v.x && v0.y < v.y
//  this case (0/1, 0/1)
// in any case, it is verifiable that only when both results are (1, 1), the
// multiplication of two components of the subtraction of the results is 1,
// and 0 in all other cases.
float insideBox(vec2 v, vec2 top_left, vec2 bottom_right)
{
    vec2 s = step(top_left, v) - step(bottom_right, v);
    return s.x * s.y;
}

void main()
{
    vec4 c0 = texture(sampler2D(tex0, sprite_sampler), uv0);
    vec4 c1 = texture(sampler2D(tex1, sprite_sampler), uv1);

    out_FragColor =
        insideBox(uv0, pc.layer0.uv_begin, pc.layer0.uv_end)
        * pc.layer0.factor * c0;
    out_FragColor +=
        insideBox(uv1, pc.layer1.uv_begin, pc.layer1.uv_end)
        * pc.layer1.factor * c1;
}
