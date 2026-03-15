#version 460 core
layout(location = 0) in vec3 p;

uniform float range;

void main()
{
    gl_Position = vec4(p.x / range, p.y / range, p.z, 1.0f);
}