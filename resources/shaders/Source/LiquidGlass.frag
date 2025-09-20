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
	valgrad *= valgrad;
	valgrad *= valgrad;
	valgrad *= valgrad;
	valgrad *= valgrad;
	valgrad *= valgrad;
	valgrad *= valgrad;
	valgrad = log(valgrad + 1.0);
	valgrad = log(valgrad + 1.0);
	valgrad = log(valgrad + 1.0);
	valgrad = log(valgrad + 1.0);
	// valgrad = log(valgrad + 2.0);
	// valgrad = log(valgrad + 2.0);
	// valgrad = log(valgrad + 2.0);
	// valgrad = log(valgrad + 2.0);
	// valgrad -= 0.25;
	// valgrad = max(0.0, valgrad);
	// valgrad *= read;
	// valgrad = abs(valgrad);
	// float d = read;
	// d *= d;
	// d *= d;
	// d *= d;
	// d += 0.1;
	// d = 1.0 - d;
	// valgrad *= d;
	// d = exp(d);
	// d = exp(d);
	// d = exp(d);
	// d = exp(d);
	// valgrad = exp(-(4.0 * valgrad - 2.0) * (4.0 * valgrad - 2.0));
	// valgrad = 1.0 - valgrad;
	// valgrad *= valgrad;
	// valgrad *= valgrad;
	// valgrad *= valgrad;
	// valgrad *= valgrad;
	// valgrad *= valgrad;
	// valgrad = 1.0 - valgrad;

	// if(valgrad)

	vec2 gradient = vec2(dFdx(valgrad), dFdy(valgrad));
	// gradient = vec2(dFdx(gradient.y), dFdy(gradient.x));

	// gradient *= valgrad;

	// gradient = normalize(gradient) * length(gradient);
	// gradient *= exp(-(4.0 * length(10.0 * gradient) - 2.0) * (10.0 * length(gradient) - 2.0));

	// FragColor = vec4(normalize(gradient) * (1.0 - read) * 1.0, 0.0, 1.0);
	FragColor = vec4(abs(gradient) * 1.0, 0.0, 1.0);

	// FragColor = vec4(vec3(1.0 - read), 1.0);
	// FragColor = vec4(vec3(valgrad), 1.0);
	// return;

	if(read == 0) {
		vec4 Color = texture(UTex, TextureCoord);
		FragColor = vec4(Color.rgb, 1.0);
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
	// float a = UTime;

	float h = dot(gradient, vec2(cos(a), sin(a)));


	// return;

	// h *= h;

	// h *= 1.0;

	h *= 0.1 + sign(h) * 1.0 / (1.0 + 0.1);

	h *= 2.0;
	h = exp(h * h) - 1.0;
	h *= 15.0;
	// h = exp(h) - 1.0;

	// h -= 0.35;

	// h = max(h, 0);
	// h *= 2.0;

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
