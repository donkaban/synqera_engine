#ifndef _THE_MATRIX_4_
#define _THE_MATRIX_4_

#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
#include "TheEngine/math/vector3.h"
#include "TheEngine/math/vector4.h"
#include "TheEngine/logger.h"


class mat4 
{
public:
    using cref = const mat4 &; 

    union
    {
        vec4 mat[4];
        struct
        {
            float xx,xy,xz,xw;
            float yx,yy,yz,yw;
            float zx,zy,zz,zw;
            float wx,wy,wz,ww;
        };
    };
    mat4() :
     xx(1),xy(0),xz(0),xw(0),
     yx(0),yy(1),yz(0),yw(0),
     zx(0),zy(0),zz(1),zw(0),
     wx(0),wy(0),wz(0),ww(1) {}
    
    mat4(const pugi::xml_node &node) {this->deserialize(node);}
  
    explicit mat4(const vec4 &x, const vec4 &y, const vec4 &z, const vec4 &w )
    {
        mat[0] = x;
        mat[1] = y;
        mat[2] = z;
        mat[3] = w;
    }
    explicit mat4(const float _xx, const float _xy, const float _xz, const float _xw,
                         const float _yx, const float _yy, const float _yz, const float _yw,
                         const float _zx, const float _zy, const float _zz, const float _zw,
                         const float _wx, const float _wy, const float _wz, const float _ww )
    :xx(_xx),xy(_xy),xz(_xz),xw(_xw),
     yx(_yx),yy(_yy),yz(_yz),yw(_yw),
     zx(_zx),zy(_zy),zz(_zz),zw(_zw),
     wx(_wx),wy(_wy),wz(_wz),ww(_ww)
    {
    }
   
    explicit mat4(const float src[4][4]) {std::memcpy(mat,src, 16 * sizeof(float));}

    const vec4 & operator[](int index) const {return mat[index];}
    vec4 & operator[](int index) {return mat[index];}

    mat4 operator*(const float a) const
    {
        return mat4(xx * a, xy * a, xz * a, xw * a,
                    yx * a, yy * a, yz * a, yw * a,
                    zx * a, zy * a, zz * a, zw * a,
                    wx * a, wy * a, wz * a, ww * a);
    }

    vec4 operator*(const vec4 &vec) const
    {
        return vec4(xx * vec.x + xy * vec.y + xz * vec.z + xw * vec.w,
                    yx * vec.x + yy * vec.y + yz * vec.z + yw * vec.w,
                    zx * vec.x + zy * vec.y + zz * vec.z + zw * vec.w,
                    wx * vec.x + wy * vec.y + wz * vec.z + ww * vec.w );
    }

    vec3 operator*(const vec3 &vec) const
    {
        float s = wx * vec.x + wy * vec.y + wz * vec.z + ww;
        if (s == 0.0)
            return vec3(0,0,0);
        if (s == 1.0)
            return vec3(xx * vec.x + xy * vec.y + xz * vec.z + xw,
                        yx * vec.x + yy * vec.y + yz * vec.z + yw,
                        zx * vec.x + zy * vec.y + zz * vec.z + zw );
        else
        {
            float invS = 1.0/s;
            return vec3((xx * vec.x + xy * vec.y + xz * vec.z + xw) * invS,
                        (yx * vec.x + yy * vec.y + yz * vec.z + yw) * invS,
                        (zx * vec.x + zy * vec.y + zz * vec.z + zw) * invS);
        }
    }


    mat4 operator*(const mat4 &a) const
    {
        mat4   dst;
        const float *m1  = reinterpret_cast<const float *>(this);
        const float *m2  = reinterpret_cast<const float *>(&a);
        float *dstPtr = reinterpret_cast<float *>(&dst);
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                *dstPtr = m1[0] * m2[0 * 4 + j]
                        + m1[1] * m2[1 * 4 + j]
                        + m1[2] * m2[2 * 4 + j]
                        + m1[3] * m2[3 * 4 + j];
                dstPtr++;
            }
            m1 += 4;
        }
        return dst;
    }

    mat4 operator+(const mat4 &a) const
    {
        return mat4(xx + a.xx, xy + a.xy, xz + a.xz, xw + a.xw,
                    yx + a.yx, yy + a.yy, yz + a.yz, yw + a.yw,
                    zx + a.zx, zy + a.zy, zz + a.zz, zw + a.zw,
                    wx + a.wx, wy + a.wy, wz + a.wz, ww + a.ww );
    }

    mat4 operator-(const mat4 &a) const
    {
        return mat4(xx - a.xx, xy - a.xy, xz - a.xz, xw - a.xw,
                    yx - a.yx, yy - a.yy, yz - a.yz, yw - a.yw,
                    zx - a.zx, zy - a.zy, zz - a.zz, zw - a.zw,
                    wx - a.wx, wy - a.wy, wz - a.wz, ww - a.ww );
    }

    mat4 &operator*=(const float a)
    {
        xx *= a; xy *= a; xz *= a; xw *= a;
        yx *= a; yy *= a; yz *= a; yw *= a;
        zx *= a; zy *= a; zz *= a; zw *= a;
        wx *= a; wy *= a; wz *= a; ww *= a;
        return *this;
    }

    mat4 &operator*=(const mat4 &a)
    {
        *this = (*this) * a;
        return *this;
    }

    mat4 &operator+=( const mat4 &a )
    {
        xx += a.xx; xy += a.xy; xz += a.xz; xw += a.xw;
        yx += a.yx; yy += a.yy; yz += a.yz; yw += a.yw;
        zx += a.zx; zy += a.zy; zz += a.zz; zw += a.zw;
        wx += a.wx; wy += a.wy; wz += a.wz; ww += a.ww;
        return *this;
    }
    mat4 &operator-=( const mat4 &a )
    {
        xx -= a.xx; xy -= a.xy; xz -= a.xz; xw -= a.xw;
        yx -= a.yx; yy -= a.yy; yz -= a.yz; yw -= a.yw;
        zx -= a.zx; zy -= a.zy; zz -= a.zz; zw -= a.zw;
        wx -= a.wx; wy -= a.wy; wz -= a.wz; ww -= a.ww;
        return *this;
    }


    friend mat4  operator*(const float a,  const mat4 &mat )    {return mat * a;}
    friend vec4  operator*(const vec4 &vec, const mat4 &mat )    {return mat * vec;}
    friend vec3  operator*(const vec3 &vec, const mat4 &mat )    {return mat * vec;}

    friend vec4 &operator*=(vec4 &vec, const mat4 &mat )         {vec = mat * vec;return vec;}
    friend vec3 &operator*=(vec3 &vec, const mat4 &mat )         {vec = mat * vec;return vec;}

    bool compare( const mat4 &a ) const
    {
        const float *p1 = reinterpret_cast<const float *>(mat);
        const float *p2 = reinterpret_cast<const float *>(a.mat);
        for (int  i = 0; i < 4*4; i++ )
            if (p1[i] != p2[i]) return false;
        return true;
    }

    bool compare( const mat4 &a, const float epsilon) const
    {
        const float *p1 = reinterpret_cast<const float *>(mat);
        const float *p2 = reinterpret_cast<const float *>(a.mat);
        for (int  i = 0; i < 4*4; i++ )
            if (the::math::abs(p1[i] - p2[i]) > the::math::EPSILON) return false;
        return true;
    }

    bool operator==(const mat4 &a ) const {return compare(a);}
    bool operator!=(const mat4 &a ) const {return !compare(a);};

    void zero() {std::memset(mat,0,sizeof(mat4));}

    bool isIdentity(const float epsilon = the::math::MATRIX_EPSILON ) const  {return compare(mat4_identity,epsilon);}
    bool isSymmetric(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        for (int i = 1; i < 4; i++ )
            for (int j = 0; j < i; j++ )
                if (the::math::abs(mat[i][j] - mat[j][i]) > epsilon )
                    return false;
        return true;
    }

    bool isDiagonal(const float epsilon = the::math::MATRIX_EPSILON ) const
    {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                if(i != j && the::math::abs(mat[i][j]) > epsilon)
                    return false;
        return true;
    }

    bool isRotated() const
    {
        if(!mat[0][1] && !mat[0][2] && !mat[1][0] && !mat[1][2] && !mat[2][0] && !mat[2][1])
            return false;
        return true;
    }

    void projectVector(const vec4 &src, vec4 &dst ) const
    {
        dst.x = src * mat[0];
        dst.y = src * mat[1];
        dst.z = src * mat[2];
        dst.w = src * mat[3];
    }

    void unprojectVector(const vec4 &src, vec4 &dst ) const
    {
        dst = mat[0] * src.x + mat[1] * src.y + mat[2] * src.z + mat[3] * src.w;
    }

    float trace() const {return (xx + yy + zz + ww);}

    float determinant() const
    {
        float det2_01_01 = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
        float det2_01_02 = mat[0][0] * mat[1][2] - mat[0][2] * mat[1][0];
        float det2_01_03 = mat[0][0] * mat[1][3] - mat[0][3] * mat[1][0];
        float det2_01_12 = mat[0][1] * mat[1][2] - mat[0][2] * mat[1][1];
        float det2_01_13 = mat[0][1] * mat[1][3] - mat[0][3] * mat[1][1];
        float det2_01_23 = mat[0][2] * mat[1][3] - mat[0][3] * mat[1][2];
        float det3_201_012 = mat[2][0] * det2_01_12 - mat[2][1] * det2_01_02 + mat[2][2] * det2_01_01;
        float det3_201_013 = mat[2][0] * det2_01_13 - mat[2][1] * det2_01_03 + mat[2][3] * det2_01_01;
        float det3_201_023 = mat[2][0] * det2_01_23 - mat[2][2] * det2_01_03 + mat[2][3] * det2_01_02;
        float det3_201_123 = mat[2][1] * det2_01_23 - mat[2][2] * det2_01_13 + mat[2][3] * det2_01_12;
        return ( - det3_201_123 * mat[3][0] + det3_201_023 * mat[3][1] - det3_201_013 * mat[3][2] + det3_201_012 * mat[3][3] );
    }

    mat4  transpose() const
    {
        mat4 t;
        for(int i = 0; i < 4; i++ )
            for(int j = 0; j < 4; j++ )
                    t[i][j] = mat[j][i];
        return t;
    }

    mat4 &transposeSelf()
    {
        float  temp;
        for(int i = 0; i < 4; i++ )
            for(int j = i + 1; j < 4; j++ )
            {
                temp = mat[ i ][ j ];
                mat[i][j] = mat[j][i];
                mat[j][i] = temp;
            }
        return *this;
    }

    bool  inverseSelf();

    mat4 inverse() const
    {
        mat4 invMat = *this;
        invMat.inverseSelf();
        return invMat;
    }


    const float *ptr() const {return &xx;}
    float *ptr() {return &xx;}

    
    static mat4 rotateX(float angle)
    {
        float sin = the::math::sin(angle);
        float cos = the::math::cos(angle);
        return mat4(1,  0,   0,  0,
                    0, cos,-sin, 0,
                    0, sin, cos, 0,
                    0,  0,   0,  1);
    }

    static mat4 rotateY(float angle)
    {
        float sin = the::math::sin(angle);
        float cos = the::math::cos(angle);
        return mat4(cos, 0, sin, 0,
                     0,  1,  0,  0,
                   -sin, 0, cos, 0,
                     0,  0,  0,  1);
    }

    static mat4 rotateZ(float angle)
    {
        float s = the::math::sin(angle);
        float c = the::math::cos(angle);
        return mat4(c, -s, 0, 0,
                    s,  c, 0, 0,
                    0,  0, 1, 0,
                    0,  0, 0, 1);
    }

    static mat4 scale(const vec3 &v)
    {
        return mat4(v.x, 0,   0,  0,
                    0,  v.y,  0,  0,
                    0,   0,  v.z, 0,
                    0,   0,  0,   1);
    }

    static mat4 translate(const vec3 &v)
    {
        return mat4(1,  0,  0,  v.x,
                    0,  1,  0,  v.y,
                    0,  0,  1,  v.z,
                    0,  0,  0,   1);
    }

    static const mat4 &identity() {return mat4_identity;}

    static mat4 frustum(float l, float r, float b, float t, float n, float f)
    {
        float  dx = r - l;
        float  dy = t - b;
        float  dz = f - n;

        if((n <= 0.0f) || (f <= 0.0f) || (dx <= 0.0f) || (dx <= 0.0f) || (dx <= 0.0f))
            FATAL_ERROR("frustum creation with bad parameters");

        return mat4(
        2.0*n/dx,       0,      (r+l)/dx,       0,
            0,      2.0*n/dy,   (t+b)/dy,       0,
            0,          0,     -(f+n)/dz,  -2.0*f*n/dz,
            0,          0,         -1,          0);
    }

    static mat4 perspective(float fov,float aspect,float n,float f)
    {
        float h = std::tan(fov * the::math::DEG2RAD * .5);
        float w = h * aspect;
        float dz = f-n;
        if((n <= 0.0f) || (f <= 0.0f) || (dz <= 0.0f))
            FATAL_ERROR("perspective creation with bad parameters");

        return mat4(
        1.0/w,  0,      0,      0,
        0,      1.0/h,  0,      0,
        0,      0,  -(f+n)/dz,-2*f*n/dz,
        0,      0,     -1,      0);
    }

    static mat4 lookAt(const vec3 &eye, const vec3 &direction, const vec3 up)
    {
        vec3 z = (eye-direction).normalize();
        vec3 y = up.cross(z).normalize();
        vec3 x = z.cross(y).normalize();

        return mat4(x.x,    x.y,    x.z,    0,
                    y.x,    y.y,    y.z,    0,
                    z.x,    z.y,    z.z,    0,
                     0,      0,      0,     1) * mat4::translate(-direction);
    }
    
    static mat4 rotate(const vec3 & axis, float angle) 
    {
        float s = the::math::sin(angle);
        float c = the::math::cos(angle);
        vec3 v(axis);
        v.normalize();
        float xx = v.x * v.x;
        float yy = v.y * v.y;
        float zz = v.z * v.z;
        float xy = v.x * v.y;
        float yz = v.y * v.z;
        float zx = v.z * v.x;
        float xs = v.x * s;
        float ys = v.y * s;
        float zs = v.z * s;
        float cc = 1.0f - c;
        return mat4(cc * xx + c,    cc * xy - zs,   cc * zx + ys,   0,
                    cc * xy + zs,   cc * yy + c,    cc * yz - xs,   0,
                    cc * zx - ys,   cc * yz + xs,   cc * zz + c,    0,
                         0,               0,             0,         1);
}


  
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    vec3 getPosition() const {return vec3(mat[0].w,mat[1].w,mat[2].w);}
    vec3 getScale()    const {return vec3(mat[0].x,mat[1].y,mat[2].z);}
    vec3 getEuler()    const
    {
        float heading, attitude, bank;
        if (mat[1][0] > 0.998) { // singularity at north pole
            heading = std::atan2(mat[0][2],mat[2][2]);
            attitude = the::math::PI/2;
            bank = 0;
        }
        else if (mat[1][0] < -0.998) { // singularity at south pole
            heading = std::atan2(mat[0][2],mat[2][2]);
            attitude = -the::math::PI/2;
            bank = 0;
        }
        else {
            heading = std::atan2(-mat[2][0],mat[0][0]);
            bank = std::atan2(-mat[1][2],mat[1][1]);
            attitude = std::asin(mat[1][0]);
        }
        return vec3(bank,heading,attitude);
    }


    const mat4 & setPosition(const vec3 &v)
    {
        mat[0].w = v.x;
        mat[1].w = v.y;
        mat[2].w = v.z;
        return *this;
    }

    const mat4 & setScale(const vec3 &v)
    {
        mat[0].x = v.x;
        mat[1].y = v.y;
        mat[2].z = v.z;
        return *this;
    }
   

    void serialize(pugi::xml_node &);
    void deserialize(const pugi::xml_node &);

    friend std::ostream& operator<<(std::ostream& o, const mat4& a)
    {
        o << "m4[" << a.mat[0] << " " << a.mat[1] << " " << a.mat[2] << " " << a.mat[3] << "]";
        return o;
    }

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif


