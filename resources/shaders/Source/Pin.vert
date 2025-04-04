//Pin.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in uint InstanceInId;
layout(location = 2) in ivec2 InstanceInPos;
layout(location = 3) in int InstanceInOrientation;
layout(location = 4) in vec3 InstanceInColor;
layout(location = 5) in vec3 InstanceInHighlightColor;

//Out
out vec2 PosInWorld;
out vec2 ScreenPos;

flat out vec3 Color;
flat out vec2 PosOfPin;
flat out vec2 A;
flat out vec2 B;
flat out vec3 HighlightColor;
flat out uint id;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

// d r u t
float Size[4] = {
	-0.6,0.5,0.5,-0.5,
};

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
//
//float OtherOffset[2*4] = {
//    0.0,-1.0,
//   -1.0,0.0,
//    0.0,1.0,
//    1.0,0.0,
//};

void main() {
	vec2 CorrectedPos = InstanceInPos;

	vec2 Right = vec2(Directions[(InstanceInOrientation+1)%4*2+0],Directions[(InstanceInOrientation+1)%4*2+1]);
	vec2 Up = vec2(Directions[(InstanceInOrientation+0)%4*2+0],Directions[(InstanceInOrientation+0)%4*2+1]);

	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

	PosInWorld = Pos;

	PosOfPin = InstanceInPos;

	vec2 Other = InstanceInPos - 0.6*Up;

	A = vec2(min(InstanceInPos.x,Other.x),min(InstanceInPos.y,Other.y));
	B = vec2(max(InstanceInPos.x,Other.x),max(InstanceInPos.y,Other.y));

	Color = InstanceInColor;

	HighlightColor = InstanceInHighlightColor;

	id = InstanceInId;
	
	vec2 PosOnScreen = (Pos+UOffset)/UZoom;

	ScreenPos = (PosOnScreen+1.0) / 2.0;

	gl_Position = vec4(PosOnScreen, 0.0, 1.0);
}

