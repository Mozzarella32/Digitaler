//HighlightPathOverdraw.frag

#version 330 core

//In
in vec2 PosInWorld;
in vec2 ScreenPos;
flat in vec2 A;
flat in vec2 B;
flat in vec3 Color;
flat in vec3 ColorHightlight;

//Out
out vec4 FragColor;


//Uniforms
uniform sampler2D UBackground;

//uniform vec3 UColor;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}


float sdCross(vec2 p, vec2 a){
	return abs(p.x-a.x)+abs(p.y-a.y);
}


float sdCircle(vec2 p, vec2 a){
	return length(a-p);
}


void main(){
    vec4 BackgroundColor = texture(UBackground,ScreenPos);
//    float blend = clamp(1.0-0.75*sqrt(2)*(sdCross(PosInWorld,A)),0.0,1.0);

    float blend = clamp(-0.6+3.0*min(min(sdCircle(PosInWorld,A+vec2(0.5,0.5)),
    sdCircle(PosInWorld,A+vec2(-0.5,0.5))),
    min(sdCircle(PosInWorld,A+vec2(0.5,-0.5)),
    sdCircle(PosInWorld,A+vec2(-0.5,-0.5)))),0.0,1.0);

    //float blend = clamp(sdCircle(PosInWorld,A),0.0,1.0);
	//FragColor = vec4(blend*vec3(1.0,1.0,0.0)+(1.0-blend)*backgroundColour.rgb,1.0);
//    FragColor = vec4(blend*UColor+(1.0-blend)*BackgroundColor.rgb,1.0);
//	vec3 BackgroundColor = vec3(0.05,0.05,0.05);

    FragColor = vec4(2*blend*ColorHightlight+(1.0-2*blend)*BackgroundColor.rgb,1.0);
//    if(blend > 1.0){
//        FragColor = vec4(1.0,0.0,1.0,1.0);
//        return;
//    }
//    FragColor = vec4(UColor,blend);
//    FragColor = vec4(0.0);
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);
