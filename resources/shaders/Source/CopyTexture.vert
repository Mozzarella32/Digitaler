//CopyTexture.vert

#version 420 core

//In
layout(location = 0) in vec2 InPosition;
//Out
out vec2 TextureCoord;

void main() {
    TextureCoord = (InPosition+1.0)/2.0;
    gl_Position = vec4(InPosition, 0.0, 1.0);
}

