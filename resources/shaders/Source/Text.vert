//CopyTexture.vert

#version 330 core


//In
layout(location = 0) in int InIndex;

//In
layout(location = 1) in vec2 InstanceInPos;
layout(location = 2) in vec4 InstanceInPosOff;
layout(location = 3) in vec4 InstanceInUVOff;
layout(location = 4) in int InstanceInOrientation;
layout(location = 5) in vec4 InstanceInForeground;
layout(location = 6) in vec4 InstanceInBackground;

//Out
out vec2 TextureCoord;
flat out vec4 Foreground;
flat out vec4 Background;

//Uniforms
uniform vec2 UOffset;
uniform vec2 UZoom;


float Directions[2*4] = {
	0,1,
	1,0,
	0,-1,
	-1,0
};

int IndexTable[2*4] = {
	0,1,
	0,3,
	2,1,
	2,3,
};

void main() {
	vec2 Right = vec2(Directions[(InstanceInOrientation+1)%4*2+0],Directions[(InstanceInOrientation+1)%4*2+1]);
	vec2 Up = vec2(Directions[(InstanceInOrientation+0)%4*2+0],Directions[(InstanceInOrientation+0)%4*2+1]);

    float OffPos[4] = {InstanceInPosOff.x,InstanceInPosOff.y,InstanceInPosOff.z,InstanceInPosOff.w};

    ivec2 Index = ivec2(IndexTable[InIndex*2],IndexTable[InIndex*2+1]);

    vec2 Pos = InstanceInPos + OffPos[Index.x] * Right + OffPos[Index.y] * Up;

	Pos += Up*0.0;

    float OffUV[4] = {InstanceInUVOff.x,InstanceInUVOff.y,InstanceInUVOff.z,InstanceInUVOff.w};

    TextureCoord = vec2(OffUV[Index.x],OffUV[Index.y]);

    Foreground = InstanceInForeground;
    Background = InstanceInBackground;

    gl_Position = vec4((Pos+UOffset)/UZoom, 0.0, 1.0);
}

