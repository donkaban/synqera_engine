#ifndef _MATH_CONSTANTS_H_
#define _MATH_CONSTANTS_H_

namespace the { namespace math {
    extern const float EPSILON                 ;
    extern const float MATRIX_INVERSE_EPSILON  ;
    extern const float MATRIX_EPSILON          ;
    extern const float PI                      ;
    extern const float TWO_PI                  ;
    extern const float HALF_PI                 ;
    extern const float SQRT_TWO                ;
    extern const float SQRT_THREE              ;
    extern const float DEG2RAD                 ;
    extern const float RAD2DEG                 ;
    extern const float INF                     ;

    template <typename T> class vector2;
    template <typename T> class vector3;
    template <typename T> class vector4;

}}

using ivec2 =  the::math::vector2<int>;     
using ivec3 =  the::math::vector3<int>;     
using ivec4 =  the::math::vector4<int>;     

using bvec2 =  the::math::vector2<bool>;
using bvec3 =  the::math::vector3<bool>;
using bvec4 =  the::math::vector4<bool> ;

using vec2  =  the::math::vector2<float>;
using vec3  =  the::math::vector3<float>;
using vec4  =  the::math::vector4<float>;

extern const vec2 vec2_zero;
extern const vec3 vec3_zero;
extern const vec4 vec4_zero;

class quaternion;
class mat2;
class mat3;
class mat4;

extern const mat2 mat2_zero;
extern const mat2 mat2_identity;

extern const mat3 mat3_zero;
extern const mat3 mat3_identity;

extern const mat4 mat4_zero;
extern const mat4 mat4_identity;


#endif
