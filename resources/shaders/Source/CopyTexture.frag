//CopyTexture.frag

#version 420 core

//In
in vec2 TextureCoord;
//Out
out vec4 FragColor;

uniform sampler2D UTexture;

void main(){
	FragColor = texture(UTexture, TextureCoord);
}
