//LiquidGlass.frag

#version 420 core

//In
in vec2 TextureCoord;

//Out
out vec4 FragColor;

//Uniforms
uniform usampler2D UBluredBase;

void main() {
	uvec4 pix = texture(UBluredBase, TextureCoord);
	if(pix.r == 0) {
		FragColor = vec4(0.0, 1.0, 0.0, 0.5);
	}
	else {
		FragColor = vec4(1.0, 0.0, 0.5, 0.5);
	}
}
