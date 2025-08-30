//AreaSelect.frag

#version 420 core

//In
in vec2 PosInWorld;

//Out
out vec4 FragColor;
// layout(location = 1)out uint Id;

//Uniform
uniform float UZoomFactor;

flat in vec2 A;
flat in vec2 B;
flat in vec3 Color;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

	return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

void main(){
	float expr = -1.0+10*sdBox(PosInWorld,A,B);
	expr += 1.0;
	if(expr > 0){
	
		FragColor = vec4(0.0);

	}
	else{
		if(expr > -UZoomFactor*100.0){
			FragColor = vec4(Color,0.5);
		}
		else {
			FragColor = vec4(Color, step(expr,0.0)*0.1);
		}
	}
}
