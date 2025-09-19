//Assets.geom

#version 420 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 UOffset;
uniform vec2 UZoom;

in uint  VSIndex[1];
in uint  VSID[1];
in uint  VSTransform[1];
in ivec2 VSIPoint1[1];
in ivec2 VSIPoint2[1];
in vec4  VSColorA[1];
in vec2  VSFPoint1[1];
in vec2  VSFPoint2[1];

flat out uint Index;
flat out uint ID;
flat out int  I1;
flat out vec2 FPoint1;
flat out vec2 FPoint2;
flat out vec2 FPoint;
flat out vec4 ColorA;
out vec2 Pos;
out vec2 TextureCoord;

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

vec4 getRect(uint Index) {
    switch (Index) {
        case 0://Box
        return vec4(VSIPoint1[0], VSIPoint2[0]);
        case 1://And
        case 2://Or
        case 3://Xor
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2)));
        case 4://Seven
        case 5://Sixteen
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2, 3)));
        case 6://Mux
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(2)));
        case 7://InpuptPin
        case 8://outputPin
        case 9://InpuptRoundPin
        case 10://outputRoundPin
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(0)));
        case 11://PathEdge
        return vec4(VSIPoint2[0], VSIPoint1[0]);
        case 12://PathIntersectionPoints
        case 13://PathVertex
        return vec4(rectFromPointAndSize(VSIPoint1[0], vec2(0)));
        case 14://AreaSelect
        return vec4(VSFPoint1[0], VSFPoint2[0]);
    }
    return vec4(VSIPoint1[0], VSIPoint2[0]);
}

float[4] getMargins(uint Index) {
    switch (Index) {
        case 0://Box
        case 4://Seven
        case 5://Sixteen
        return float[4](0.0, 0.0, 0.0, 0.0);
        case 1://And
        case 2://Or
        return float[4](0.9, 0.2, 0.9, 0.2);
        case 3://Xor
        return float[4](0.9, 0.2, 1.5, 0.2);
        case 6://Mux
        return float[4](0.2, 0.7, 0.2, 0.7);
        case 7://InputPin
        case 8://OutputPin
        return float[4](-0.1, -0.1, 0.2, -0.1);
        case 9://InputRoundPin
        case 10://OutputRoundPin
        return float[4](-0.1, -0.1, -0.1, -0.1);
        case 11://PathEdge
        {
            float v = float(VSIPoint1[0].x == VSIPoint2[0].x);
            float v1 = mix(-0.0, -0.5, v);
            float v2 = mix(-0.5, -0.0, v);
            return float[4](v1, v2, v1, v2);
        }
        case 12://PathIntersectionPoints
        return float[4](0.0, 0.0, 0.0, 0.0);
        case 13://PathVertex
        return float[4](-0.35, -0.35, -0.35, -0.35);
        case 14://AreaSelect
        return float[4](-0.5, -0.5, -0.5, -0.5);
    }
    return float[4](0.0, 0.0, 0.0, 0.0);
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
    ColorA =  VSColorA[0];
    I1 =      VSIPoint2[0].x;

    VSIPoint1[0].x == VSIPoint2[0].x;

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
    uint Rot = Transform & 0x3;
    uint Flip = Transform >> 2;

    vec4 rect = getRect(Index);
    
    vec2 size = vec2(rect.zw - rect.xy) / 2.0 + 0.5;

    FPoint = abs((rect.zw - rect.xy) / 2.0);

    vec2 base = vec2(rect.xy + rect.zw) / 2.0;

    float diff[4] = getMargins(Index);
    
    for (int i = 0; i < 4; ++i) {
        Pos = (size + vec2(diff[lookup[i].x],diff[lookup[i].y])) * signes[i];
        vec2 Off = (size + vec2(diff[lookup[i].x],diff[lookup[i].y])) * signes[i];
        Off -= vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        Off *= rot(Rot);
        Off *= flip(Flip);
        Off += vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        TextureCoord = ((base + Off + UOffset)/UZoom + 1.0) / 2.0;
        gl_Position = vec4((base + Off + UOffset)/UZoom, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
