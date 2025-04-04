//And.frag

#version 330 core

//In
in vec2 PosInWorld;
flat in vec2 Corner1;
flat in vec2 Corner2;
flat in vec2 Midpoint;
flat in vec3 HighlightColor;
flat in uint id;

//Out
layout(location = 0)out vec4 FragColor;
layout(location = 1)out uint Id;

//Uniforms
uniform vec4 UBodyColorA = vec4(0.2,0.2,0.2,1.0);
uniform bool UIDRun;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

	return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

void main(){
	float alpha = max(1.0 - 4 * sdBox(PosInWorld,Corner1,Corner2),
		6.25 - 4 * distance(PosInWorld,Midpoint));

   if(alpha < 0.0){
   		if(UIDRun){
			discard;
			return;
		}
		if(HighlightColor == vec3(0)){
			FragColor = vec4(0.0);
		}
		else {
			FragColor = vec4(HighlightColor, alpha + 1.0);
		}
//		FragColor = vec4(UHighlightColor, alpha + 1.0);
//       FragColor = vec4(UHighlightColor ,max(alpha+1.0,0.1));
//		if(gl_FragDepth == 0.0) {
//			Id = 0u;
//		}
//		Id = 0u;
		return;
	}
	if(UIDRun){
		Id = id;
		return;
	}
	FragColor = UBodyColorA;
//	gl_FragDepth = 1.0;
//	Id = id;
//	Id = uvec4(id & 65535u,(id >> 16) & 65535u,0,1);

}
