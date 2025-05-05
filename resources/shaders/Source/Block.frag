//Block.frag

#version 420 core

//In
in vec2 PosInWorld;

//Out
layout(location = 0)out vec4 FragColor;
layout(location = 1)out uint Id;

flat in vec2 A;
flat in vec2 B;
flat in vec4 Color;
flat in vec3 HighlightColor;
flat in uint id;

//Uniforms
uniform bool UIDRun;

//flat in int Flags;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

	return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}


//
//float sdBox( in vec2 p, in vec2 b )
//{
//    vec2 d = abs(p)-b;
//    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
//}
//
//
//float sdBoxAB( in vec2 p, in vec2 a, in vec2 b)
//{
//   return sdBox(p+a,a-b),
//}

void main(){
	float expr = -1.0+10*sdBox(PosInWorld,A,B);
	if(expr > 0){
		if(UIDRun){
			discard;
			return;
		}
//        FragColor = vec4(1.0,1.0,0.0,0.1);
		if(HighlightColor== vec3(0)) {
			FragColor = vec4(0);
		}
		else {
			FragColor = vec4(HighlightColor,max(1.0-0.5*373.0/416.0*expr,0.0));
		}
//		if(gl_FragDepth == 0.0) {
//		Id = 0u;
//			gl_FragDepth = 0.0;
//		}
//		else{
//			gl_FragDepth = 0.0;
//		}
//		gl_FragDepth = gl_FragDepth;
	}
	else{
		if(UIDRun){
			Id = id;
			return;
		}
		FragColor = vec4(Color.rgb,min(step(expr,0.0),Color.a));
//		gl_FragDepth = 1.0;
//		Id = uvec4(id & 65535u,(id >> 16) & 65535u,0,1);
	}
//	gl_FragDepth = 0.231242;

//    FragColor = vec4(1.0,0.0,0.0,1.0);
//    FragColor = vec4(1.0,1.0,0.0,1.0);
//    FragColor = vec4(PosInWorld.rg,0,0.5);
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);

//float segDist = sdSegment(ScreenPos,A,B);
//    float dist = 1.0-10*segDist;
//    float cuttoff = step(-0.1,dist);
//	FragColor = vec4(0.941,0.286,0.351,cuttoff);
//    if(Flags % 2 == 1 && cuttoff == 0.0){
//	    FragColor = vec4(1.0,1.0,0.0,1.0-3.0*segDist);
//    }
