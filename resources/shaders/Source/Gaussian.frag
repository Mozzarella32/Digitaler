//Gaussian.frag

#version 330 core
layout(location = 1) out uint FragColor;
  
in vec2 TextureCoord;

uniform usampler2D UImage;
uniform bool horizontal;
uniform float UZoomFactor;
// uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
// uniform float weight[9] = float[] (0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06);
// uniform float weight[9] = float[] ();

// uniform float weight[10] = float[](0.156024, 0.152935, 0.144028, 0.130322, 0.113297, 0.094633, 0.075945, 0.058558, 0.043381, 0.030877);

uniform float weight[20] = float[](0.080691, 0.080289, 0.079093, 0.077140, 0.074487, 0.071210, 0.067399, 0.063157, 0.058594, 0.053819, 0.048942, 0.044063, 0.039277, 0.034661, 0.030284, 0.026197, 0.022435, 0.019023, 0.015969, 0.013272);


void main() {             
    int max = 20;

    max = 20 - int(UZoomFactor / 0.01);

    max = clamp(max, 5, 20);
    
    vec2 tex_offset = 1.0 / textureSize(UImage, 0);
    float result = float(texture(UImage, TextureCoord).r) * weight[0];
    if(horizontal) {
        for(int i = 1; i < max; ++i) {
			//float off = i / 1.0 + i * i / 20.0;
			float off = i;
            result += float(texture(UImage, TextureCoord + vec2(tex_offset.x * off, 0.0)).r) * weight[i];
            result += float(texture(UImage, TextureCoord - vec2(tex_offset.x * off, 0.0)).r) * weight[i];
        }
    }
    else {
        for(int i = 1; i < max; ++i) {
			//float off = i / 1.0 + i * i / 20.0;
			float off = i;
            result += float(texture(UImage, TextureCoord + vec2(0.0, tex_offset.y * off)).r) * weight[i];
            result += float(texture(UImage, TextureCoord - vec2(0.0, tex_offset.y * off)).r) * weight[i];
        }
    }
    FragColor = uint(result / 1.9);
}
