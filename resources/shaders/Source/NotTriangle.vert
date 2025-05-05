//Or.vert

#version 420 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in int InstanceInOrientation;

//Out
out vec2 PosInWorld;
//out vec2 ScreenPos;
//flat out vec2 A;
//flat out vec2 B;
//flat out vec2 SelectA;
//flat out vec2 SelectB;
flat out vec3 Color;
//flat out vec2 Pos;
flat out vec2 Base1;
flat out vec2 Base2;
flat out vec2 BaseFar;
flat out vec2 ControllNear;
flat out vec2 Controll1;
flat out vec2 Controll2;
flat out vec2 BaseTriangle;
flat out int Orientation;

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

	vec2 Right = vec2(Directions[(InstanceInOrientation+1)%4*2+0],Directions[(InstanceInOrientation+1)%4*2+1]);
	vec2 Up = vec2(Directions[(InstanceInOrientation+0)%4*2+0],Directions[(InstanceInOrientation+0)%4*2+1]);

	ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

	vec2 Pos = CorrectedPos + Size[Index.x] * Up + Size[Index.y] * Right;

	PosInWorld = Pos;

	BaseTriangle = CorrectedPos + 1.6*Up;

	Base1 = CorrectedPos - 2*Up - 1.5*Right;
	Base2 = CorrectedPos - 2*Up + 1.5*Right;

	BaseFar = CorrectedPos + 1.9*Up;
	ControllNear = CorrectedPos - 1.5*Up;

	Controll1 = CorrectedPos + 1.5*Up - 1.3*Right;
	Controll2 = CorrectedPos + 1.5*Up + 1.3*Right;

	Orientation = InstanceInOrientation;

	gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

