//SixteenSeg.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in int InstanceInTransform;
layout(location = 3) in int InstanceInFlags;
layout(location = 4) in vec3 InstanceInColor;

//Out
out vec2 PosInWorld;
out vec2 PosInWorldUnshifted;
out vec2 ScreenPos;
flat out vec3 Color;
flat out int Flags;
flat out vec2 AL;
flat out vec2 AR;
flat out vec2 BL;
flat out vec2 BR;
flat out vec2 CU;
flat out vec2 CD;
flat out vec2 DU;
flat out vec2 DD;
flat out vec2 EL;
flat out vec2 ER;
flat out vec2 FL;
flat out vec2 FR;
flat out vec2 GU;
flat out vec2 GD;
flat out vec2 HU;
flat out vec2 HD;
flat out vec2 IL;
flat out vec2 IR;
flat out vec2 JL;
flat out vec2 JR;
flat out vec2 K;
flat out vec2 LU;
flat out vec2 LD;
flat out vec2 M;
flat out vec2 N;
flat out vec2 OU;
flat out vec2 OD;
flat out vec2 P;
flat out vec2 Point;
flat out float Rotate;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

// d r u t
float Size[4] = {
	-2.5,1.5,1.5,-1.5,
};

vec2 Offset = vec2(1.0,-1.0);//Because center and 2by2

float Directions[2*4] = {
	0,1,
	1,0,
	0,-1,
	-1,0
};

int IndexTable[2*4] = {
	0,1,
	0,3,
	2,1,
	2,3,
};

float Correction[2*4] = {
	0,0,
	1,0,
	0,-1,
	0,0,
};

void main() {

	int Rotation = InstanceInTransform & 0x3;
	int xflip = (InstanceInTransform >> 2) & 0x1;
	int yflip = (InstanceInTransform >> 3) & 0x1;
	vec2 flipx = vec2(1 - 2 * xflip);
	vec2 flipy = vec2(1 - 2 * yflip);
	int BetterRot = Rotation*(1-yflip)+(Rotation+2-4*int(floor((Rotation+2)/4)))*yflip;
	
	vec2 CorrectedPos = InstanceInPos + Offset + vec2(Correction[BetterRot*2+0],Correction[BetterRot*2+1]);

	vec2 Right = vec2(Directions[(Rotation+1)%4*2+0],Directions[(Rotation+1)%4*2+1]) * flipx;
	vec2 Up = vec2(Directions[(Rotation+0)%4*2+0],Directions[(Rotation+0)%4*2+1]) * flipy;

	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

	Rotate = float(Rotation == 0) + float(Rotation == 2);

	vec2 PosShifted = Pos;
	PosInWorldUnshifted = Pos;
	PosShifted = PosShifted - 0.125*(dot(PosShifted,Up) - dot(CorrectedPos,Up))*Right;
	PosInWorld = PosShifted;

	vec2 A = CorrectedPos - 0.5 * Up + 1.6 * Up - 0.4 * Right;
	vec2 B = CorrectedPos - 0.5 * Up + 1.6 * Up + 0.4 * Right;
	vec2 C = CorrectedPos - 0.5 * Up + 0.8 * Right + 0.8 * Up;
	vec2 D = CorrectedPos - 0.5 * Up + 0.8 * Right - 0.8 * Up;
	vec2 E = CorrectedPos - 0.5 * Up - 1.6 * Up + 0.4 * Right;
	vec2 F = CorrectedPos - 0.5 * Up - 1.6 * Up - 0.4 * Right;
	vec2 G = CorrectedPos - 0.5 * Up - 0.8 * Right - 0.8 * Up;
	vec2 H = CorrectedPos - 0.5 * Up - 0.8 * Right + 0.8 * Up;
	vec2 I = CorrectedPos - 0.5 * Up - 0.4 * Right;
	vec2 J = CorrectedPos - 0.5 * Up + 0.4 * Right;
	K = CorrectedPos - 0.5 * Up - 0.4 * Right + 0.8 * Up;
	vec2 L = CorrectedPos - 0.5 * Up + 0.8 * Up;
	M = CorrectedPos - 0.5 * Up + 0.4 * Right + 0.8 * Up;
	N = CorrectedPos - 0.5 * Up - 0.4 * Right - 0.8 * Up;
	vec2 O = CorrectedPos - 0.5 * Up - 0.8 * Up;
	P = CorrectedPos - 0.5 * Up + 0.4 * Right - 0.8 * Up;
	Point = CorrectedPos - 0.5 * Up - 1.6 * Up + 1.0 * Right; 

	AL = A - 0.2 * Right;
	AR = A + 0.2 * Right;
	BL = B - 0.2 * Right;
	BR = B + 0.2 * Right;
	CU = C + 0.6 * Up;
	CD = C - 0.6 * Up;
	DU = D + 0.6 * Up;
	DD = D - 0.6 * Up;
	EL = E - 0.2 * Right;
	ER = E + 0.2 * Right;
	FL = F - 0.2 * Right;
	FR = F + 0.2 * Right;
	GU = G + 0.6 * Up;
	GD = G - 0.6 * Up;
	HU = H + 0.6 * Up;
	HD = H - 0.6 * Up;
	IL = I - 0.2 * Right;
	IR = I + 0.2 * Right;
	JL = J - 0.2 * Right;
	JR = J + 0.2 * Right;
	LU = L + 0.6 * Up;
	LD = L - 0.6 * Up;
	OU = O + 0.6 * Up;
	OD = O - 0.6 * Up;

	Flags = InstanceInFlags;

	ScreenPos = (Pos+UOffset)/UZoom;

	Color = InstanceInColor;

	gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

