//Gaussian.frag

#version 330 core
layout(location = 1) out uint FragColor;
  
in vec2 TextureCoord;

uniform usampler2D UImage;
uniform bool horizontal;
uniform float UZoomFactor;

uniform float weight[8] = float[](0.166500, 0.152656, 0.117657, 0.076229, 0.041517, 0.019008, 0.007316, 0.002367);

void main() {             
    int max = 8;
    
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
    FragColor = uint(result / 3.0);
}
