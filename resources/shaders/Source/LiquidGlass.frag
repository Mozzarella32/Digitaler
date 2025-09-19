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

	float valgrad = val;
	val *= val;
	val *= val;
	val *= val;

	vec2 gradient = vec2(dFdx(valgrad), dFdy(valgrad));
	// vec2 gradient = vec2(dFdx(read), dFdy(read));

	// FragColor = vec4(gradient * 100.0, 0.0, 1.0);
	// return;

	if(read == 0) {
		vec4 Color = texture(UTex, TextureCoord);
		FragColor = Color;
		return;
	}

	vec4 Color = texture(UTex, TextureCoord);

	// FragColor = vec4(gradient * 10.0, 0.0, 0.5);
	// FragColor = vec4(vec3(read), 0.5);
	// return;

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

	float a = 2.3;

	float h = dot(gradient, vec2(cos(a), sin(a))) * 10.0;

	h *= 0.15 + sign(h) * 1.0 / (1.0 + 0.15);

	h -= 0.25;

	h = max(h, 0);

	// h *= h;
	// h *= h;
	// h = exp(h);
	// h -= 1;
	// h = exp(h);
	// h -= 1;
	// h = exp(h);
	// h -= 1;
	// h = exp(h);
	// h -= 1;
	// h = exp(h);
	// h -= 1;
	h *= 3.0;

	// h = sqrt(h);
	// h *= 20.0;
	// h = sin(h * 3.1415);

	// FragColor = vec4(1.0,1.0,1.0,h);
	// return;
	// h = 1.0 - h;

	// FragColor = vec4(dot(gradient.xy, vec2(1.0)) * 10.0, 0.0, 0.0, 0.5);
	// FragColor = vec4(vec3(h), 1.0);
	// return;

	vec3 mixed = mix(Color.rgb, UColor, val);

	mixed = mix(mixed, vec3(1.0), clamp(h, 0.0, 1.0));

	FragColor = vec4(mixed, 1.0);

	// vec4 col = mix(vec4(UColor, val), Highlight, clamp(h, 0.0, 1.0));

	// FragColor = vec4(mix(mix(Color, vec4(UColor, val)).rgb, 1.0), Highlight, ;
}
