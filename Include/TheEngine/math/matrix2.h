#ifndef _THE_MATRIX_2_H_
#define _THE_MATRIX_2_H_

#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
#include "TheEngine/math/vector2.h"

class mat2
{
 public:
    union
    {
        struct
        {
            float xx,xy;
            float yx,yy;
        };
        vec2 mat[2];
    };

   mat2() {};
   virtual ~mat2() {}

    explicit mat2(const vec2 &x, const vec2 &y)
    {
       xx = x.x; xy = x.y;
       yx = y.x; yy = y.y;
    }
    explicit mat2(const float _xx, const float _xy, const float _yx, const float _yy )
         : xx(_xx), xy(_xy), yx(_yx), yy(_yy)
    {
    }
    explicit mat2(const float src[2][2])
    {
        std::memcpy(mat,src,2*2*sizeof(float));
    }

    const vec2 &  operator[](int index) const {return mat[index];}
    vec2 & operator[](int index) {return mat[index];}

    mat2 operator-() const {return mat2(-xx,-xy,-yx,-yy);}

    mat2 operator*(const mat2 &a) const
    {
        return mat2(xx * a.xx + xy * a.yx,xx * a.xy + xy * a.yy,
                    yx * a.xx + yy * a.yx,yx * a.xy + yy * a.yy);
    }

    vec2 operator*(const vec2 &vec) const
    {
        return vec2(xx * vec.x + xy * vec.y,yx * vec.x + yy * vec.y );
    }

    mat2 operator*(const float a) const
    {
        return mat2(xx * a, xy * a, yx * a, yy * a);
    }

    mat2 operator+(const mat2 &a) const
    {
        return mat2(xx + a.xx, xy + a.xy,yx + a.yx, yy + a.yy);
    }
    mat2 operator-(const mat2 &a) const
    {
        return mat2(xx - a.xx, xy - a.xy,yx - a.yx, yy - a.yy);
    }

    mat2 & operator*=(const float a)
    {
       xx *= a; xy *= a;
       yx *= a; yy *= a;
       return *this;
    }
    mat2 & operator*=(const mat2 &a)
    {
        float x = xx;
        float y = xy;
        xx = x * a.xx + y * a.yx;
        xy = x * a.xy + y * a.yy;
        x = yx; y = yy;
        yx = x * a.xx + y * a.yx;
        yy = x * a.xy + y * a.yy;
        return *this;
    }

    mat2 & operator+=(const mat2 &a)
    {
        xx += a.xx; xy += a.xy;
        yx += a.yx; yy += a.yy;
        return *this;
    }
    mat2 & operator-=(const mat2 &a)
    {
        xx -= a.xx; xy -= a.xy;
        yx -= a.yx; yy -= a.yy;
        return *this;
    }

    friend mat2   operator*(const float a, const mat2 &mat)   {return mat * a;};
    friend vec2   operator*(const vec2 &vec, const mat2 &mat)  {return mat * vec;};
    friend vec2 & operator*=(vec2 &vec, const mat2 &mat)       {vec = mat * vec; return vec;}


    bool compare(const mat2 &a) const
    {
        if (mat[0].compare(a[0]) && mat[1].compare(a[1]))
            return true;
        return false;
    }
    bool compare(const mat2 &a, const float epsilon ) const
    {
        if (mat[0].compare(a[0],epsilon ) && mat[1].compare(a[1],epsilon ))
            return true;
        return false;
    }
    bool operator==(const mat2 &a) const {return compare(a);}
    bool operator!=(const mat2 &a) const {return !compare(a);}

    void zero() {mat[0].zero();mat[1].zero();}

    bool isIdentity(const float epsilon = the::math::MATRIX_EPSILON ) const {return compare(mat2_identity,epsilon);}
    bool isSymmetric(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        return (std::fabs(xy - yx) < epsilon);
    }
    bool isDiagonal(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        if (std::fabs(xy) > epsilon || std::fabs(yx) > epsilon )
            return false;
        return true;
    }

    float  trace() const {return xx + yy;}
    float  determinant() const {return xx * yy - xy * yx;}

    mat2 & transposeSelf()
    {
        float tmp = yx;
        xy = yx;
        yx = tmp;
        return *this;
    }
    mat2  transpose() {return mat2(xx, yx,xy, yy);}

    bool   inverseSelf();
    mat2 inverse() const
    {
        mat2 invMat = *this;
        invMat.inverseSelf();
        return invMat;
    }

    const float *  ptr() const {return &xx;}
    float * ptr() {return &xx;}

    friend std::ostream& operator<<(std::ostream& o, const mat2& a)
    {
        o << "mat2(" << a.mat[0] << ",\n" << a.mat[1] << ")";
        return o;
    }
};




#endif