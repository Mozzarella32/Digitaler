//SevenSeg.frag

#version 420 core

//In
in vec2 PosInWorld;
in vec2 ScreenPos;
flat in vec3 Color;
flat in int Flags;
flat in vec2 AL;
flat in vec2 AR;
flat in vec2 BU;
flat in vec2 BD;
flat in vec2 CU;
flat in vec2 CD;
flat in vec2 DL;
flat in vec2 DR;
flat in vec2 EU;
flat in vec2 ED;
flat in vec2 FU;
flat in vec2 FD;
flat in vec2 GL;
flat in vec2 GR;
flat in vec2 P;//Point

//Out
out vec4 FragColor;

//Uniforms
uniform vec3 USevenSegOffColor = vec3(0.2,0.2,0.2);

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	vec2 r = ba*h;
	return abs(pa.x-r.x)+abs(pa.y-r.y);
}


void main(){

	float A = 1.0-4.0*sdSegment(PosInWorld,AL,AR);
	float B = 1.0-4.0*sdSegment(PosInWorld,BU,BD);
	float C = 1.0-4.0*sdSegment(PosInWorld,CU,CD);
	float D = 1.0-4.0*sdSegment(PosInWorld,DL,DR);
	float E = 1.0-4.0*sdSegment(PosInWorld,EU,ED);
	float F = 1.0-4.0*sdSegment(PosInWorld,FU,FD);
	float G = 1.0-4.0*sdSegment(PosInWorld,GL,GR);
	float Point = 1.0-4.0*length(P-PosInWorld);

	float On = 
	   max(max(max(A*float((Flags & 0x1) != 0),
	   B*float((Flags & 0x2) != 0)),
	   max(C*float((Flags & 0x4) != 0),
	   D*float((Flags & 0x8) != 0))),
	   max(max(E*float((Flags & 0x10) != 0),
	   F*float((Flags & 0x20) != 0)),
	   max(G*float((Flags & 0x40) != 0),
	   Point*float((Flags & 0x80) != 0))));

	float Off = 
	   max(max(max(A*float((Flags & 0x1) == 0),
	   B*float((Flags & 0x2) == 0)),
	   max(C*float((Flags & 0x4) == 0),
	   D*float((Flags & 0x8) == 0))),
	   max(max(E*float((Flags & 0x10) == 0),
	   F*float((Flags & 0x20) == 0)),
	   max(G*float((Flags & 0x40) == 0),
	   Point*float((Flags & 0x80) == 0))));

	if(On > Off){
		FragColor = vec4(Color,2.5*On-0.5);
		return;
	}
	FragColor = vec4(USevenSegOffColor,step(-Off,-0.4));
}
