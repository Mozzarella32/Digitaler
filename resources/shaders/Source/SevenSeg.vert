//SevenSeg.vert

#version 420 core

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
flat out vec2 BU;
flat out vec2 BD;
flat out vec2 CU;
flat out vec2 CD;
flat out vec2 DL;
flat out vec2 DR;
flat out vec2 EU;
flat out vec2 ED;
flat out vec2 FU;
flat out vec2 FD;
flat out vec2 GL;
flat out vec2 GR;
flat out vec2 P;//Point

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

	int flipflip = int(Rotation == 1 || Rotation == 3); 

	int newxflip = xflip*(1-flipflip) + yflip * flipflip;
	int newyflip = yflip*(1-flipflip) + xflip * flipflip;

	xflip = newxflip;
	yflip = newyflip;

	vec2 flipx = vec2(1 - 2 * xflip);
	vec2 flipy = vec2(1 - 2 * yflip);
	int BetterRot = Rotation*(1-yflip)+(Rotation+2-4*int(floor((Rotation+2)/4)))*yflip;

	vec2 Right = vec2(Directions[(Rotation+1)%4*2+0],Directions[(Rotation+1)%4*2+1]) * flipx;
	vec2 Up = vec2(Directions[(Rotation+0)%4*2+0],Directions[(Rotation+0)%4*2+1]) * flipy;

	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 CorrectedPos = InstanceInPos + Offset + vec2(Correction[BetterRot*2+0],Correction[BetterRot*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

    vec2 PosShifted = Pos;
    PosInWorldUnshifted = Pos;
    PosShifted = PosShifted - 0.125*(dot(PosShifted,Up) - dot(CorrectedPos,Up))*Right;
    PosInWorld = PosShifted;

    vec2 A = CorrectedPos - 0.5 * Up + 1.6 * Up;
    vec2 B = CorrectedPos - 0.5 * Up + 0.8 * Right + 0.8 * Up;
    vec2 C = CorrectedPos - 0.5 * Up + 0.8 * Right - 0.8 * Up;
    vec2 D = CorrectedPos - 0.5 * Up - 1.6 * Up;
    vec2 E = CorrectedPos - 0.5 * Up - 0.8 * Right - 0.8 * Up;
    vec2 F = CorrectedPos - 0.5 * Up - 0.8 * Right + 0.8 * Up;
    vec2 G = CorrectedPos - 0.5 * Up;
    P = CorrectedPos - 0.5 * Up - 1.6 * Up + 1.0 * Right; 

//    vec2 A = CorrectedPos - 0.5 * Up + 1.6 * Up;
//    vec2 B = CorrectedPos - 0.5 * Up + 0.8 * Right + 0.8 * Up;
//    vec2 C = CorrectedPos - 0.5 * Up + 0.8 * Right - 0.8 * Up;
//    vec2 D = CorrectedPos - 0.5 * Up - 1.6 * Up;
//    vec2 E = CorrectedPos - 0.5 * Up - 0.8 * Right - 0.8 * Up;
//    vec2 F = CorrectedPos - 0.5 * Up - 0.8 * Right + 0.8 * Up;
//    vec2 G = CorrectedPos - 0.5 * Up;
//    P = CorrectedPos - 0.5 * Up - 1.6 * Up + 1.05* Right; 

    AL = A - 0.55 * Right;
    AR = A + 0.55 * Right;
    BU = B + 0.55 * Up;
    BD = B - 0.55 * Up;
    CU = C + 0.55 * Up;
    CD = C - 0.55 * Up;
    DL = D - 0.55 * Right;
    DR = D + 0.55 * Right;
    EU = E + 0.55 * Up;
    ED = E - 0.55 * Up;
    FU = F + 0.55 * Up;
    FD = F - 0.55 * Up;
    GL = G - 0.55 * Right;
    GR = G + 0.55 * Right;

//    float upd = dot(CorrectedPos,Up);

//    AL.x += 0.1*(dot(AL,Up) - upd);
//    AR.x += 0.1*(dot(AR,Up) - dot(CorrectedPos,Up));
    //AR += (AR * Right - CorrectedPos * Right) * Right;

//    AL.x += 0.1*(dot(AL,Up)-upd);
//    AR.x += 0.1*(dot(AR,Up)-upd);
//    BU.x += 0.1*(dot(BU,Up)-upd);
//    BD.x += 0.1*(dot(BD,Up)-upd);
//    CU.x += 0.1*(dot(CU,Up)-upd);
//    CD.x += 0.1*(dot(CD,Up)-upd);
//    DL.x += 0.1*(dot(DL,Up)-upd);
//    DR.x += 0.1*(dot(DR,Up)-upd);
//    EU.x += 0.1*(dot(EU,Up)-upd);
//    ED.x += 0.1*(dot(ED,Up)-upd);
//    FU.x += 0.1*(dot(FU,Up)-upd);
//    FD.x += 0.1*(dot(FD,Up)-upd);
//    GL.x += 0.1*(dot(GL,Up)-upd);
//    GR.x += 0.1*(dot(GR,Up)-upd);


    Flags = InstanceInFlags;

    ScreenPos = (Pos+UOffset)/UZoom;

    Color = InstanceInColor;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

