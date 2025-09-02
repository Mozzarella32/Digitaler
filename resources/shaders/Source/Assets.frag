//Assets.frag

#version 420 core

flat in int  ID;
flat in int  Index;
flat in vec2 FPoint;
flat in vec4 ColorA;

in vec2 Pos;

out vec4 FragColor;

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
void main () {
    float sd = 1.0-10.0*sdBox(Pos, vec2(FPoint));
    // FragColor = vec4(ColorA.rgb, min(sd, ColorA.a));
    FragColor = vec4(Pos.xy, -Pos.x, 1.0);
}
