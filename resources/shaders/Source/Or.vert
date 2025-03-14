//Or.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstancePos;
layout(location = 2) in int InstanceOrientation;
layout(location = 3) in vec3 InstanceColor;

//Out
out vec2 PosInWorld;
//out vec2 ScreenPos;
//flat out vec2 A;
//flat out vec2 B;
//flat out vec2 SelectA;
//flat out vec2 SelectB;
flat out vec3 Color;
//flat out vec2 Pos;
flat out vec2 Corner1;
flat out vec2 Corner2;
flat out vec2 FarPoint;
flat out vec2 NearPoint;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;
// lx dy rx uy
float Offset[4*4] = {
    2.0,2.5,-2.0,-2.5,
    2.5,-2.0,-2.5,2.0,
    -2.0,-2.5,2.0,2.5,
    -2.5,2.0,2.5,-2.0,
};

// x,y,-x,-y
float Directions[2*4] = {
    1.0,0.0,
    0.0,1.0,
    -1.0,0.0,
    0.0,-1.0,
};

float PosOffset[2*4] = {
    1.0,-1.0,
    1.0,-1.0,
    1.0,-1.0,
    2.0,-1.0,
};

void main() {
vec2 CorrectedPos = InstancePos + vec2(PosOffset[InstanceOrientation*2+0],PosOffset[InstanceOrientation*2+1]);

 vec2 Pos = 
      int(InIndex == 0)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+0],CorrectedPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 1)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+0],CorrectedPos.y+Offset[InstanceOrientation*4+3])
    + int(InIndex == 2)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+2],CorrectedPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 3)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+2],CorrectedPos.y+Offset[InstanceOrientation*4+3]);

    vec2 Right = vec2(Directions[(InstanceOrientation+0)%4*2+0],Directions[(InstanceOrientation+0)%4*2+1]);
    vec2 Up = vec2(Directions[(InstanceOrientation+1)%4*2+0],Directions[(InstanceOrientation+1)%4*2+1]);

    PosInWorld = Pos;

    Corner1 = CorrectedPos - 2*Up - 1.5*Right;
    Corner2 = CorrectedPos - 2*Up + 1.5*Right;

//    Midpoint = CorrectedPos+0.5*Up;
    FarPoint = CorrectedPos + 6*Up;
    NearPoint = CorrectedPos - 1.5*Up;

//    ScreenPos = (Pos+UOffset)/UZoom;

    Color = InstanceColor;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

