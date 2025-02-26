//Hex.vert

#version 330 core

//In
layout(location = 0) in vec2 InPosition;

//Out
out vec2 ScreenPos;

//Uniforms
uniform mat3 UViewProjectionMatrix;

void main() {
    vec3 ProjctionAffin = UViewProjectionMatrix*vec3(InPosition.xy,1.0);
    ScreenPos = ProjctionAffin.xy;
    gl_Position = vec4(InPosition.xy, 0.0, 1.0);
}

