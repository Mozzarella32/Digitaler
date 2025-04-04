//CopyTexture.frag

#version 330 core

//In
in vec2 TextureCoord;
flat in vec4 Foreground;
flat in vec4 Background;

//Out
out vec4 FragColor;

//Uniforms
uniform sampler2D UTexture;

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

    FragColor = mtsdf;
    FragColor = vec4(mtsdf.rgb,1.0);
//    return;

	FragColor = mix(Background, Foreground, alpha);
//	return;
//
    if(alpha <= 0.0) {
        FragColor = Background;
        return;
    }
     if(alpha >= 0.5) {
        FragColor = Foreground;
        return;
    }

    FragColor = Background;
//
//    float anti = 1.0-abs(alpha-0.5);
//
//    anti = anti * anti;
//    anti = anti * anti;
//    anti = anti * anti;
//
//    FragColor = vec4(0.0,1.0,0.0,anti);
}
