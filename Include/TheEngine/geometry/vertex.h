#ifndef _THE_VERTEX_
#define _THE_VERTEX_

#include "TheEngine/math/all.h"
#include "TheEngine/logger.h"

namespace NDX { enum 
{
    POSITION = 0,
    TEXCOORD,
    NORMAL,
    BONE_ID,
    BONE_WEIGHT,
    BONE_COUNT,
    TANGENT,
    BINORMAL,
    VERTEX_SIZE = 8
};}


class Vertex
{
public:
    using cref = const Vertex &;
  
    vec3 position   = vec3_zero;  // [0] position
    vec2 texcoord   = vec2_zero;  // [1] texture coordinates
    vec3 normal     = vec3_zero;  // [2] normal
    vec4 boneId     = vec4_zero;  // [3] bone id's 
    vec4 weight     = vec4_zero;  // [4] bone weights 
    int  bonesCount = 0;          // [5] num of bones 
    vec3 tangent    = vec3_zero;  // [6] tangent 
    vec3 binormal   = vec3_zero;  // [7] binormal

   
    static const size_t SIZE[];
    static const size_t OFFSET[];
     
    Vertex() : Vertex(vec3_zero, vec2_zero, vec3_zero) 
    {}
    Vertex(vec3::cref p) : Vertex(p, vec2_zero, vec3_zero) 
    {}
    Vertex(vec3::cref p, vec2::cref t) : Vertex(p, t, vec3_zero)
    {}
    Vertex(vec3::cref p, vec2::cref t, vec3::cref n) : 
        position(p), 
        texcoord(t), 
        normal(n),
        weight(vec4(1,0,0,0))
    {}

    float   operator[] (const int index) const {return ((float *)(&position))[index];};
    float & operator[] (const int index)       {return ((float *)(&position))[index];};

    friend bool operator== (cref v1, cref v2) {return v1.position == v2.position;}
    friend bool operator!= (cref v1, cref v2) {return !(v1 == v2);}

    const float * ptr() const {return position.ptr();}
    float       * ptr()       {return position.ptr();}

    bool setBoneData(int bone, float w)  
    {
        bonesCount++;
        if(w==0)
            FATAL_ERROR("[Vertex] Null weight");
        if(bonesCount > 4) return false;
        boneId[bonesCount-1] = bone;
        weight[bonesCount-1] = w;
        return true;
    }

    ~Vertex(){}

};

#endif 
