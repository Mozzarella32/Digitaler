//Gaussian.vert

#version 420 core

//In
layout(location = 0) in vec2 InPosition;

out vec2 TextureCoord;

void main() {
    TextureCoord = (InPosition.xy + 1.0) / 2.0;
    gl_Position = vec4(InPosition.xy, 0.0, 1.0);
}

