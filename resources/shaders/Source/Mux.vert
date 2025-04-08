//Mux.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in uint InstanceInId;
layout(location = 2) in ivec2 InstanceInPos;
layout(location = 3) in int InstanceInTransform;
layout(location = 4) in int InstanceInSize;
layout(location = 5) in int InstanceInSelected;
layout(location = 6) in vec3 InstanceInColorHighlight;

//Out
out vec2 PosInWorld;
//out vec2 ScreenPos;
flat out vec2 A;
flat out vec2 B;
flat out vec2 SelectA;
flat out vec2 SelectB;
flat out vec2 Pos;
flat out vec3 ColorHighlight;
flat out uint id;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

// d r u t
float Size[4] = {
	-1.7,2.2,1.7,-2.2,
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

void main() {

	vec2 CorrectedPos = InstanceInPos + Offset;

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

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

	A = CorrectedPos - 1.0 * Up;
	B = CorrectedPos + 1.0 * Up;

	SelectA = CorrectedPos - 1.2 * Up + 1.0 * Right;
	SelectB = CorrectedPos + 1.2 * Up ;

	PosInWorld = Pos;

	ColorHighlight = InstanceInColorHighlight;

	id = InstanceInId;

	gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

