//NDot.frag

#version 330 core

//In
in vec2 PosInWorld;
flat in vec2 PosOfPoint;

//Out
out vec4 FragColor;

//Uniforms
uniform vec4 UBodyColorA = vec4(0.2,0.2,0.2,1.0);
uniform vec3 UHighlightColor = vec3(1.0,1.0,0.0);

void main(){
//
//	FragColor = vec4(0.5);
//	return;

	float d = 1.0-2*length(PosInWorld-PosOfPoint);

//	vec3 col = (d>0.0) ? vec3(0.9,0.6,0.3) : vec3(0.65,0.85,1.0);
//	col *= 1.0 - exp(-4.0*abs(d));
//	col *= 0.8 + 0.2*cos(110.0*d);
//	col = mix( col, vec3(1.0), 1.0-smoothstep(0.0,0.015,abs(d)) );
//
//	FragColor = vec4(col,1.0);
//	return;

//	FragColor = vec4(1.0,0.0,0.0,1.0-d);

	if(d < 0.0){
		FragColor = vec4(0.0);
//		FragColor = vec4(UHighlightColor.r*d);
//	   FragColor = vec4(UHighlightColor,max(5*d+2,0.1));
	   return;
	}
//		FragColor = vec4(UHighlightColor.r*d);

	FragColor = UBodyColorA;

}
