//Assets.frag

#version 420 core

precision highp float;

flat in int  ID;
flat in int  Index;
flat in vec2 FPoint1;
flat in vec2 FPoint2;
flat in vec2 FPoint;
flat in int  I1;
flat in vec4 ColorA;

in vec2 Pos;
in vec2 TextureCoord;

//Uniform
uniform float UZoomFactor;
 
layout(location = 0)out vec4 FragColor;
layout(location = 1)out uint Id;

uniform sampler2D UBackground;
uniform sampler2D UPath;

uniform bool UWirePass;
uniform bool UIDRun;

float dot2(vec2 v) {
    return dot(v, v);
}

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float sdSegmentSharp( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 pa = p-a, ba = b-a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	vec2 r = ba*h;
	return abs(pa.x-r.x)+abs(pa.y-r.y);
}

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

float sdTunnel( in vec2 p, in vec2 wh )
{
    p.x = abs(p.x); p.y = -p.y;
    vec2 q = p - wh;

    float d1 = dot2(vec2(max(q.x,0.0),q.y));
    q.x = (p.y>0.0) ? q.x : length(p)-wh.x;
    float d2 = dot2(vec2(q.x,max(q.y,0.0)));
    float d = sqrt( min(d1,d2) );
    
    return (max(q.x,q.y)<0.0) ? -d : d;
}

float sdBezier( in vec2 pos, in vec2 A, in vec2 B, in vec2 C ) {
	vec2 a = B - A;
	vec2 b = A - 2.0*B + C;
	vec2 c = a * 2.0;
	vec2 d = A - pos;
	float kk = 1.0/dot(b,b);
	float kx = kk * dot(a,b);
	float ky = kk * (2.0*dot(a,a)+dot(d,b)) / 3.0;
	float kz = kk * dot(d,a);      
	float res = 0.0;
	float p = ky - kx*kx;
	float p3 = p*p*p;
	float q = kx*(2.0*kx*kx-3.0*ky) + kz;
	float h = q*q + 4.0*p3;
	if( h >= 0.0) 
	{ 
		h = sqrt(h);
		vec2 x = (vec2(h,-h)-q)/2.0;
		vec2 uv = sign(x)*pow(abs(x), vec2(1.0/3.0));
		float t = clamp( uv.x+uv.y-kx, 0.0, 1.0 );
		res = dot2(d + (c + b*t)*t);
	}
	else
	{
		float z = sqrt(-p);
		float v = acos( q/(p*z*2.0) ) / 3.0;
		float m = cos(v);
		float n = sin(v)*1.732050808;
		vec3  t = clamp(vec3(m+m,-n-m,n-m)*z-kx,0.0,1.0);
		res = min( dot2(d+(c+b*t.x)*t.x),
				   dot2(d+(c+b*t.y)*t.y) );
		// the third root cannot be the closest
		// res = min(res,dot2(d+(c+b*t.z)*t.z));
	}
	return sqrt( res );
}

float sdTriangle( in vec2 p, in vec2 p0, in vec2 p1, in vec2 p2 )
{
    vec2 e0 = p1-p0, e1 = p2-p1, e2 = p0-p2;
    vec2 v0 = p -p0, v1 = p -p1, v2 = p -p2;
    vec2 pq0 = v0 - e0*clamp( dot(v0,e0)/dot(e0,e0), 0.0, 1.0 );
    vec2 pq1 = v1 - e1*clamp( dot(v1,e1)/dot(e1,e1), 0.0, 1.0 );
    vec2 pq2 = v2 - e2*clamp( dot(v2,e2)/dot(e2,e2), 0.0, 1.0 );
    float s = sign( e0.x*e2.y - e0.y*e2.x );
    vec2 d = min(min(vec2(dot(pq0,pq0), s*(v0.x*e0.y-v0.y*e0.x)),
                     vec2(dot(pq1,pq1), s*(v1.x*e1.y-v1.y*e1.x))),
                     vec2(dot(pq2,pq2), s*(v2.x*e2.y-v2.y*e2.x)));
    return -sqrt(d.x)*sign(d.y);
}

float sdParallelogram( in vec2 p, float wi, float he, float sk )
{
    float db = abs(p.y)-he;
    if( db>0.0 && abs(p.x-sk*sign(p.y))<wi )
        return db;
        
    float e2 = sk*sk + he*he;
    float h  = p.x*he - p.y*sk;
    float da = (abs(h)-wi*he)*inversesqrt(e2);
    if( da<0.0 && db<0.0 )
        return max( da, db );

    vec2 q = (h<0.0)?-p:p; q.x -= wi;
    float v = abs(q.x*sk+q.y*he);
    if( v<e2 )
        return da;
    
    return sqrt( dot(q,q)+e2-2.0*v );
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

float sdIntersect(float sd1, float sd2) {
    return max(sd1, sd2);
}

float sdSub(float pos, float neg) {
    return max(pos, -neg);
}
float sdUnion(float sd1, float sd2) {
    return min(sd1, sd2);
}

float sdOr(vec2 Pos) {
    Pos.x = abs(Pos.x);
    return
    sdUnion(
        sdUnion(
            sdBezier(Pos, vec2(0.0, 1.9), vec2(1.0, 1.9), vec2(1.25, -1.9)) - 0.15,
            sdBezier(Pos, vec2(-1.25, -1.9), vec2(0.0, -1.0), vec2(1.25, -1.9)) - 0.15
        ),
        sdTriangle(Pos, vec2(0.0, 1.5), vec2(0.0, -1.0), vec2(0.75, -1.2)) - 0.5
    );
}

float sdXor(vec2 Pos) {
    Pos.x = abs(Pos.x);
    return
    sdUnion(
        sdOr(Pos),
        sdBezier(Pos, vec2(-1.25, -2.6), vec2(0.0, -1.7), vec2(1.25, -2.6)) - 0.05
    );
}

vec4 sdSevenSeg(vec2 Pos) {

    vec2 PosUnshifted = Pos;
    Pos.x -= 0.1 * Pos.y;
    
    int flags = I1;
    
    float off = 0.55;
    vec2 AL = vec2(-off, 1.6);
    vec2 AR = vec2(off, 1.6);
    vec2 BU = vec2(0.8, 0.8 + off);
    vec2 BD = vec2(0.8, 0.8 - off);
    vec2 CU = vec2(0.8, -0.8 + off);
    vec2 CD = vec2(0.8, -0.8 - off);
    vec2 DL = vec2(-off, -1.6);
    vec2 DR = vec2(off, -1.6);
    vec2 EU = vec2(-0.8, -0.8 + off);
    vec2 ED = vec2(-0.8, -0.8 - off);
    vec2 FU = vec2(-0.8, 0.8 + off);
    vec2 FD = vec2(-0.8, 0.8 - off);
    vec2 GL = vec2(-off, 0.0);
    vec2 GR = vec2(off, 0.0);
    vec2 P =  vec2(1.0, -1.6);

	float A = sdSegmentSharp(Pos,AL,AR);
	float B = sdSegmentSharp(Pos,BU,BD);
	float C = sdSegmentSharp(Pos,CU,CD);
	float D = sdSegmentSharp(Pos,DL,DR);
	float E = sdSegmentSharp(Pos,EU,ED);
	float F = sdSegmentSharp(Pos,FU,FD);
	float G = sdSegmentSharp(Pos,GL,GR);
	float Point = length(P-(PosUnshifted));

    float On = 1.0 / 0.0;
    if ((flags & 0x1 ) != 0) On = min(On, A);
    if ((flags & 0x2 ) != 0) On = min(On, B);
    if ((flags & 0x4 ) != 0) On = min(On, C);
    if ((flags & 0x8 ) != 0) On = min(On, D);
    if ((flags & 0x10) != 0) On = min(On, E);
    if ((flags & 0x20) != 0) On = min(On, F);
    if ((flags & 0x40) != 0) On = min(On, G);
    if ((flags & 0x80) != 0) On = min(On, Point);

    float Off = 1.0 / 0.0;
    if ((flags & 0x1 ) == 0) Off = min(Off, A);
    if ((flags & 0x2 ) == 0) Off = min(Off, B);
    if ((flags & 0x4 ) == 0) Off = min(Off, C);
    if ((flags & 0x8 ) == 0) Off = min(Off, D);
    if ((flags & 0x10) == 0) Off = min(Off, E);
    if ((flags & 0x20) == 0) Off = min(Off, F);
    if ((flags & 0x40) == 0) Off = min(Off, G);
    if ((flags & 0x80) == 0) Off = min(Off, Point);

    On -= 0.12;
    Off -= 0.12;

    vec4 SegColor;

    if(On < Off) {
		SegColor = vec4(ColorA.rgb, max(2.0 * On, 0.0));
	}
	else{
	    SegColor = vec4(0.2, 0.2, 0.2, max(2.0 * Off, 0.0));
	}

    vec4 box = vec4(0.1, 0.1, 0.1 , sdBox(PosUnshifted, FPoint + 0.3));

    return vec4(mix(box.rgb, SegColor.rgb,  max(0, 1.0 - 10.0 * SegColor.a)), box.a);
 }


vec4 sdSixteenSeg(vec2 Pos) {

    vec2 PosUnshifted = Pos;
    Pos.x -= 0.1 * Pos.y;
    
    int flags = I1;
    
    float off = 0.55;
    vec2 AL = vec2(-0.4 - 0.2, 1.6);
    vec2 AR = vec2(-0.4 + 0.2, 1.6);
    vec2 BL = vec2(0.4 - 0.2, 1.6);
    vec2 BR = vec2(0.4 + 0.2, 1.6);
    vec2 CU = vec2(0.8, 0.8 + 0.6);
    vec2 CD = vec2(0.8, 0.8 - 0.6);
    vec2 DU = vec2(0.8, -0.8 + 0.6);
    vec2 DD = vec2(0.8, -0.8 - 0.6);
    vec2 EL = vec2(0.4 - 0.2, -1.6);
    vec2 ER = vec2(0.4 + 0.2, -1.6);
    vec2 FL = vec2(-0.4 - 0.2, -1.6);
    vec2 FR = vec2(-0.4 + 0.2, -1.6);
    vec2 GU = vec2(-0.8, -0.8 + 0.6);
    vec2 GD = vec2(-0.8, -0.8 - 0.6);
    vec2 HU = vec2(-0.8, 0.8 + 0.6);
    vec2 HD = vec2(-0.8, 0.8 - 0.6);
    vec2 IL = vec2(-0.4 - 0.2, 0.0);
    vec2 IR = vec2(-0.4 + 0.2, 0.0);
    vec2 JL = vec2(0.4 - 0.2, 0.0);
    vec2 JR = vec2(0.4 + 0.2, 0.0);
    vec2 K =  vec2(-0.4, 0.8);
    vec2 LU = vec2(0.0, 0.8 + 0.6);
    vec2 LD = vec2(0.0, 0.8 - 0.6);
    vec2 M =  vec2(0.4, 0.8);
    vec2 N =  vec2(-0.4, -0.8);
    vec2 OU = vec2(0.0, -0.8 + 0.6);
    vec2 OD = vec2(0.0, -0.8 - 0.6);
    vec2 P =  vec2(0.4, -0.8);
    vec2 Point = vec2(1.0, -1.6);

    float a = sdSegmentSharp(Pos,AL,AR);
    float b = sdSegmentSharp(Pos,BL,BR);
    float c = sdSegmentSharp(Pos,CU,CD);
    float d = sdSegmentSharp(Pos,DU,DD);
    float e = sdSegmentSharp(Pos,EL,ER);
    float f = sdSegmentSharp(Pos,FL,FR);
    float g = sdSegmentSharp(Pos,GU,GD);
    float h = sdSegmentSharp(Pos,HU,HD);
    float i = sdSegmentSharp(Pos,IL,IR);
    float j = sdSegmentSharp(Pos,JL,JR);
    float k = sdParallelogram(K-Pos,0.1,0.6,-0.10) + 0.15;
    float l = sdSegmentSharp(Pos,LU,LD);
    float n = sdParallelogram(N-Pos,0.1,0.6,0.10) + 0.15;
    float m = sdParallelogram(M-Pos,0.1,0.6,0.10) + 0.15;
    float o = sdSegmentSharp(Pos,OU,OD);
    float p = sdParallelogram(P-Pos,0.1,0.6,-0.10) + 0.15;
    float sdp = length(Point-PosUnshifted);

    float On = 1.0 / 0.0;
    if ((flags & 0x00001) != 0) On = min(On, a);
    if ((flags & 0x00002) != 0) On = min(On, b);
    if ((flags & 0x00004) != 0) On = min(On, c);
    if ((flags & 0x00008) != 0) On = min(On, d);
    if ((flags & 0x00010) != 0) On = min(On, e);
    if ((flags & 0x00020) != 0) On = min(On, f);
    if ((flags & 0x00040) != 0) On = min(On, g);
    if ((flags & 0x00080) != 0) On = min(On, h);
    if ((flags & 0x00100) != 0) On = min(On, i);
    if ((flags & 0x00200) != 0) On = min(On, j);
    if ((flags & 0x00400) != 0) On = min(On, k);
    if ((flags & 0x00800) != 0) On = min(On, l);
    if ((flags & 0x01000) != 0) On = min(On, m);
    if ((flags & 0x02000) != 0) On = min(On, n);
    if ((flags & 0x04000) != 0) On = min(On, o);
    if ((flags & 0x08000) != 0) On = min(On, p);
    if ((flags & 0x10000) != 0) On = min(On, sdp);

    float Off = 1.0 / 0.0;
    if ((flags & 0x00001) == 0) Off = min(Off, a);
    if ((flags & 0x00002) == 0) Off = min(Off, b);
    if ((flags & 0x00004) == 0) Off = min(Off, c);
    if ((flags & 0x00008) == 0) Off = min(Off, d);
    if ((flags & 0x00010) == 0) Off = min(Off, e);
    if ((flags & 0x00020) == 0) Off = min(Off, f);
    if ((flags & 0x00040) == 0) Off = min(Off, g);
    if ((flags & 0x00080) == 0) Off = min(Off, h);
    if ((flags & 0x00100) == 0) Off = min(Off, i);
    if ((flags & 0x00200) == 0) Off = min(Off, j);
    if ((flags & 0x00400) == 0) Off = min(Off, k);
    if ((flags & 0x00800) == 0) Off = min(Off, l);
    if ((flags & 0x01000) == 0) Off = min(Off, m);
    if ((flags & 0x02000) == 0) Off = min(Off, n);
    if ((flags & 0x04000) == 0) Off = min(Off, o);
    if ((flags & 0x08000) == 0) Off = min(Off, p);
    if ((flags & 0x10000) == 0) Off = min(Off, sdp);

    On -= 0.12;
    Off -= 0.12;

    vec4 SegColor;

    if(On < Off) {
		SegColor = vec4(ColorA.rgb, max(2.0 * On, 0.0));
	}
	else{
	    SegColor = vec4(0.2, 0.2, 0.2, max(2.0 * Off, 0.0));
	}

    vec4 box = vec4(0.1, 0.1, 0.1 , sdBox(PosUnshifted, FPoint + 0.3));

    return vec4(mix(box.rgb, SegColor.rgb,  max(0, 1.0 - 10.0 * SegColor.a)), box.a);
 }

vec4 sdMux(vec2 Pos) {

    vec2 A = vec2(0.0, -1.0);
    vec2 B = vec2(0.0, 1.0);
    float sd = sdTrapezoid(Pos, A, B, 1.5, 1.0) - 0.3;
    float sdinner = 2.0 * sd + 0.4;

    vec2 selA = vec2(0.0, 1.2);
    vec2 sel1 = vec2(-1.0, -1.2);
    vec2 sel2 = vec2(1.0, -1.2);

    vec2 selB = sel1 * I1 + sel2 * (1 - I1);

    vec4 sel = vec4(ColorA.rgb, max(2.0 * sdSegment(Pos, selA, selB) - 0.2, 0.0));

    vec4 inner = vec4(0.1, 0.1, 0.1, sdinner);

    inner = vec4(mix(inner.rgb, sel.rgb,  max(0, 1.0 - 10.0 * sel.a)), inner.a);

    vec4 outer = vec4(0.1, 0.7, 0.1, sd);

    return vec4(mix(outer.rgb, inner.rgb,  max(0, 1.0 - 10.0 * inner.a)), outer.a);
}

vec3 PathCornerColor = vec3(0.3, 1.4, 0.7);

vec4 holeColor() {
    vec4 Return = vec4(vec3(0.05), max(5.0 * length(Pos) - 0.5, 0.0));
    if(UWirePass) {
        Return.rgb = texture(UPath, TextureCoord).rgb;
        vec4 corner = vec4(PathCornerColor, 5.0 * length(Pos) - 0.25);
        return vec4(mix(Return.rgb, corner.rgb,  max(0, 1.0 - 10.0 * corner.a)), Return.a);
    }
    return Return;
}

vec4 sdPin(vec2 Pos) {
    vec4 hole = holeColor();
    vec4 mark = vec4(ColorA.rgb, max(5.0 * sdSegment(Pos, vec2(0.0, 0.0), vec2(0.0, -1.0)) - 0.5, 0.0));
    vec3 bodyColor = Index == 7 ? vec3(0.7, 0.0, 0.0) : vec3(0.0, 0.7, 0.0);
    vec4 body = vec4(bodyColor, sdTunnel(Pos, vec2(0.2, 1.0)));
    body = vec4(mix(body.rgb, mark.rgb,  max(0, 1.0 - 10.0 * mark.a)), body.a);
    return vec4(mix(body.rgb, hole.rgb,  max(0, 1.0 - 10.0 * hole.a)), body.a);
}

vec4 sdRoundPin(vec2 Pos) {
    vec4 hole = holeColor();
    vec3 bodyColor = Index == 9 ? vec3(0.7, 0.0, 0.0) : vec3(0.0, 0.7, 0.0);
    vec4 body = vec4(bodyColor, length(Pos) - 0.2);
    return vec4(mix(body.rgb, hole.rgb,  max(0, 1.0 - 10.0 * hole.a)), body.a);
}

vec4 sdPathEdge(vec2 Pos) {
    vec4 corner = vec4(PathCornerColor, sdUnion(
          5.0 * length(Pos - FPoint1) - 0.25,
          5.0 * length(Pos - FPoint2) - 0.25  
        ));
    vec4 segment = vec4(ColorA.rgb, 2.5 * sdSegment(Pos, FPoint1, FPoint2) - 0.25);
    return vec4(mix(segment.rgb, corner.rgb,  max(0, 1.0 - 10.0 * corner.a)), segment.a);
}

vec4 sdPathIntersectionPoints(vec2 Pos) {
    vec4 corner = vec4(PathCornerColor, 5.0 * length(Pos) - 0.25);
    vec4 segment = vec4(ColorA.rgb, 1.0 - 2.5 * length(abs(Pos) - vec2(0.5)));

    vec4 col = vec4(mix(segment.rgb, corner.rgb,  max(0, 1.0 - 10.0 * corner.a)), segment.a);
    vec4 pix = texture(UBackground, TextureCoord);
    return vec4(mix(pix.rgb, col.rgb, clamp(1.0 - 10.0 * col.a, 0.0, 1.0)), -1.0 / 0.0);
}

vec4 sdPathVertex(vec2 Pos) {
    vec4 corner = vec4(PathCornerColor, 5.0 * length(Pos) - 0.25);
    vec4 segment = vec4(ColorA.rgb, 2.5 * length(Pos) - 0.25);
    return vec4(mix(segment.rgb, corner.rgb,  max(0, 1.0 - 10.0 * corner.a)), segment.a);
}

vec4 sdAreaSelect(vec2 Pos) {
    Pos = abs(Pos);
    float sd = -max(Pos.x - FPoint.x, Pos.y - FPoint.y);
    // return vec4(UZoomFactor * 100.0, 0.0, 0.0, -1.0/0.0);
    if(sd < UZoomFactor * 5.0) {
        return vec4(ColorA.rgb, 0.015);
    //     // return vec4(ColorA.rgb, sd);
    }
    // sd += 1.0;
    // if(sd > 0.0) {
    //     return vec4(0.0, 0.0, 0.0, 0.1);
    // }
    // if(sd > -100.0 * UZoomFactor) {
    //     return vec4(ColorA.rgb, max(0.025, 20.0 * sdBox(Pos, FPoint - 0.1) - 1.95));
    // }
    return vec4(ColorA.rgb, 0.085);
}

// r g b sdf
vec4 get() {
    switch (Index) {
        case 0://Box
        return vec4(ColorA.rgb, sdBox(Pos, FPoint + 0.3));
        case 1://And
        return vec4(1.0,0.0,0.0, sdTunnel(Pos - vec2(0.0, 0.7), vec2(1.3,2.7)));
        case 2://Or
        return vec4(0.0,1.0,0.0, sdOr(Pos));
        case 3://Xor
        return vec4(0.0,0.0,1.0, sdXor(Pos));
        case 4://Seven
        return sdSevenSeg(Pos);
        case 5://Sixteen
        return sdSixteenSeg(Pos);
        case 6://Mux
        return sdMux(Pos);
        case 7://InputPin
        case 8://OutputPin
        return sdPin(Pos);
        case 9://InputPin
        case 10://OutputPin
        return sdRoundPin(Pos);
        case 11://PathEdge
        return sdPathEdge(Pos);
        case 12://PathIntersectionPoints
        return sdPathIntersectionPoints(Pos);
        case 13://PathVertex
        return sdPathVertex(Pos);
        case 14://AreaSelct
        return sdAreaSelect(Pos);
    }
    return vec4(ColorA.rgb, sdBox(Pos, FPoint + 0.3));
}

void main () {
    // if(Index == 14){
    //     FragColor = vec4(Pos,0.0,1.0);
    //     return;
    // }
    vec4 col = get();

    if(UIDRun) {
        if(1.0 - 10.0 * col.a > 0) {
            Id = ID;
            return;
        }
        else{
            discard;
        }
    }

    FragColor = vec4(col.rgb, clamp(1.0 - 10.0 * col.a, 0.0, 1.0));
    // if(Index == 6) discard;
    // FragColor = vec4(col.rgb, col.a);
    // if(1.0 - 10.0 * col.a < 0.0){
    //     FragColor = vec4(1.0,0.0,0.0,0.5);
    // }
    // FragColor = vec4(ColorA.rgb, min(sd, ColorA.a));
    // FragColor = vec4(Pos.xy, -Pos.x, 1.0);
    // FragColor = ColorA;
}
