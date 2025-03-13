//SevenSeg.frag

#version 330 core

//In
in vec2 PosInWorld;
in vec2 PosInWorldUnshifted;
in vec2 ScreenPos;
flat in vec3 Color;
flat in int Flags;
flat in vec2 AL;
flat in vec2 AR;
flat in vec2 BL;
flat in vec2 BR;
flat in vec2 CU;
flat in vec2 CD;
flat in vec2 DU;
flat in vec2 DD;
flat in vec2 EL;
flat in vec2 ER;
flat in vec2 FL;
flat in vec2 FR;
flat in vec2 GU;
flat in vec2 GD;
flat in vec2 HU;
flat in vec2 HD;
flat in vec2 IL;
flat in vec2 IR;
flat in vec2 JL;
flat in vec2 JR;
flat in vec2 K;
flat in vec2 LU;
flat in vec2 LD;
flat in vec2 M;
flat in vec2 N;
flat in vec2 OU;
flat in vec2 OD;
flat in vec2 P;
flat in vec2 Point;
flat in float Rotate;

//Out
out vec4 FragColor;

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    vec2 r = ba*h;
    return abs(pa.x-r.x)+abs(pa.y-r.y);
}

float sdParallelogramNonRot( in vec2 p, float wi, float he, float sk )
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



float sdParallelogramRot( in vec2 p, float wi, float he, float sk )
{
    float db = abs(p.x)-he;
    if( db>0.0 && abs(p.y+sk*sign(p.x))<wi )
        return db;
        
    float e2 = sk*sk + he*he;
    float h  = p.y*he + p.x*sk;
    float da = (abs(h)-wi*he)*inversesqrt(e2);
    if( da<0.0 && db<0.0 )
        return max( da, db );

    vec2 q = (h<0.0)?-p:p;
    q.y -= wi;
    float v = abs(q.y*sk-q.x*he);
    if( v<e2 )
        return da;
    
    return sqrt( dot(q,q)+e2-2.0*v );
}

float sdParallelogram( in vec2 p, float wi, float he, float sk ){
    if(Rotate == 0){
        return sdParallelogramRot(p,wi,he,sk);
    }
    return sdParallelogramNonRot(p,wi,he,sk);
}


void main(){

//    float A = 1.0-4.0*sdSegment(PosInWorld,AL,AR);
//    float B = 1.0-4.0*sdSegment(PosInWorld,BU,BD);
//    float C = 1.0-4.0*sdSegment(PosInWorld,CU,CD);
//    float D = 1.0-4.0*sdSegment(PosInWorld,DL,DR);
//    float E = 1.0-4.0*sdSegment(PosInWorld,EU,ED);
//    float F = 1.0-4.0*sdSegment(PosInWorld,FU,FD);
//    float G = 1.0-4.0*sdSegment(PosInWorld,GL,GR);

    float A = 1.0 - 4.0 * sdSegment(PosInWorld,AL,AR);
    float B = 1.0 - 4.0 * sdSegment(PosInWorld,BL,BR);
    float C = 1.0 - 4.0 * sdSegment(PosInWorld,CU,CD);
    float D = 1.0 - 4.0 * sdSegment(PosInWorld,DU,DD);
    float E = 1.0 - 4.0 * sdSegment(PosInWorld,EL,ER);
    float F = 1.0 - 4.0 * sdSegment(PosInWorld,FL,FR);
    float G = 1.0 - 4.0 * sdSegment(PosInWorld,GU,GD);
    float H = 1.0 - 4.0 * sdSegment(PosInWorld,HU,HD);
    float I = 1.0 - 4.0 * sdSegment(PosInWorld,IL,IR);
    float J = 1.0 - 4.0 * sdSegment(PosInWorld,JL,JR);
    float K = 0.0 - 5.0 * sdParallelogram(K-PosInWorld,0.17,0.645,-0.08);
//    float K = 1.0-2.0*length(K-PosInWorld);
    float L = 1.0 - 4.0 * sdSegment(PosInWorld,LU,LD);
    float N = 0.0 - 5.0 * sdParallelogram(N-PosInWorld,0.17,0.645,0.08);
    float M = 0.0 - 5.0 * sdParallelogram(M-PosInWorld,0.17,0.645,0.08);
    float O = 1.0 - 4.0 * sdSegment(PosInWorld,OU,OD);
    float P = 0.0 - 5.0 * sdParallelogram(P-PosInWorld,0.17,0.645,-0.08);
    float sdP = 1.0-4.0*length(Point-PosInWorldUnshifted);

//    float On = 
//        max(max(max(max(max(A * float((Flags & 0x00001) != 0),
//        B * float((Flags & 0x00002) != 0)),
//        max(C * float((Flags & 0x00004) != 0),
//        D * float((Flags & 0x00008) != 0))),
//        max(max(E * float((Flags & 0x00010) != 0),
//        F * float((Flags & 0x00020) != 0)),
//        max(G * float((Flags & 0x00040) != 0),
//        H * float((Flags & 0x00080) != 0)))),
//        max(max(max(I * float((Flags & 0x00100) != 0),
//        J * float((Flags & 0x00200) != 0)),
//        max(K * float((Flags & 0x00400) != 0),
//        L * float((Flags & 0x00800) != 0))),
//        max(max(M * float((Flags & 0x01000) != 0),
//        N * float((Flags & 0x02000) != 0)),
//        max(O * float((Flags & 0x00400) != 0),
//        P * float((Flags & 0x08000) != 0)))),
//        sdP * float((Flags & 0x10000) != 0));

    float On = max(
        max(
            max(
                max(
                    max(A * float((Flags & 0x00001) != 0),
                        B * float((Flags & 0x00002) != 0)),
                    max(C * float((Flags & 0x00004) != 0),
                        D * float((Flags & 0x00008) != 0))),
                max(
                    max(E * float((Flags & 0x00010) != 0),
                        F * float((Flags & 0x00020) != 0)),
                    max(G * float((Flags & 0x00040) != 0),
                        H * float((Flags & 0x00080) != 0)))),
            max(
                max(
                    max(I * float((Flags & 0x00100) != 0),
                        J * float((Flags & 0x00200) != 0)),
                    max(K * float((Flags & 0x00400) != 0),
                        L * float((Flags & 0x00800) != 0))),
                max(
                    max(M * float((Flags & 0x01000) != 0),
                        N * float((Flags & 0x02000) != 0)),
                    max(O * float((Flags & 0x04000) != 0),
                        P * float((Flags & 0x08000) != 0))))),
        sdP * float((Flags & 0x10000) != 0));


   float Off = max(
    max(
        max(
            max(
                max(A * float((Flags & 0x00001) == 0),
                    B * float((Flags & 0x00002) == 0)),
                max(C * float((Flags & 0x00004) == 0),
                    D * float((Flags & 0x00008) == 0))),
            max(
                max(E * float((Flags & 0x00010) == 0),
                    F * float((Flags & 0x00020) == 0)),
                max(G * float((Flags & 0x00040) == 0),
                    H * float((Flags & 0x00080) == 0)))),
        max(
            max(
                max(I * float((Flags & 0x00100) == 0),
                    J * float((Flags & 0x00200) == 0)),
                max(K * float((Flags & 0x00400) == 0),
                    L * float((Flags & 0x00800) == 0))),
            max(
                max(M * float((Flags & 0x01000) == 0),
                    N * float((Flags & 0x02000) == 0)),
                max(O * float((Flags & 0x04000) == 0),
                    P * float((Flags & 0x08000) == 0))))),
    sdP * float((Flags & 0x10000) == 0));


//    float On = 
//       A*float((Flags & 0x1) != 0),
//       B*float((Flags & 0x2) != 0),
//       C*float((Flags & 0x4) != 0),
//       D*float((Flags & 0x8) != 0),
//       E*float((Flags & 0x10) != 0),
//       F*float((Flags & 0x20) != 0),
//       G*float((Flags & 0x40) != 0),
//       Point*float((Flags & 0x80) != 0);
//
//    float Off = 
//       A*float((Flags & 0x1) == 0),
//       B*float((Flags & 0x2) == 0),
//       C*float((Flags & 0x4) == 0),
//       D*float((Flags & 0x8) == 0),
//       E*float((Flags & 0x10) == 0),
//       F*float((Flags & 0x20) == 0),
//       G*float((Flags & 0x40) == 0),
//       Point*float((Flags & 0x80) == 0);

    if(On > Off){
        //FragColor = vec4(1.0,1.0,0.0,1.0);
        FragColor = vec4(1.0,0.0,1.0,2.5*On-0.5);
        return;
    }
    FragColor = vec4(0.2,0.2,0.2,step(-Off,-0.4));

   


//       float alpha2 = 
//       max(max(max(max(1.0-5.0*length(PosInWorld-AL),
//       1.0-5.0*length(PosInWorld-AR)),
//       max(1.0-5.0*length(PosInWorld-BU),
//       1.0-5.0*length(PosInWorld-BD))),
//       max(max(1.0-5.0*length(PosInWorld-CU),
//       1.0-5.0*length(PosInWorld-CD)),
//       max(1.0-5.0*length(PosInWorld-DL),
//       1.0-5.0*length(PosInWorld-DR)))),
//       max(max(max(1.0-5.0*length(PosInWorld-EU),
//       1.0-5.0*length(PosInWorld-ED)),
//       max(1.0-5.0*length(PosInWorld-FU),
//       1.0-5.0*length(PosInWorld-FD))),
//       max(1.0-5.0*length(PosInWorld-GL),
//       1.0-5.0*length(PosInWorld-GR))));


//   FragColor = vec4(1.0,0.0,1.0,max(1.0-5*length(PosInWorld - AL),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,AL,AR),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,BU,BD),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,CU,CD),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,DL,DR),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,EU,ED),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,FU,FD),0.1));
//   FragColor = vec4(1.0,0.0,1.0,max(1.0-4.0*sdSegment(PosInWorld,GL,GR),0.1));
   //FragColor = vec4(1.0,1.0,1.0,1.0);
}
