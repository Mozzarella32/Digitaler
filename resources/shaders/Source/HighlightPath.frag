//HighlitePath.frag

#version 330 core

//In
in vec2 PosInWorld;
//in vec2 ScreenPos;
flat in vec2 A;
flat in vec2 B;
flat in vec3 ColorHightlight;

//Out
out vec4 FragColor;

//Uniforms
//uniform sampler2D UBackground;

//uniform vec3 UColor;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
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
//    vec4 backgroundColour = texture(UBackground,ScreenPos);
//    vec4 backgroundColour = vec4(ScreenPos,0.0,1.0);
    float blend = clamp(0.9-3.0*sdSegment(PosInWorld,A,B),0.0,1.0);
//	FragColor = vec4(blend*UColor+(1.0-blend)*backgroundColour.rgb,1.0);
//    FragColor = backgroundColour;
//    if(blend >= 1.0){
//        FragColor = vec4(1.0,0.0,1.0,1.0);
//        return;
//    }
    FragColor = vec4(ColorHightlight,2*blend);
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);


