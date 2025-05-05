//IntersectionPath.frag

#version 420 core

//In
in vec2 PosInWorld;

//Out
out vec4 FragColor;

flat in vec2 A;
flat in vec3 ColorPath;

//Uniforms
//uniform sampler2D UBackground;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}


//float sdCross(vec2 p, vec2 a){
//	return abs(p.x-a.x)+abs(p.y-a.y);
//}


float sdCircle(vec2 p, vec2 a){
	return length(a-p);
}


void main(){
//    //vec4 backgroundColour = texture(UBackground,ScreenPos);
//    float blend = clamp(1.0-0.75*sqrt(2)*(sdCross(PosInWorld,A)),0.0,1.0);

    if(distance(PosInWorld,A) < 0.05){
        FragColor = vec4(0.0,1.0,1.0,1.0);
        return;
    }

    float blend = clamp(-0.5+3*min(min(sdCircle(PosInWorld,A+vec2(0.5,0.5)),
    sdCircle(PosInWorld,A+vec2(-0.5,0.5))),
    min(sdCircle(PosInWorld,A+vec2(0.5,-0.5)),
    sdCircle(PosInWorld,A+vec2(-0.5,-0.5)))),0.0,1.0);

    //float blend = clamp(sdCircle(PosInWorld,A),0.0,1.0);
	//FragColor = vec4(blend*vec3(1.0,1.0,0.0)+(1.0-blend)*backgroundColour.rgb,1.0);
    if(blend > 0.67059){
//        FragColor = vec4(0.941,0.286,0.351,1.0);
        FragColor = vec4(ColorPath,1.0);
//        FragColor = vec4(0.941,0.286,0.351,blend);
        return;
    }
    FragColor = vec4(0.0);
}

//	FragColor = vec4((1.0-10*sdSegment(ScreenPos,A,B))*vec3(1.0),1.0);
