#version 440

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 matrix;
    vec4 color;
} ubuf;

void main()
{
    fragColor = ubuf.color;
}
