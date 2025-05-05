//Or.frag

#version 420 core

//In
in vec2 PosInWorld;
flat in vec2 Base1;
flat in vec2 Base2;
flat in vec2 BaseFar;
flat in vec2 ControllNear;
flat in vec2 Controll1;
flat in vec2 Controll2;
flat in vec2 BaseTriangle;
flat in int Orientation;
flat in vec3 HighlightColor;
flat in uint id;

//Out
layout(location = 0)out vec4 FragColor;
layout(location = 1)out uint Id;
//layout(location = 1)out ivec4 Id;
//layout(location = 1)out vec4 Id;

//Uniforms
uniform vec4 UBodyColorA = vec4(0.2,0.2,0.2,1.0);
uniform bool UIDRun;

float sdBox( in vec2 p, in vec2 a, in vec2 b )
{
	vec2 q = abs(p - (a + b) * 0.5) - (b - a) * 0.5;

	return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

float dot2( vec2 v ) { return dot(v,v); }

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

float sdTriangleIsosceles( in vec2 p, in vec2 q )
{
	p.x = abs(p.x);
	vec2 a = p - q*clamp( dot(p,q)/dot(q,q), 0.0, 1.0 );
	vec2 b = p - q*vec2( clamp( p.x/q.x, 0.0, 1.0 ), 1.0 );
	float s = -sign( q.y );
	vec2 d = min( vec2( dot(a,a), s*(p.x*q.y-p.y*q.x) ),
				  vec2( dot(b,b), s*(p.y-q.y)  ));
	return -sqrt(d.x)*sign(d.y);
}

float sdTriangleIsoscelesRot( in vec2 p, in vec2 q )
{
	if(Orientation == 0) {
		return sdTriangleIsosceles(p, q);//Up
	}
	if(Orientation == 1) {
		return sdTriangleIsosceles(vec2(-p.y, p.x), q);//Right
	}
	if(Orientation == 2) {
		return sdTriangleIsosceles(vec2(p.x, -p.y), q);//Down
	}
	return sdTriangleIsosceles(vec2(p.y, -p.x), q);//Left
}

void main(){

//    float alpha =
//   max(max(max(1.0-2.0*length(PosInWorld-Base1),
//    1.0-2.0*length(PosInWorld-Base2)),
//    max(1.0-2.0*length(PosInWorld-BaseFar),
//    1.0-2.0*length(PosInWorld-ControllNear))),
//    max(1.0-2.0*length(PosInWorld-Controll1),
//    1.0-2.0*length(PosInWorld-Controll2)));
//    vec2 fardiff = PosInWorld-FarPoint;

	float d  = sdBezier(PosInWorld, Base1, Controll1, BaseFar) - 0.2;
	d = min(d,sdBezier(PosInWorld, Base2 ,Controll2, BaseFar) - 0.2);
	d = min(d,sdBezier(PosInWorld, Base1, ControllNear, Base2) - 0.2);
	d = min(d, sdTriangleIsoscelesRot(PosInWorld - BaseTriangle,vec2(1.2, -3.0)) - 0.44);

//	vec3 col = (d>0.0) ? vec3(0.9,0.6,0.3) : vec3(0.65,0.85,1.0);
//	col *= 1.0 - exp(-4.0*abs(d));
//	col *= 0.8 + 0.2*cos(110.0*d);
//	col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.015,abs(d)) );
//
//	FragColor = vec4(col,1.0);
//	return;

	if(d > 0.0){
		if(UIDRun){
			discard;
			return;
		}

		if(HighlightColor == vec3(0)){
			FragColor = vec4(0.0);
		}
		else {
			FragColor = vec4(HighlightColor, 1.0 - 5.0 * d);
		}
//	   FragColor = vec4(UHighlightColor,max(1.0-5.0*d,0.1));
//		Id = 0u;
//		Id = vec4(0,0,0,0);
		return;
	}
	if(UIDRun){
		Id = id;
		return;
	}

	FragColor = UBodyColorA;
//	Id = vec4(vec3(id),1.0);
//	Id = uvec4(id & 65535u,(id >> 16) & 65535u,0,1);
}
