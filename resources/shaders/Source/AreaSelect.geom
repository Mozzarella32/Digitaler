//AreaSelect.geom

#version 420 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 VSA[1];
in vec2 VSB[1];
in vec3 VSColor[1];

out vec2 PosInWorld;
flat out vec3 Color;
flat out vec2 A;
flat out vec2 B;

// Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;

void main() {
    vec2 a = VSA[0];
    vec2 b = VSB[0];
    vec3 color = VSColor[0];

    A = a;
    B = b;
    Color = color;

    vec2 quad[4];
    quad[0] = vec2(a.x - 1.0, a.y - 1.0);
    quad[1] = vec2(a.x - 1.0, b.y + 1.0);
    quad[2] = vec2(b.x + 1.0, a.y - 1.0);
    quad[3] = vec2(b.x + 1.0, b.y + 1.0);

    for (int i = 0; i < 4; ++i) {
        PosInWorld = quad[i];
        gl_Position = vec4((quad[i] + UOffset)/UZoom, 0.0, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
