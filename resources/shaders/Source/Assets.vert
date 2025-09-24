//Assets.vert

#version 420 core

layout(location = 0)  in uint  InIndex;
layout(location = 1)  in uint  InID;
layout(location = 2)  in uint  InTransform;
layout(location = 3)  in ivec2 InIPoint1;
layout(location = 4)  in ivec2 InIPoint2;
layout(location = 5)  in vec4  InColorA1;
layout(location = 6)  in vec2  InFPoint1;
layout(location = 7)  in vec2  InFPoint2;
layout(location = 8)  in vec4  InColorA2;
layout(location = 9)  in vec4  InOffsets;
layout(location = 10) in vec4  InUVs;

out uint  VSIndex;
out uint  VSID;
out uint  VSTransform;
out ivec2 VSIPoint1;
out ivec2 VSIPoint2;
out vec4  VSColorA1;
out vec2  VSFPoint1;
out vec2  VSFPoint2;
out vec4  VSColorA2;
out vec4  VSOffsets;
out vec4  VSUVs;

void main() {
    VSIndex =     InIndex;
    VSID =        InID;
    VSTransform = InTransform;
    VSIPoint1 =   InIPoint1;
    VSIPoint2 =   InIPoint2;
    VSColorA1 =   InColorA1;
    VSFPoint1 =   InFPoint1;
    VSFPoint2 =   InFPoint2;
    VSColorA2 =   InColorA2;
    VSOffsets =   InOffsets;
    VSUVs =       InUVs;
}
