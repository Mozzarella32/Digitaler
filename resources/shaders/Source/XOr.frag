//Mux.frag

#version 330 core

//In
in vec2 PosInWorld;
in vec2 PosInWorldUnshifted;
in vec2 ScreenPos;
flat in vec3 Color;
flat in int Size;
flat in vec2 A;
flat in vec2 B;
flat in vec2 SelectA;
flat in vec2 SelectB;

//Out
out vec4 FragColor;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    vec2 r = ba*h;
    return abs(pa.x-r.x)+abs(pa.y-r.y);
}

float sdTrapezoid( in vec2 p, in vec2 a, in vec2 b, in float ra, float rb )
{
    float rba  = rb-ra;
    float baba = dot(b-a,b-a);
    float papa = dot(p-a,p-a);
    float paba = dot(p-a,b-a)/baba;
    float x = sqrt( papa - paba*paba*baba );
    float cax = max(0.0,x-((paba<0.5)?ra:rb));
    float cay = abs(paba-0.5)-0.5;
    float k = rba*rba + baba;
    float f = clamp( (rba*(x-ra)+paba*baba)/k, 0.0, 1.0 );
    float cbx = x-ra - f*rba;
    float cby = paba - f;
    float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
    return s*sqrt( min(cax*cax + cay*cay*baba,
                       cbx*cbx + cby*cby*baba) );
}

void main(){

    float alpha = 1.0-2.5*sdTrapezoid(PosInWorld,A,B,1.5,1.0);
    float alphaA = 1.0-5.0*length(PosInWorld-A);
    float alphaB = 1.0-5.0*length(PosInWorld-B);

    if(alpha > 0){
        if(alpha > 0.5){
            float segAlpha = 1.0-5.0*sdSegment(PosInWorld,SelectA,SelectB);
	        FragColor = vec4(vec3(0.1,0.1,0.1)*min(1.0-segAlpha,1.0)+vec3(1.0,0.0,1.0)*segAlpha,1.0);
        }
        else {
	        FragColor = vec4(0.1,0.7,0.1,1.0);
        }
    }
    else {

        FragColor = vec4(1.0,1.0,0.0,max(2.0*alpha+1.2,0.1));
    }
}
