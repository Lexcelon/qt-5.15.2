#version 440

layout(location = 0) in vec4 qt_Vertex;
layout(location = 1) in vec2 qt_MultiTexCoord0;

layout(location = 0) out vec2 qt_TexCoord0;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    vec4 qt_SubRect_source;
} ubuf;

void main()
{
    qt_TexCoord0 = ubuf.qt_SubRect_source.xy + ubuf.qt_SubRect_source.zw * qt_MultiTexCoord0;
    gl_Position = ubuf.qt_Matrix * qt_Vertex;
}
