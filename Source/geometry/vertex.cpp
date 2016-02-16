#include "TheEngine/geometry/vertex.h"

const size_t Vertex::SIZE[] = 
    {
       /* POSITION */  sizeof(position)/sizeof(float),
       /* TEXCOORD */  sizeof(texcoord)/sizeof(float),
       /* NORMAL   */  sizeof(normal)/sizeof(float),
       /* BONE_ID  */  sizeof(boneId)/sizeof(float),
       /* BONE_WGT */  sizeof(weight)/sizeof(float),
       /* BONE_CNT */  sizeof(bonesCount)/sizeof(int),
       /* TANGENT  */  sizeof(tangent)/sizeof(float),
       /* BINORMAL */  sizeof(binormal)/sizeof(float),
    };    
const size_t Vertex::OFFSET[] =
    {
       /* POSITION */  0,
       /* TEXCOORD */  sizeof(position),
       /* NORMAL   */  sizeof(position) + sizeof(texcoord),
       /* BONE_ID  */  sizeof(position) + sizeof(texcoord) + sizeof(normal),
       /* BONE_WGT */  sizeof(position) + sizeof(texcoord) + sizeof(normal) + sizeof(boneId), 
       /* BONE_CNT */  sizeof(position) + sizeof(texcoord) + sizeof(normal) + sizeof(boneId) + sizeof(weight) ,
       /* TANGENT  */  sizeof(position) + sizeof(texcoord) + sizeof(normal) + sizeof(boneId) + sizeof(weight) + sizeof(bonesCount),
       /* BINORMAL */  sizeof(position) + sizeof(texcoord) + sizeof(normal) + sizeof(boneId) + sizeof(weight) + sizeof(bonesCount) + sizeof(tangent),
    }; 