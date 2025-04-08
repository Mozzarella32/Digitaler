//Path.frag

#version 330 core

//In
in vec2 PosInWorld;

//Out
out vec4 FragColor;

flat in vec2 A;
flat in vec2 B;

//flat in int Flags;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length(pa-ba*h);
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
    if(min(distance(PosInWorld,A),distance(PosInWorld,B)) < 0.05){
        FragColor = vec4(0.0,1.0,1.0,1.0);
        return;
    }
	FragColor = vec4(0.941,0.286,0.351,step(-0.1,1.0-10*sdSegment(PosInWorld,A,B)));
//    FragColor = vec4(0.0);
//	FragColor = vec4(0.0,0.0,1.0,1.0-3*sdSegment(PosInWorld,A,B));
//	FragColor = vec4(0.0,0.0,1.0,step(-0.1,1.0-10*sdSegment(PosInWorld,A,B)));
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);

//float segDist = sdSegment(ScreenPos,A,B);
//    float dist = 1.0-10*segDist;
//    float cuttoff = step(-0.1,dist);
//	FragColor = vec4(0.941,0.286,0.351,cuttoff);
//    if(Flags % 2 == 1 && cuttoff == 0.0){
//	    FragColor = vec4(1.0,1.0,0.0,1.0-3.0*segDist);
//    }
