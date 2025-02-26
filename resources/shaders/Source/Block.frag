//Block.frag

#version 330 core

//In
in vec2 PosInWorld;

//Out
out vec4 FragColor;

flat in vec2 A;
flat in vec2 B;
flat in vec4 Color;

//flat in int Flags;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}


//
//float sdBox( in vec2 p, in vec2 b )
//{
//    vec2 d = abs(p)-b;
//    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
//}
//
//
//float sdBoxAB( in vec2 p, in vec2 a, in vec2 b)
//{
//   return sdBox(p+a,a-b),
//}

void main(){
    float expr = 1.0-10*sdBox(PosInWorld,A,B);
    FragColor = vec4(Color.rgb,min(expr,0.5));
//    FragColor = vec4(PosInWorld.rg,0,0.5);
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);

//float segDist = sdSegment(ScreenPos,A,B);
//    float dist = 1.0-10*segDist;
//    float cuttoff = step(-0.1,dist);
//	FragColor = vec4(0.941,0.286,0.351,cuttoff);
//    if(Flags % 2 == 1 && cuttoff == 0.0){
//	    FragColor = vec4(1.0,1.0,0.0,1.0-3.0*segDist);
//    }
