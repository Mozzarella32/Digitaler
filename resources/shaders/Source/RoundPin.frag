//RoundPin.frag

#version 330 core

//In
in vec2 PosInWorld;
in vec2 ScreenPos;
flat in vec2 PosOfPin;
flat in vec3 Color;

//Out
out vec4 FragColor;

//flat in int Flags;

//uniform
uniform bool UHasWire;
uniform sampler2D UPath;

void main(){
//    //float expr = 1.0-10*sdBox(PosInWorld,A,B);
//    //FragColor = vec4(Color.rgb,min(expr,0.5));
////    float box = sdBox(PosInWorld,A,B);
//
////    FragColor = vec4(vec3(box),0.5);
//
////    FragColor = vec4(Color,sdBox(PosInWorld,A,B));
//
////    FragColor = vec4(Color,0.5*length(PosInWorld-Other));
//
//    float alpha1 = 1.0-10*step(length(PosInWorld-PosOfPin),0.11);
//    vec4 OldColor = vec4(Color,alpha1);
//
////    float alpha = length(PosInWorld-PosOfPin);
//    float horizontal = float(A.y == B.y);
//    float alpha = 1.0-4.5*sdBox(PosInWorld,A,B)+1.0;
//    
//    //alpha += 1.0-4.5*min(length(PosInWorld-OtherUp),length(PosInWorld-OtherDown));
//
//    if(alpha > 1.0 && alpha1 != 1.0){
////        FragColor = vec4(1.0,0.0,1.0,1.0);
//        if(UHasWire){
//            FragColor = vec4(0.0);
//            return;
//        }
//        FragColor = vec4(0.05,0.05,0.05,1.0);
//        return;
//    }
//
//    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,0)+OldColor.a*OldColor;
////    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,alpha)+OldColor.a*OldColor;
//
//    //FragColor= vec4(1.0,0.0,1.0,1.0-4.5*min(length(PosInWorld-OtherUp),length(PosInWorld-OtherDown)));
//    //FragColor= vec4(1.0,0.0,1.0,1.0-4.5*(horizontal*abs(PosInWorld.x-Other.x)+(1-horizontal)*abs(PosInWorld.y-Other.y)));
////    FragColor= vec4(1.0,0.0,1.0,1.0-length(PosInWorld-OtherUp));
//    //FragColor = vec4(1.0,1.0,0.0,alpha);
////    FragColor = vec4(Color.rgb,0.8);
////    FragColor = vec4(vec3(length(PosInWorld-PosOfPin)),0.8);
//    //FragColor = vec4(1.0,1.0,0.0,1.0);
////    FragColor = vec4(PosInWorld.rg,0,0.5);
//	FragColor = vec4(1.0,0.0,1.0,1.0);

    float l = length(PosInWorld-PosOfPin);

    float alpha1 = 1.0-10*step(l,0.11)-step(1.0-4.5*l,0.0);

    vec4 OldColor = vec4(Color,alpha1);

    float alpha = 1.0-4.5*l+1.0;

     if(alpha > 1.0 && alpha1 != 1.0){
//        FragColor = vec4(1.0,0.0,1.0,1.0);

         if(UHasWire){
            FragColor = vec4(texture(UPath,ScreenPos).rgb,1.0);
            return;
        }
        FragColor = vec4(0.05,0.05,0.05,1.0);
        return;
    }

   
    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,0)+OldColor.a*OldColor;
//    FragColor = (1.0-OldColor.a)*vec4(1.0,1.0,0.0,alpha)+OldColor.a*OldColor;

}