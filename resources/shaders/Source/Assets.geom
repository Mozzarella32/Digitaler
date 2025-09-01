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

void main() {
    ID =      VSID[0];
    Index =   VSIndex[0];
    ivec2 A = VSIPoint1[0];
    ivec2 B = VSIPoint2[0];
    ColorA =  VSColorA[0];
    
    vec2 size = vec2(B - A) / 2.0 + 0.5;

    FPoint = (B - A) / 2.0 + 0.3;

    vec2 base = vec2(A + B) / 2.0;

    vec2 quad[4];
    quad[0] = vec2(size.x,   size.y);
    quad[1] = vec2(size.x,  -size.y);
    quad[2] = vec2(-size.x,  size.y);
    quad[3] = vec2(-size.x, -size.y);

    for (int i = 0; i < 4; ++i) {
        Pos = quad[i];
        gl_Position = vec4((base + quad[i] + UOffset)/UZoom, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
