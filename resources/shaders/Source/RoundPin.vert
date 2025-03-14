//RoundPin.vert

#version 330 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInPos;
layout(location = 2) in vec3 InstanceInColor;

//Out
out vec2 PosInWorld;
out vec2 ScreenPos;
flat out vec2 PosOfPin;
flat out vec3 Color;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;


void main() {
  vec2 Pos = 
      int(InIndex == 0)*vec2(InstanceInPos.x+0.5,InstanceInPos.y+0.5)
    + int(InIndex == 1)*vec2(InstanceInPos.x+0.5,InstanceInPos.y-0.5)
    + int(InIndex == 2)*vec2(InstanceInPos.x-0.5,InstanceInPos.y+0.5)
    + int(InIndex == 3)*vec2(InstanceInPos.x-0.5,InstanceInPos.y-0.5);


    PosInWorld = Pos;

    PosOfPin = InstanceInPos;

    Color = InstanceInColor;

   vec2 PosOnScreen = (Pos+UOffset)/UZoom;

    ScreenPos = (PosOnScreen+1.0) / 2.0;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

