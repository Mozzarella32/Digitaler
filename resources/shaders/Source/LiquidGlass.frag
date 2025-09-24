//LiquidGlass.frag

#version 420 core

//In
in vec2 TextureCoord;

//Out
out vec4 FragColor;

//Uniforms
uniform usampler2D UBluredBase;
uniform sampler2D UTex;
uniform vec3 UColor;

uniform float UTime;

void main() {
	uvec4 pix = texture(UBluredBase, TextureCoord);

	float read = pix.r / 4294967295.0;

	float val = 1.0 - read;
	val *= val;
	val *= val;
	val *= val;

	float valgrad = 1.0 - read;
	vec2 gradient = vec2(dFdx(valgrad), dFdy(valgrad));
	// FragColor = vec4(abs(gradient) * 1.0, 0.0, 1.0);
	if(read == 0) {
		vec4 Color = texture(UTex, TextureCoord);
		FragColor = vec4(Color.rgb, 1.0);
		return;
	}

	vec4 Color = texture(UTex, TextureCoord);
	val *= 0.5;
	val = max(val, 0.2);

	float dist = read - 1.0;

	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist *= dist;
	dist = 1.0 - dist;

	val *= dist;

	vec4 Highlight = vec4(vec3(3.0) * val, 1.0);

	float a = 2.3;

	float h = dot(normalize(gradient), vec2(cos(a), sin(a)));

	h *= 0.05 + sign(h) * 1.0 / (1.0 + 0.05);

	h *= h;
	h *= h;

	// FragColor = vec4(vec3(10.0 * (val * val - 0.05)), 1.0);
	// FragColor = vec4(vec3(h), 1.0);
	// FragColor = vec4(vec3((val - 0.2) * h), 1.0);
	// return;
	h *= max(40.0 * (val * val - 0.05), 0.0);

	vec3 mixed = mix(Color.rgb, UColor, val);

	mixed = mix(mixed, vec3(1.0), clamp(h, 0.0, 1.0));

	FragColor = vec4(mixed, 1.0);
}
