//Background.frag

#version 420 core

//In
in vec2 ScreenPos;

//Out
out vec4 FragColor;

//Uniforms
uniform vec2 UMousePos;
uniform float UZoomFactor;

uniform bool UShowDots = true;

uniform float UTime;

//Defines 
#define M_PI 3.1415926535897932384626433832795

float sdBoxRounded(vec2 p, vec2 b, float r) {
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0) - r;
}

float sdCross( in vec2 p, in vec2 b, float r ) 
{
    p = abs(p); p = (p.y>p.x) ? p.yx : p.xy;
    vec2  q = p - b;
    float k = max(q.y,q.x);
    vec2  w = (k>0.0) ? q : vec2(b.y-p.x,-k);
    return sign(k)*length(max(w,0.0)) + r;
}

float ApplyScaling(float sdf) {
    return 1.0 - 0.15 / UZoomFactor * sdf;
}

void main(){
	vec3 baseColor = vec3(0.05);

	if(!UShowDots){
		FragColor = vec4(vec3(0.05),1.0);
		return;
	}

	float box = sdBoxRounded(fract(ScreenPos+0.5)-0.5,vec2(0.05,0.05),0.025);

	vec3 boxColor = vec3(0.3);

	if (length(ScreenPos) < 0.50) {
		boxColor = vec3(0.6);
		box -= 0.1;

	}
	if (length(ScreenPos-UMousePos) < 0.50) {
		boxColor = vec3(1.0, 0.0, 0.0);
	}

	FragColor = vec4(mix(baseColor, boxColor, clamp(ApplyScaling(box), 0.0, 1.0)), 1.0);
}
