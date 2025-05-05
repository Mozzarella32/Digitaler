//NDot.vert

#version 420 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in int InstanceInOrientation;

//Out
out vec2 PosInWorld;
flat out vec2 PosOfPoint;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

// d r u t
float Size[4] = {
	0.0,0.7,1.2,-0.7
//	-5,5,5,-5
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

void main() {
	vec2 CorrectedPos = InstanceInPos;

	vec2 Right = vec2(Directions[(InstanceInOrientation+1)%4*2+0],Directions[(InstanceInOrientation+1)%4*2+1]);
	vec2 Up = vec2(Directions[(InstanceInOrientation+0)%4*2+0],Directions[(InstanceInOrientation+0)%4*2+1]);


	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

//	Pos += 0.5*Up;

	PosInWorld = Pos;

	PosOfPoint = InstanceInPos + 0.5*Up;

	gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}
