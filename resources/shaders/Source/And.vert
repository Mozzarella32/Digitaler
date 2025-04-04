//And.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in uint InstanceInId;
layout(location = 2) in ivec2 InstanceInPos;
layout(location = 3) in int InstanceInTransform;
layout(location = 4) in vec3 InstanceInHighlightColor;

//Out
out vec2 PosInWorld;
flat out vec2 Corner1;
flat out vec2 Corner2;
flat out vec2 Midpoint;
flat out vec3 HighlightColor;
flat out uint id;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

// d r u t
float Size[4] = {
	-3.0,2.0,2.5,-2.0,
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
	vec2 flipx = vec2(1 - 2 * xflip);
	vec2 flipy = vec2(1 - 2 * yflip);

	vec2 Right = vec2(Directions[(Rotation+1)%4*2+0],Directions[(Rotation+1)%4*2+1]) * flipx;
	vec2 Up = vec2(Directions[(Rotation+0)%4*2+0],Directions[(Rotation+0)%4*2+1]) * flipy;


	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

	PosInWorld = Pos;

	vec2 TempCorner1 = CorrectedPos - 1.8 * Up - 1.3125 * Right;
	vec2 TempCorner2 = CorrectedPos + 0.5 * Up + 1.3125 * Right;
	Corner1 = vec2(min(TempCorner1.x,TempCorner2.x),min(TempCorner1.y,TempCorner2.y));
	Corner2 = vec2(max(TempCorner1.x,TempCorner2.x),max(TempCorner1.y,TempCorner2.y));

	Midpoint = CorrectedPos+0.5*Up;

	HighlightColor= InstanceInHighlightColor;

	id = InstanceInId;

	gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

