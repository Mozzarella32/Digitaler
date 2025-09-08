//LiquidGlass.vert

#version 420 core

//In
layout(location = 0) in vec2 InPosition;

//Out
out vec2 TextureCoord;

//Uniforms
uniform mat3 UViewProjectionMatrix;

void main() {
    vec3 ProjctionAffin = UViewProjectionMatrix*vec3(InPosition.xy,1.0);
    TextureCoord = (InPosition.xy + 1.0) / 2.0;
    gl_Position = vec4(InPosition.xy, 0.0, 1.0);
}

