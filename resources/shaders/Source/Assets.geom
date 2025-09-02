//Assets.geom

#version 420 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 UOffset;
uniform vec2 UZoom;

in int   VSIndex[1];
in int   VSID[1];
in int   VSTransform[1];
in ivec2 VSIPoint1[1];
in ivec2 VSIPoint2[1];
in vec4  VSColorA[1];

flat out int   ID;
flat out int   Index;
flat out vec2 FPoint;
flat out vec4 ColorA;

out vec2 Pos;

vec2 signes[4] = vec2[4](
    vec2(1, 1),
    vec2(1, -1),
    vec2(-1, 1),
    vec2(-1, -1)
);

mat2 rot(int i) {
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

mat2 flip(int i) {
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

void main() {
    ID =      VSID[0];
    Index =   VSIndex[0];
    ivec2 A = VSIPoint1[0];
    ivec2 B = VSIPoint2[0];
    ColorA =  VSColorA[0];

    int Transform = VSTransform[0];
    int Rot = Transform & 0x3;
    int Flip = Transform >> 2;
    
    vec2 size = vec2(B - A) / 2.0 + 0.5;

    FPoint = (B - A) / 2.0 + 0.3;

    vec2 base = vec2(A + B) / 2.0;

    for (int i = 0; i < 4; ++i) {
        Pos = size * signes[i];
        vec2 Off = size * signes[i];
        Off -= vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        Off *= rot(Rot);
        Off *= flip(Flip);
        Off += vec2(-size.x, size.y) + vec2(1, -1) / 2.0;
        gl_Position = vec4((base + Off + UOffset)/UZoom, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
