//LiquidGlass.frag

#version 420 core

//In
in vec2 TextureCoord;

//Out
out vec4 FragColor;

//Uniforms
uniform usampler2D UBluredBase;
uniform vec3 UColor;

void main() {
	uvec4 pix = texture(UBluredBase, TextureCoord);

	float read = (float(pix.r) / 255.0) * 0.5;

	float val = 1.0 - read;

	float valgrad = val;
	valgrad *= valgrad;
	// val *= val;
	// val *= val;
	// val *= val;

	vec2 gradient = vec2(dFdx(valgrad), dFdy(valgrad));

	if(read == 0) {
		FragColor = vec4(0.0);
		return;
	}

	val *= 0.5;

	val = max(val, 0.2);

	float dist = read - 1.0;

	// dist = 1.0 - dist;
	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist = 1.0 - dist;

	val *= dist;

	vec4 Highlight = vec4(vec3(3.0) * val, 1.0);

	float h = abs(dot(gradient, normalize(vec2(-0.75,1.0))) * 6.0);

	h *= h;
	h *= h;
	h *= 4.0;
	// h = sin(h * 3.1415);

	// FragColor = vec4(1.0,1.0,1.0,h);
	// return;
	// h = 1.0 - h;

	// FragColor = vec4(dot(gradient.xy, vec2(1.0)) * 10.0, 0.0, 0.0, 0.5);
	
	FragColor = mix(vec4(UColor, val), Highlight, h);
}
