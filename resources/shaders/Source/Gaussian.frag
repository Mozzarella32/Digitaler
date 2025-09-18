//Gaussian.frag

#version 330 core
layout(location = 1) out uint FragColor;
  
in vec2 TextureCoord;

uniform usampler2D UImage;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
// uniform float weight[9] = float[] (0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06,0.06);

void main() {             
    vec2 tex_offset = 1.0 / textureSize(UImage, 0);
    float result = float(texture(UImage, TextureCoord).r) * weight[0];
    if(horizontal) {
        for(int i = 1; i < 5; ++i) {
			// float off = i / 1.0 + i * i / 5.0;
			float off = i;
            result += float(texture(UImage, TextureCoord + vec2(tex_offset.x * off, 0.0)).r) * weight[i];
            result += float(texture(UImage, TextureCoord - vec2(tex_offset.x * off, 0.0)).r) * weight[i];
        }
    }
    else {
        for(int i = 1; i < 5; ++i) {
			// float off = i / 1.0 + i * i / 5.0;
			float off = i;
            result += float(texture(UImage, TextureCoord + vec2(0.0, tex_offset.y * off)).r) * weight[i];
            result += float(texture(UImage, TextureCoord - vec2(0.0, tex_offset.y * off)).r) * weight[i];
        }
    }
    FragColor = uint(result);
}
