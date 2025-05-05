//HighlightPath.vert

#version 420 core

//In
layout(location = 0) in int InIndex;

//Per Instance
layout(location = 1) in ivec2 InstanceInFirst;
layout(location = 2) in ivec2 InstanceInSecond;
layout(location = 4) in vec3 InstanceInColorHightlight;

//Out
out vec2 PosInWorld;
out vec2 ScreenPos;
flat out vec2 A;
flat out vec2 B;
//flat out vec3 ColorPath;
flat out vec3 ColorHightlight;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

void main() {

    int IsPoint = int(InstanceInFirst == InstanceInSecond);
    int IsVertical = int(InstanceInFirst.x == InstanceInSecond.x);

    vec2 FirstMoved = IsPoint*(InstanceInFirst)+(1.0-IsPoint)*
    (IsVertical*(vec2(InstanceInFirst.x,InstanceInFirst.y+0.5))+
    (1.0-IsVertical)*(vec2(InstanceInFirst.x+0.5,InstanceInFirst.y)));

    vec2 SecondMoved =  IsPoint*(InstanceInSecond)+(1.0-IsPoint)*
    (IsVertical*(vec2(InstanceInSecond.x,InstanceInSecond.y-0.5))+
    (1.0-IsVertical)*(vec2(InstanceInSecond.x-0.5,InstanceInSecond.y)));


    vec2 Pos = int(InIndex == 0)*vec2(FirstMoved.xy - vec2(0.5))
    + int(InIndex == 1)*vec2(FirstMoved.x-0.5,SecondMoved.y+0.5)
    + int(InIndex == 2)*vec2(SecondMoved.x+0.5,FirstMoved.y-0.5)
    + int(InIndex == 3)*vec2(SecondMoved.xy+vec2(0.5));

     PosInWorld = Pos;

//    PosInWorld = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
//    + int(InIndex == 1 || InIndex == 3)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
//    + int(InIndex == 2 || InIndex == 4)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
//    + int(InIndex == 5)*vec2(InstanceInSecond.xy+vec2(0.5));
//
//    ScreenPos = int(InIndex == 0)*vec2(InstanceInFirst.xy - vec2(0.5))
//    + int(InIndex == 1 || InIndex == 3)*vec2(InstanceInFirst.x-0.5,InstanceInSecond.y+0.5)
//    + int(InIndex == 2 || InIndex == 4)*vec2(InstanceInSecond.x+0.5,InstanceInFirst.y-0.5)
//    + int(InIndex == 5)*vec2(InstanceInSecond.xy+vec2(0.5));
//

    A = InstanceInFirst;
    B = InstanceInSecond;
    
    ColorHightlight = InstanceInColorHightlight;

    vec2 PosOnScreen = (Pos+UOffset)/UZoom;

    ScreenPos = (PosOnScreen+1.0) / 2.0;

    gl_Position = vec4(PosOnScreen, 0.0, 1.0);
}

