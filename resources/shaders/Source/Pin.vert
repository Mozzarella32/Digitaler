//Pin.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in int InstanceOrientation;
layout(location = 3) in vec3 InstanceInColor;

//Out
out vec2 PosInWorld;
out vec2 ScreenPos;

flat out vec3 Color;
flat out vec2 PosOfPin;
flat out vec2 A;
flat out vec2 B;
flat out vec2 OtherUp;
flat out vec2 OtherDown;
flat out vec2 Other;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;
// lx dy rx uy
float Offset[4*4] = {
    0.5,-0.6,-0.5,0.5,
    -0.5,-0.5,0.6,0.5,
    -0.5,0.6,0.5,-0.5,
    0.5,0.5,-0.6,-0.5,
};

float OtherOffset[2*4] = {
    0.0,-1.0,
    1.0,0.0,
    0.0,1.0,
   -1.0,0.0,
};


void main() {
 vec2 Pos = 
      int(InIndex == 0)*vec2(InstanceInPos.x+Offset[InstanceOrientation*4+0],InstanceInPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 1)*vec2(InstanceInPos.x+Offset[InstanceOrientation*4+0],InstanceInPos.y+Offset[InstanceOrientation*4+3])
    + int(InIndex == 2)*vec2(InstanceInPos.x+Offset[InstanceOrientation*4+2],InstanceInPos.y+Offset[InstanceOrientation*4+1])
    + int(InIndex == 3)*vec2(InstanceInPos.x+Offset[InstanceOrientation*4+2],InstanceInPos.y+Offset[InstanceOrientation*4+3]);

    PosInWorld = Pos;

//
//    ScreenPos = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
//    + int(InIndex == 1 || InIndex == 3)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
//    + int(InIndex == 2 || InIndex == 4)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
//    + int(InIndex == 5)*vec2(InstanceInSecond.xy+vec2(0.5));
//

    PosOfPin = InstanceInPos;

    Other = InstanceInPos + 0.6*vec2(OtherOffset[InstanceOrientation*2+0],OtherOffset[InstanceOrientation*2+1]);

    vec2 OtherLess = InstanceInPos + 0.4*vec2(OtherOffset[InstanceOrientation*2+0],OtherOffset[InstanceOrientation*2+1]);

    OtherUp = OtherLess + 0.5*vec2(OtherOffset[(InstanceOrientation+1)%4*2+0],OtherOffset[(InstanceOrientation+1)%4*2+1]);
    OtherDown = OtherLess + 0.5*vec2(OtherOffset[(InstanceOrientation-1)%4*2+0],OtherOffset[(InstanceOrientation-1)%4*2+1]);

    A = vec2(min(InstanceInPos.x,Other.x),min(InstanceInPos.y,Other.y));
    B = vec2(max(InstanceInPos.x,Other.x),max(InstanceInPos.y,Other.y));

   vec2 PosOnScreen = (Pos+UOffset)/UZoom;

    ScreenPos = (PosOnScreen+1.0) / 2.0;

    Color = InstanceInColor;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

