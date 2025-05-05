//Pin.frag

#version 420 core

//In
in vec2 PosInWorld;
in vec2 ScreenPos;

flat in vec2 PosOfPin;
flat in vec2 A;
flat in vec2 B;
flat in vec3 Color;
flat in vec3 HighlightColor;
flat in uint id;

//Out
layout(location = 0)out vec4 FragColor;
layout(location = 1)out uint Id;

//uniform
uniform bool UHasWire;
uniform sampler2D UPath;
uniform bool UIDRun;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

	return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

void main(){

	float alpha1 = 1.0-10*step(length(PosInWorld-PosOfPin),0.11)-step(1.0-4.5*sdBox(PosInWorld,A,B),0.0);
	vec4 OldColor = vec4(Color,alpha1);

	float alpha = 1.0-4.5*sdBox(PosInWorld,A,B)+1.0;
	
//	FragColor = vec4(vec3(alpha1),alpha1);
//	return;

	if(UIDRun){
		if(alpha > 1.0 && alpha1 == 1.0){
			Id = id;
			return;
		}
		else {
			discard;
			return;
		}
	}

	if(alpha > 1.0 && alpha1 != 1.0){
		if(UHasWire){
			vec3 Path = texture(UPath,ScreenPos).rgb;
			float shouldhavedot = step(distance(PosInWorld,PosOfPin),0.05);
			float hasdot = float(Path.r == 0.0) * float(Path.g == 1.0) * float(Path.b == 1.0);
			FragColor = vec4(mix(mix(Path,vec3(0.0,1.0,0.2),shouldhavedot),Path,hasdot),1.0);
			return;
		}
		FragColor = vec4(0.05,0.05,0.05,1.0);
		return;
	}
//	if(HighlightColor == vec3(0)){
//		FragColor = vec4(0.0);
//	}
//	else {
	alpha = HighlightColor == vec3(0) ? 0.0 : alpha;
	FragColor = (1.0-OldColor.a)*vec4(HighlightColor,alpha)+OldColor.a*OldColor;
//	}
}
