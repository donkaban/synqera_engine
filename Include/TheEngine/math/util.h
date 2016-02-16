#ifndef _THE_MATH_ALGORITHM_
#define _THE_MATH_ALGORITHM_

#include "TheEngine/math/constants.h"
#include <cstddef>
namespace the { namespace math {

float  rnd();
int    abs  (int);    
float  abs  (float);  
double abs  (double); 
float  rsqrt(float); 
float  sqrt (float);
float  sin  (float);
float  cos  (float); 
float  acos (float); 
float  asin (float); 
float  atan (float); 
int    floor(float); 
float  atan2(float,float); 
bool   isPowerOfTwo(int);   
float  radians(float deg); 
float  degrees(float rad); 


template <typename T> T max(const T &x, const T &y) {return (x > y) ? x : y; }
template <typename T> T min(const T &x, const T &y) {return (x < y) ? x : y; }
template <typename T> T sign(const T &f) {return ( f > 0 ) ? 1 : ((f < 0) ? -1 : 0 );}


template <typename T>
void replace(T& src, const T& find, const T& replace)
{
    size_t find_lenght = find.size();
    size_t rep_lenght  = replace.size();
    for(size_t pos = 0; (pos=src.find(find, pos))!=T::npos; pos+=rep_lenght) 
        src.replace(pos, find_lenght, replace);
}

template<typename T> inline bool between(const T &v, const T &v0, const T &v1)
{
    return (v >= v0 && v <= v1);
}     

template<typename T> bool compare(T v0, T v1)
{
    return (abs(v0 - v1) < EPSILON);
}

template<typename T> T clamp(T v,T v0, T v1)
{
    if(v < v0) return v0;
    if(v > v1) return v1;
    return v;
}

template<typename T> T lerp(const T &v0,const T &v1,float t)
{
    return v0 + (v1 - v0) * t;
}
namespace interpolate
{
    template<typename T> T linear(const T &v0,const T &v1,float t)
    {
        return v0 + (v1 - v0) * t;
    }

    template<typename T> T cosine(const T& v0, const T& v1, float t)
    {
        auto t2 = (1.0 - cos(t * PI)) / 2.0;
        return v0 * (1.0 - t2) + v1 * t2;
    }

    template<typename T> T cubic(const T& a, const T& b, const T& c, const T& d, float t)
    {
        auto t2 = t * t;
        auto a0 = d - c - a + b;
        auto a1 = a - b - a0;
        auto a2 = c - a;
        auto a3 = b;
        return(a0 * t * t2 + a1 * t2 + a2 * t + a3);
    }

    template<typename T> T bezier(const T& a, const T& b, const T& ak, const T& bk, float t)
    {
        auto t2 = (1.0 - t) * (1.0 - t);       
        auto t3 = (1.0 - t) * t2;
        auto Ak = ak+a;
        auto Bk = bk+b;
        return t3 * a + 3 * t2 * t * Ak + 3*(1-t) * t * t * Bk + t*t*t*b;

    }




}}}

#endif
