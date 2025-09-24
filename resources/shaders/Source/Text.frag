//CopyTexture.frag

#version 420 core

//In
in vec2 TextureCoord;
flat in vec4 Foreground;
flat in vec4 Background;
flat in float FontScale;

//Out
out vec4 FragColor;

//Uniforms
uniform sampler2D UTexture;
uniform float UZoomFactor;

float avg(float a, float b){
	return (a+b)/2.0;
}

float median(float a, float b, float c, float d){
	return avg(max(min(a,b),min(c,d)),min(max(a,b),max(c,d)));
}

float median(float a, float b, float c){
	return max(min(a,b),min(c,max(a,b)));
}

void main(){
	vec4 mtsdf = texture(UTexture, TextureCoord);

	float alpha = median(mtsdf.r, mtsdf.g, mtsdf.b);

	float screenPxDistance = max(FontScale * 1.0/UZoomFactor*0.001,1)*(alpha - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

	FragColor = mix(Background, Foreground, opacity);
	return;
}
