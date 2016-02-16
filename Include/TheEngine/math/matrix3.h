#ifndef _THE_MATRIX_3_
#define _THE_MATRIX_3_

#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
#include "TheEngine/math/vector3.h"
#include "TheEngine/math/matrix4.h"


class mat3 
{
public:
    using cref = const mat3 &; 
    union
    {
        vec3 mat[3];
        struct
        {
            float xx,xy,xz;
            float yx,yy,yz;
            float zx,zy,zz;
        };
    };   
    mat3() : xx(0),xy(0),xz(0),yx(0),yy(0),yz(0),zx(0),zy(0),zz(0) {}
    mat3(const pugi::xml_node &node) {this->deserialize(node);}
    explicit mat3(const vec3 &x, const vec3 &y, const vec3 &z)
    {
        mat[0] = x;
        mat[1] = y;
        mat[2] = z;
    }
    explicit mat3(const float _xx, const float _xy, const float _xz,
                         const float _yx, const float _yy, const float _yz,
                         const float _zx, const float _zy, const float _zz)
    :xx(_xx),xy(_xy),xz(_xz),
     yx(_yx),yy(_yy),yz(_yz),
     zx(_zx),zy(_zy),zz(_zz)
    {
    }
    explicit mat3(const mat4 & m):
        xx(m.xx),xy(m.xy),xz(m.xz),
        yx(m.yx),yy(m.yy),yz(m.yz),
        zx(m.zx),zy(m.zy),zz(m.zz)
    {
    }
    explicit mat3(const float src[3][3]) {std::memcpy(mat,src, 9 * sizeof(float));}
    
    const vec3 & operator[](int index) const {return mat[index];}
    vec3 & operator[](int index) {return mat[index];}

    mat3 operator*(const float a) const
    {
        return mat3(xx * a, xy * a, xz * a, 
                    yx * a, yy * a, yz * a, 
                    zx * a, zy * a, zz * a);
    }

    vec3 operator*(const vec3 &vec) const
    {
        return vec3(xx * vec.x + xy * vec.y + xz * vec.z, 
                    yx * vec.x + yy * vec.y + yz * vec.z,
                    zx * vec.x + zy * vec.y + zz * vec.z);
    }
 
    mat3 operator+(const mat3 &a) const
    {
        return mat3(xx + a.xx, xy + a.xy, xz + a.xz, 
                    yx + a.yx, yy + a.yy, yz + a.yz, 
                    zx + a.zx, zy + a.zy, zz + a.zz); 
    }

    mat3 operator-(const mat3 &a) const
    {
        return mat3(xx - a.xx, xy - a.xy, xz - a.xz,
                    yx - a.yx, yy - a.yy, yz - a.yz,
                    zx - a.zx, zy - a.zy, zz - a.zz);
    }

    mat3 &operator*=(const float a)
    {
        xx *= a; xy *= a; xz *= a; 
        yx *= a; yy *= a; yz *= a; 
        zx *= a; zy *= a; zz *= a; 
        return *this;
    }

    

    mat3 &operator+=( const mat3 &a )
    {
        xx += a.xx; xy += a.xy; xz += a.xz; 
        yx += a.yx; yy += a.yy; yz += a.yz; 
        zx += a.zx; zy += a.zy; zz += a.zz; 
        return *this;
    }

    mat3 &operator-=( const mat3 &a )
    {
        xx -= a.xx; xy -= a.xy; xz -= a.xz;
        yx -= a.yx; yy -= a.yy; yz -= a.yz;
        zx -= a.zx; zy -= a.zy; zz -= a.zz;
        return *this;
    }


    friend mat3  operator*(const float a,  const mat3 &mat )    {return mat * a;}
    friend vec3  operator*(const vec3 &vec, const mat3 &mat )    {return mat * vec;}
    friend vec3 &operator*=(vec3 &vec, const mat3 &mat )         {vec = mat * vec;return vec;}

    bool compare( const mat3 &a ) const
    {
        const float *p1 = reinterpret_cast<const float *>(mat);
        const float *p2 = reinterpret_cast<const float *>(a.mat);
        for (int  i = 0; i < 3*3; i++ )
            if (p1[i] != p2[i]) return false;
        return true;
    }

    bool compare( const mat3 &a, const float epsilon) const
    {
        const float *p1 = reinterpret_cast<const float *>(mat);
        const float *p2 = reinterpret_cast<const float *>(a.mat);
        for (int  i = 0; i < 3*3; i++ )
            if (the::math::abs(p1[i] - p2[i]) > the::math::EPSILON) return false;
        return true;
    }

    bool operator==(const mat3 &a ) const {return compare(a);}
    bool operator!=(const mat3 &a ) const {return !compare(a);};

    void zero() {std::memset(mat,0,sizeof(mat3));}

    bool isSymmetric(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        for (int i = 1; i < 3; i++ )
            for (int j = 0; j < i; j++ )
                if (the::math::abs(mat[i][j] - mat[j][i]) > epsilon )
                    return false;
        return true;
    }

    bool isDiagonal(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                if(i != j && the::math::abs(mat[i][j]) > epsilon)
                    return false;
        return true;
    }
    
    float trace() const {return (xx + yy + zz);}

    mat3 transpose() const 
    {
    return mat3(mat[0][0], mat[1][0], mat[2][0],
                mat[0][1], mat[1][1], mat[2][1],
                mat[0][2], mat[1][2], mat[2][2]);
    }

    mat3 &transposeSelf() 
    {
        float tmp0, tmp1, tmp2;
        tmp0 = mat[0][1]; mat[0][1] = mat[1][0];
        mat[1][0] = tmp0; tmp1 = mat[0][2];
        mat[0][2] = mat[2][0]; mat[2][0] = tmp1;
        tmp2 = mat[1][2]; mat[1][2] = mat[2][1];
        mat[2][1] = tmp2;
        return *this;
    }


    const float *ptr() const {return &xx;}
    float *ptr() {return &xx;}
   
    void serialize(pugi::xml_node &);
    void deserialize(const pugi::xml_node &);

    friend std::ostream& operator<<(std::ostream& o, const mat3& a)
    {
        o << "mat3(" << a.mat[0] << ",\n" << a.mat[1] << ",\n" << a.mat[2] << ")";
        return o;
    }
};



#endif
