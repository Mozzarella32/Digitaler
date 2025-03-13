//SevenSeg.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in int InstanceOrientation;
layout(location = 3) in int InstanceFlags;
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
// lx dy rx uy
float Offset[4*4] = {
    1.5,1.5,-1.5,-2.5,
    2.5,-1.5,-1.5,1.5,
    -1.5,-1.5,1.5,2.5,
    -2.5,1.5,1.5,-1.5,
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
    1.0,-2.0,
    2.0,-1.0,
};

void main() {
vec2 CorrectedPos = InstanceInPos + vec2(PosOffset[InstanceOrientation*2+0],PosOffset[InstanceOrientation*2+1]);

 vec2 Pos = 
      int(InIndex == 0)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+0],CorrectedPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 1)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+0],CorrectedPos.y+Offset[InstanceOrientation*4+3])
    + int(InIndex == 2)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+2],CorrectedPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 3)*vec2(CorrectedPos.x+Offset[InstanceOrientation*4+2],CorrectedPos.y+Offset[InstanceOrientation*4+3]);


//    ScreenPos = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
//    + int(InIndex == 1 || InIndex == 3)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
//    + int(InIndex == 2 || InIndex == 4)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
//    + int(InIndex == 5)*vec2(InstanceInSecond.xy+vec2(0.5));
//

    vec2 Right = vec2(Directions[(InstanceOrientation+0)%4*2+0],Directions[(InstanceOrientation+0)%4*2+1]);
    vec2 Up = vec2(Directions[(InstanceOrientation+1)%4*2+0],Directions[(InstanceOrientation+1)%4*2+1]);
//    vec2 Left = vec2(Directions[(InstanceOrientation+2)%4*2+0],Directions[(InstanceOrientation+2)%4*2+1]);
//    vec2 Down = vec2(Directions[(InstanceOrientation+3)%4*2+0],Directions[(InstanceOrientation+3)%4*2+1]);
//

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


    Flags = InstanceFlags;

    ScreenPos = (Pos+UOffset)/UZoom;

    Color = InstanceInColor;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

