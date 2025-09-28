//Assets.geom

#version 420 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2  UOffset;
uniform vec2  UZoom;
uniform float UZoomFactor;

in uint  VSIndex[1];
in uint  VSID[1];
in uint  VSTransform[1];
in ivec2 VSIPoint1[1];
in ivec2 VSIPoint2[1];
in vec4  VSColorA1[1];
in vec2  VSFPoint1[1];
in vec2  VSFPoint2[1];
in vec4  VSColorA2[1];
in vec4  VSOffsets[1];
in vec4  VSUVs[1];

flat out uint Index;
flat out uint ID;
flat out int  I1;
flat out vec2 FPoint1;
flat out vec2 FPoint2;
flat out vec2 FPoint;
flat out vec4 ColorA1;
flat out vec4 ColorA2;
out vec2 Pos;
out vec2 TextureCoord;
out vec2 ScreenPos;

vec2 signes[4] = vec2[4](
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(-1, -1)
);

mat2 rot(uint i) {
    if(i == 0) {
        return mat2(1, 0, 0, 1);
    }
    else if(i == 1) {
        return mat2(0, 1, -1, 0);
    }
    else if(i == 2) {
        return mat2(-1, 0, 0, -1);
    }
    else { // n == 3
        return mat2(0, -1, 1, 0);
    }
}

mat2 flip(uint i) {
    if(i == 0) {
        return mat2(1, 0, 0, 1);
    }
    else if(i == 1) {
        return mat2(-1, 0, 0, 1);
    }
    else if(i == 2) {
        return mat2(1, 0, 0, -1);
    }
    else { // n == 3
        return mat2(-1, 0, 0, -1);
    }
}

vec4 rectFromPointAndSize(vec2 pos, vec2 size) {
    return vec4(pos - vec2(0, size.y), pos + vec2(size.x, 0));
}

#define Box                   0
#define And                   Box                   + 1
#define Or                    And                   + 1
#define XOr                   Or                    + 1
#define Seven                 XOr                   + 1
#define Sixteen               Seven                 + 1
#define Mux                   Sixteen               + 1
#define Driver                Mux                   + 1
#define InputPin              Driver                + 1
#define OutputPin             InputPin              + 1
#define InputRoundPin         OutputPin             + 1
#define OutputRoundPin        InputRoundPin         + 1
#define PathEdge              OutputRoundPin        + 1
#define PathIntersectionPoint PathEdge              + 1
#define PathVertex            PathIntersectionPoint + 1
#define Text                  PathVertex            + 1
#define AreaSelect            Text                  + 1

vec4 getRect(uint Index) {
    switch (Index) {
        case Box:
        return vec4(VSIPoint1[0], VSIPoint2[0]);
        case And:
        case Or:
        case XOr:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2)));
        case Seven:
        case Sixteen:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2, 3)));
        case Mux:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2)));
        case Driver:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(0)));
        case InputPin:
        case OutputPin:
        case InputRoundPin:
        case OutputRoundPin:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(0)));
        case PathEdge:
        return vec4(VSIPoint2[0], VSIPoint1[0]);
        case PathIntersectionPoint:
        case PathVertex:
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(0)));
        case Text:
        return VSOffsets[0];
        case AreaSelect:
        return vec4(VSFPoint1[0], VSFPoint2[0]);
    }
    return vec4(VSIPoint1[0], VSIPoint2[0]);
}

float[4] getMargins(uint Index) {
    switch (Index) {
        case Box:
        case Seven:
        case Sixteen:
        return float[4](0.0, 0.0, 0.0, 0.0);
        case And:
        case Or:
        return float[4](0.9, 0.2, 0.9, 0.2);
        case XOr:
        return float[4](0.9, 0.2, 1.5, 0.2);
        case Mux:
        return float[4](0.2, 0.7, 0.2, 0.7);
        case Driver:
        return float[4](0.7, 0.3, 0.7, 0.3);
        case InputPin:
        case OutputPin:
        return float[4](-0.1, -0.1, 0.2, -0.1);
        case InputRoundPin:
        case OutputRoundPin:
        return float[4](-0.1, -0.1, -0.1, -0.1);
        case PathEdge:
        {
            float v = float(VSIPoint1[0].x == VSIPoint2[0].x);
            float v1 = mix(-0.0, -0.5, v);
            float v2 = mix(-0.5, -0.0, v);
            return float[4](v1, v2, v1, v2);
        }
        case PathIntersectionPoint:
        return float[4](0.0, 0.0, 0.0, 0.0);
        case PathVertex:
        // return float[4](-0.35, -0.35, -0.35, -0.35);
        return float[4](0.0, 0.0, 0.0, 0.0);
        case Text:
        return float[4](-0.5, -0.5, -0.5, -0.5);
        case AreaSelect:
        return float[4](-0.5, -0.5, -0.5, -0.5);
    }
    return float[4](0.0, 0.0, 0.0, 0.0);
}

float doMarginExtend() {
    switch (Index) {
        case PathEdge:
        case PathIntersectionPoint:
        case PathVertex:
        return 0.0;
        default:
        return 1.0;
    }
}

ivec2 lookup[4] = ivec2[4](
    ivec2(1,0),        
    ivec2(1,2),        
    ivec2(3,0),        
    ivec2(3,2)        
);

void main() {
    ID =      VSID[0];
    Index =   VSIndex[0];
    ColorA1 = VSColorA1[0];
    ColorA2 = VSColorA2[0];
    I1 =      VSIPoint2[0].x;

    FPoint1 =
        vec2(0, (VSIPoint1[0].y - VSIPoint2[0].y) / 2.0) *
        float(VSIPoint1[0].x == VSIPoint2[0].x) +
        vec2((VSIPoint1[0].x - VSIPoint2[0].x) / 2.0, 0) *
        float(VSIPoint1[0].y == VSIPoint2[0].y);
    FPoint2 =
        vec2(0, (VSIPoint2[0].y - VSIPoint1[0].y) / 2.0) *
        float(VSIPoint1[0].x == VSIPoint2[0].x) +
        vec2((VSIPoint2[0].x - VSIPoint1[0].x) / 2.0, 0) *
        float(VSIPoint1[0].y == VSIPoint2[0].y);

    uint Transform = VSTransform[0];
    uint Rot = Transform & uint(0x3);
    uint Flip = Transform >> 2;

    vec4 rect = getRect(Index);
    
    vec2 size = vec2(rect.zw - rect.xy) / 2.0 + 0.5;

    FPoint = abs((rect.zw - rect.xy) / 2.0);

    vec2 base = vec2(rect.xy + rect.zw) / 2.0;

    float diff[4] = getMargins(Index);

    float marginBlurExtend = 20.0 * min(UZoomFactor, 0.05) * doMarginExtend();
    diff[0] += marginBlurExtend;
    diff[1] += marginBlurExtend;
    diff[2] += marginBlurExtend;
    diff[3] += marginBlurExtend;

    vec2 Up = vec2(0.0, 1.0) * rot(Rot);
    vec2 Right = vec2(1.0, 0.0) * rot(Rot);
    float UVs[4] = float[4](VSUVs[0].x, VSUVs[0].y, VSUVs[0].z, VSUVs[0].w);

    if(Index == Text &&  UZoomFactor / VSFPoint2[0].x > 0.05) {
        return;
    }
    
    for (int i = 0; i < 4; ++i) {
        Pos = (size + vec2(diff[lookup[i].x],diff[lookup[i].y])) * signes[i];
        vec2 Off = (size + vec2(diff[lookup[i].x],diff[lookup[i].y])) * signes[i];
        Off -= vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        Off *= rot(Rot);
        Off *= flip(Flip);
        Off += vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        if (Index == Text) {
            TextureCoord = vec2(UVs[lookup[i].x], UVs[lookup[i].y]);        
            Off = VSFPoint1[0] + rect[lookup[i].x] * Right + rect[lookup[i].y] * Up;
            base = vec2(0);
        }
        else {
            TextureCoord = ((base + Off + UOffset)/UZoom + 1.0) / 2.0;
        }
        ScreenPos = base + Off;
        gl_Position = vec4((base + Off + UOffset)/UZoom, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
