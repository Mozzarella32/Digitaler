//Path.vert

#version 420 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInFirst;
layout(location = 2) in ivec2 InstanceInSecond;
layout(location = 3) in vec3 InstanceInColor;

//Out
out vec2 PosInWorld;
out vec2 ScreenPos;
flat out vec2 A;
flat out vec2 B;
flat out vec3 PathColor;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

void main() {
    vec2 Pos = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
    + int(InIndex == 1)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
    + int(InIndex == 2)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
    + int(InIndex == 3)*vec2(InstanceInSecond.xy+vec2(0.5));

    PosInWorld = Pos;
//
//    ScreenPos = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
//    + int(InIndex == 1 || InIndex == 3)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
//    + int(InIndex == 2 || InIndex == 4)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
//    + int(InIndex == 5)*vec2(InstanceInSecond.xy+vec2(0.5));
//

    A = InstanceInFirst;
    B = InstanceInSecond;
    
    PathColor = InstanceInColor;

    ScreenPos = (Pos+UOffset)/UZoom;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

