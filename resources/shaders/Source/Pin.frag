//Pin.frag

#version 330 core

//In
in vec2 PosInWorld;
in vec2 ScreenPos;

flat in vec2 PosOfPin;
flat in vec2 A;
flat in vec2 B;
flat in vec3 Color;
flat in vec2 OtherUp;
flat in vec2 OtherDown;
flat in vec2 Other;

//Out
out vec4 FragColor;


//uniform
uniform bool UHasWire;
uniform sampler2D UPath;

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
    //float expr = 1.0-10*sdBox(PosInWorld,A,B);
    //FragColor = vec4(Color.rgb,min(expr,0.5));
//    float box = sdBox(PosInWorld,A,B);

//    FragColor = vec4(vec3(box),0.5);

//    FragColor = vec4(Color,sdBox(PosInWorld,A,B));

//    FragColor = vec4(Color,0.5*length(PosInWorld-Other));

    float alpha1 = 1.0-10*step(length(PosInWorld-PosOfPin),0.11)-step(1.0-4.5*sdBox(PosInWorld,A,B),0.0);
    vec4 OldColor = vec4(Color,alpha1);

//    float alpha = length(PosInWorld-PosOfPin);
//    float horizontal = float(A.y == B.y);
    float alpha = 1.0-4.5*sdBox(PosInWorld,A,B)+1.0;
    
    //alpha += 1.0-4.5*min(length(PosInWorld-OtherUp),length(PosInWorld-OtherDown));

    if(alpha > 1.0 && alpha1 != 1.0){
//        FragColor = vec4(1.0,0.0,1.0,1.0);
        if(UHasWire){
            FragColor = vec4(texture(UPath,ScreenPos).rgb,1.0);
            return;
        }
        FragColor = vec4(0.05,0.05,0.05,1.0);
        return;
    }

    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,0)+OldColor.a*OldColor;
//    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,alpha)+OldColor.a*OldColor;

    //FragColor= vec4(1.0,0.0,1.0,1.0-4.5*min(length(PosInWorld-OtherUp),length(PosInWorld-OtherDown)));
    //FragColor= vec4(1.0,0.0,1.0,1.0-4.5*(horizontal*abs(PosInWorld.x-Other.x)+(1-horizontal)*abs(PosInWorld.y-Other.y)));
//    FragColor= vec4(1.0,0.0,1.0,1.0-length(PosInWorld-OtherUp));
    //FragColor = vec4(1.0,1.0,0.0,alpha);
//    FragColor = vec4(Color.rgb,0.8);
//    FragColor = vec4(vec3(length(PosInWorld-PosOfPin)),0.8);
    //FragColor = vec4(1.0,1.0,0.0,1.0);
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
