//Or.frag

#version 330 core

//In
in vec2 PosInWorld;
flat in vec3 Color;
flat in vec2 Corner1;
flat in vec2 Corner2;
flat in vec2 FarPoint;
flat in vec2 NearPoint;
//Out
out vec4 FragColor;

//float sdSegment( in vec2 p, in vec2 a, in vec2 b )
//{
//    vec2 pa = p-a, ba = b-a;
//    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
//    vec2 r = ba*h;
//    return abs(pa.x-r.x)+abs(pa.y-r.y);
//}
//
//float sdTrapezoid( in vec2 p, in vec2 a, in vec2 b, in float ra, float rb )
//{
//    float rba  = rb-ra;
//    float baba = dot(b-a,b-a);
//    float papa = dot(p-a,p-a);
//    float paba = dot(p-a,b-a)/baba;
//    float x = sqrt( papa - paba*paba*baba );
//    float cax = max(0.0,x-((paba<0.5)?ra:rb));
//    float cay = abs(paba-0.5)-0.5;
//    float k = rba*rba + baba;
//    float f = clamp( (rba*(x-ra)+paba*baba)/k, 0.0, 1.0 );
//    float cbx = x-ra - f*rba;
//    float cby = paba - f;
//    float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
//    return s*sqrt( min(cax*cax + cay*cay*baba,
//                       cbx*cbx + cby*cby*baba) );
//}
//

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}
//
//float sdParabola( in vec2 pos, in float k )
//{
//    pos.x = abs(pos.x);
//    
//    float ik = 1.0/k;
//    float p = ik*(pos.y - 0.5*ik)/3.0;
//    float q = 0.25*ik*ik*pos.x;
//    
//    float h = q*q - p*p*p;
//    float r = sqrt(abs(h));
//
//    float x = (h>0.0) ? 
//        // 1 root
//        pow(q+r,1.0/3.0) + pow(abs(q-r),1.0/3.0)*sign(p) :
//        // 3 roots
//        2.0*cos(atan(r,q)/3.0)*sqrt(p);
//    
//    float d = length(pos-vec2(x,k*x*x));
//    
//    return (pos.x<x)? -d : d;
//}
//
//// The MIT License
//// Copyright © 2018 Inigo Quilez
//// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//// Distance to a quadratic bezier segment, which can be solved analyically with a cubic.
//
//// List of some other 2D distances: https://www.shadertoy.com/playlist/MXdSRf
////
//// and iquilezles.org/articles/distfunctions2d
//
//
//
//float dot2( vec2 v ) { return dot(v,v); }
//float cro( vec2 a, vec2 b ) { return a.x*b.y-a.y*b.x; }
//float cos_acos_3( float x ) { x=sqrt(0.5+0.5*x); return x*(x*(x*(x*-0.008972+0.039071)-0.107074)+0.576975)+0.5; } // https://www.shadertoy.com/view/WltSD7
//
//float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C, out vec2 outQ )
//{    
//    vec2 a = B - A;
//    vec2 b = A - 2.0*B + C;
//    vec2 c = a * 2.0;
//    vec2 d = A - pos;
//
//    // cubic to be solved (kx*=3 and ky*=3)
//    float kk = 1.0/dot(b,b);
//    float kx = kk * dot(a,b);
//    float ky = kk * (2.0*dot(a,a)+dot(d,b))/3.0;
//    float kz = kk * dot(d,a);      
//
//    float res = 0.0;
//    float sgn = 0.0;
//
//    float p  = ky - kx*kx;
//    float q  = kx*(2.0*kx*kx - 3.0*ky) + kz;
//    float p3 = p*p*p;
//    float q2 = q*q;
//    float h  = q2 + 4.0*p3;
//
//
//    if( h>=0.0 ) 
//    {   // 1 root
//        h = sqrt(h);
//        vec2 x = (vec2(h,-h)-q)/2.0;
//
////        #if 0
////      
////        if( abs(p)<0.001 )
////        {
////            float k = p3/q;              // linear approx
////          //float k = (1.0-p3/q2)*p3/q;  // quadratic approx 
////            x = vec2(k,-k-q);  
////        }
////        #endif
//
//        vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
//        float t = uv.x + uv.y;
//
//		// from NinjaKoala - single newton iteration to account for cancellation
//        t -= (t*(t*t+3.0*p)+q)/(3.0*t*t+3.0*p);
//        
//        t = clamp( t-kx, 0.0, 1.0 );
//        vec2  w = d+(c+b*t)*t;
//        outQ = w + pos;
//        res = dot2(w);
//    	sgn = cro(c+2.0*b*t,w);
//    }
//    else 
//    {   // 3 roots
//        float z = sqrt(-p);
////        #if 0
////        float v = acos(q/(p*z*2.0))/3.0;
////        float m = cos(v);
////        float n = sin(v);
////        #else
//        float m = cos_acos_3( q/(p*z*2.0) );
//        float n = sqrt(1.0-m*m);
////        #endif
//        n *= sqrt(3.0);
//        vec3  t = clamp( vec3(m+m,-n-m,n-m)*z-kx, 0.0, 1.0 );
//        vec2  qx=d+(c+b*t.x)*t.x; float dx=dot2(qx), sx=cro(a+b*t.x,qx);
//        vec2  qy=d+(c+b*t.y)*t.y; float dy=dot2(qy), sy=cro(a+b*t.y,qy);
//        if( dx<dy ) {res=dx;sgn=sx;outQ=qx+pos;} else {res=dy;sgn=sy;outQ=qy+pos;}
//    }
//    
//    return sqrt( res )*sign(sgn);
//}
//

void main(){

    float alpha = max(max(1.0-2.0*length(PosInWorld-Corner1),
    1.0-2.0*length(PosInWorld-Corner2)),
    max(1.0-2.0*length(PosInWorld-NearPoint),
    1.0-2.0*length(PosInWorld-FarPoint)));

//    vec2 fardiff = PosInWorld-FarPoint;


//    alpha = min(1.0-3.5*sdParabola(fardiff,-1.8),
//     3.5*sdParabola(PosInWorld-NearPoint,-0.15));
//

//    vec2 q;
//    float d  = sdBezier(PosInWorld,Corner1,FarPoint,Corner2,q);
//    float b2  = sdBezier(PosInWorld,Corner1,NearPoint,Corner2,q);
//
//    d = b2*d;

    float d = alpha;

//    alpha = b1;

    vec3 col = (d>0.0) ? vec3(0.9,0.6,0.3) : vec3(0.65,0.85,1.0);
	col *= 1.0 - exp(-4.0*abs(d));
	col *= 0.8 + 0.2*cos(110.0*d);
	col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.015,abs(d)) );

    FragColor = vec4(col,1.0);
    return;

    if(alpha < 0.0){
        FragColor = vec4(1.0,1.0,0.0,max(alpha,0.1));
        return;
    }
    FragColor = vec4(1.0,0.0,1.0,1.0);

//    float alpha = max(max(1.0-2.0*length(PosInWorld-Corner1),
//        1.0-2.0*length(PosInWorld-Corner2)),
//            1.0-2.0*length(PosInWorld-Midpoint)
//            );
//    float alpha = max(1.0-3.5*sdBox(PosInWorld,Corner1,Corner2),
//        6.25-3.5*length(PosInWorld-Midpoint));
//
//    if(alpha < 1.0){
//        FragColor = vec4(1.0,1.0,0.0,max(alpha,0.1));
//        return;
//    }
//    FragColor = vec4(1.0,0.0,1.0,1.0);
//
////    FragColor = vec4(1.0,0.0,1.0,1.0);
////    float alpha = 1.0-2.5*sdTrapezoid(PosInWorld,A,B,1.5,1.0);
////    float alphaA = 1.0-5.0*length(PosInWorld-A);
////    float alphaB = 1.0-5.0*length(PosInWorld-B);
////
////    if(alpha > 0){
////        if(alpha > 0.5){
////            float segAlpha = 1.0-5.0*sdSegment(PosInWorld,SelectA,SelectB);
////	        FragColor = vec4(vec3(0.1,0.1,0.1)*min(1.0-segAlpha,1.0)+vec3(1.0,0.0,1.0)*segAlpha,1.0);
////        }
////        else {
////	        FragColor = vec4(0.1,0.7,0.1,1.0);
////        }
////    }
////    else {
////
////        FragColor = vec4(1.0,1.0,0.0,max(2.0*alpha+1.2,0.1));
////    }
}
