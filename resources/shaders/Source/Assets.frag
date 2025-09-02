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

// r g b sdf
vec4 get() {
    switch (Index) {
        case 0://Box
        return vec4(ColorA.rgb, 1.0-10.0*sdBox(Pos, FPoint + 0.3));
        case 1://And
        return vec4(1.0,0.0,0.0, 1.0-10.0*sdBox(Pos, FPoint));
        case 2://Or
        return vec4(0.0,1.0,0.0, 1.0-10.0*sdBox(Pos, FPoint));
        case 3://Xor
        return vec4(0.0,0.0,1.0, 1.0-10.0*sdBox(Pos, FPoint));
    }
}

void main () {
    vec4 col = get();
    FragColor = col;
    if(col.a < 0.0){
        FragColor = vec4(1.0,0.0,0.0,0.5);
    }
    // FragColor = vec4(ColorA.rgb, min(sd, ColorA.a));
    // FragColor = vec4(Pos.xy, -Pos.x, 1.0);
    // FragColor = ColorA;
}
