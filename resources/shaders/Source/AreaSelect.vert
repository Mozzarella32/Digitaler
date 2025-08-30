//AreaSelect.vert

#version 420 core

layout(location = 0) in vec2 InA;
layout(location = 1) in vec2 InB;
layout(location = 2) in vec3 InColor;

out vec2 VSA;
out vec2 VSB;
out vec3 VSColor;

void main() {
    VSA = InA;
    VSB = InB;
    VSColor = InColor;
}
