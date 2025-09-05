//Assets.vert

#version 420 core

layout(location = 0) in int   InIndex;
layout(location = 1) in int   InID;
layout(location = 2) in int   InTransform;
layout(location = 3) in ivec2 InIPoint1;
layout(location = 4) in ivec2 InIPoint2;
layout(location = 5) in vec4  InColorA;
layout(location = 6) in ivec2 InFPoint1;
layout(location = 7) in ivec2 InFPoint2;

out int   VSIndex;
out int   VSID;
out int   VSTransform;
out ivec2 VSIPoint1;
out ivec2 VSIPoint2;
out vec4  VSColorA;
out ivec2 VSFPoint1;
out ivec2 VSFPoint2;

void main() {
    VSIndex =     InIndex;
    VSID =        InID;
    VSTransform = InTransform;
    VSIPoint1 =   InIPoint1;
    VSIPoint2 =   InIPoint2;
    VSColorA =    InColorA;
    VSFPoint1 =   InFPoint1;
    VSFPoint2 =   InFPoint2;
}
