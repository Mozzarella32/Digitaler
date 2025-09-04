//Background.frag

#version 420 core

//In
in vec2 ScreenPos;

//Out
out vec4 FragColor;

//Uniforms
uniform vec2 UMousePos;

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

void main(){
//	FragColor = vec4(1.0,0.0,0.0,1.0-sdCross(ScreenPos, vec2(1.0),0));
//	return;
	if(min(abs(ScreenPos.x), abs(ScreenPos.y)) < 0.04 && abs(ScreenPos.x) <0.4 && abs(ScreenPos.y) < 0.4){
		FragColor = vec4(1.0,0.0,0.0,1.0);
		return;
	}

	if(length(ScreenPos-UMousePos) < 0.11){
		FragColor = vec4(1.0,0.0,0.0,1.0);
		return;
	}

	// vec3 offset = vec3(
	// 	10*ScreenPos.x,
	// 	10*ScreenPos.y,
	// 	10*(ScreenPos.x+ScreenPos.y)
	// );
////	offset += vec3(
////		UTime*M_PI*2.0,
////		UTime*M_PI*2.0 + M_PI / 3.0,
////		UTime*M_PI*2.0 + 2.0 * M_PI / 3.0
////	);
	// vec3 colour = (sin(offset) + 1 )/2.0;
////	vec3 colour = (sin(offset) + 1 )/2.0;
	// FragColor = vec4(colour ,1.0);
	// return;
//	vec2 offset = vec2(cos(UTime*2*M_PI),sin(UTime*2*M_PI));
////	FragColor = vec4((ScreenPos+offset)*0.25,0.0,1.0);
//	return;

	if(!UShowDots){
		FragColor = vec4(0.05,0.05,0.05,1.0);
		return;
	}

	float box = sdBoxRounded(fract(ScreenPos+0.5)-0.5,vec2(0.05,0.05),0.025);
	box *= -1;
	FragColor = vec4(clamp(10*box+0.25,0.05,0.3)*vec3(1,1,1),1.0);
	// FragColor = vec4(clamp(1000*box+250,0.05,0.5)*vec3(1,1,1),1.0);

//	float c = sdCross(fract(ScreenPos+0.5)-0.5);
//	FragColor = vec4(1.0,1.0,1.0,c);
}
