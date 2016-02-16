#ifndef _THE_VECTOR_3_
#define _THE_VECTOR_3_

#include "TheEngine/comtypes.h"
#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
namespace the { namespace math {
template <typename T> class vector3 
{

public:
    using cref = const vector3<T> &; 

    union
    {
        struct { T x, y, z; };
        T v[3];
    };
        
    vector3() : x(0),y(0),z(0) {}
    explicit vector3(const T _x, const T _y,const T _z) : x(_x), y(_y),z(_z)       {}
    vector3(std::initializer_list<float> l) : x(*l.begin()), y(*(l.begin()+1)), z(*(l.begin()+2)) {}
    vector3(const pugi::xml_node &node) {this->deserialize(node);}
    void zero() {x=y=z=0;}

    T  operator[](int index ) const  {return (&x)[index];}
    T & operator[](int index )       {return (&x)[index];}

    vector3 operator-() const {return vector3(-x,-y,-z);}
    T operator*( const vector3 &a ) const {return x * a.x + y * a.y + z * a.z;}
    vector3 operator*( const T a ) const    {return vector3( x * a, y * a, z* a);}
    vector3 operator/( const T a ) const
    {
        T inva = 1.0f / a;
        return vector3(x * inva, y * inva,z * inva);
    }
    vector3   operator+( const vector3 &a ) const {return vector3(x + a.x, y + a.y, z+ a.z);}
    vector3   operator-( const vector3 &a ) const {return vector3(x - a.x, y - a.y, z- a.z);}
    vector3 & operator+=( const vector3 &a )
    {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
    }
    vector3 & operator-=( const vector3 &a )
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
    }
    vector3  & operator/=( const vector3 &a )
    {
        x /= a.x;
        y /= a.y;
        z /= a.z;
        return *this;
    }
    vector3  & operator/=( const T a )
    {
        float inva = 1.0f / a;
        x *= inva;
        y *= inva;
        z *= inva;
        return *this;
    }
    vector3  & operator*=( const T a )
    {
        x *= a;
        y *= a;
        z *= a;
        return *this;
    }
    friend vector3 operator*( const T a, const vector3 b ) {return vector3( b.x * a, b.y * a, b.z *a);}
    bool compare(const vector3 &a) const
    {
        return ((x == a.x) && (y == a.y) && (z == a.z));
    }
    bool compare(const vector3 &a, const T epsilon) const
    {
        if (the::math::abs(x - a.x) > epsilon) return false;
        if (the::math::abs(y - a.y) > epsilon) return false;
        if (the::math::abs(z - a.z) > epsilon) return false;
        return true;
    }
    bool operator==( const vector3 &a ) const {return compare(a);}
    bool operator!=( const vector3 &a ) const {return !compare(a);}
    T length()  const {return (T) std::sqrt(x * x + y * y + z * z);}
    void clamp( const vector3 &min, const vector3 &max )
    {
        if ( x < min.x )        x = min.x;
        else if ( x > max.x )   x = max.x;
        if ( y < min.y )        y = min.y;
        else if ( y > max.y )   y = max.y;
        if ( z < min.z )        z = min.z;
        else if ( z > max.z )   z = max.z;

    }
    void snap()
    {
        x = std::floor( x + 0.5f );
        y = std::floor( y + 0.5f );
        z = std::floor( y + 0.5f );

    }
    const T *   ptr() const {return &x;}
    T *         ptr() {return &x;}
    void  lerp(const vector3 &v1, const vector3 &v2, const float l )
    {
        if ( l <= 0.0 )         (*this) = v1;
        else if ( l >= 1.0 )    (*this) = v2;
        else  (*this) = v1 + l * ( v2 - v1 );
    }

    vector3 cross(const vector3 &a) const
    {
        return vector3(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
    }

    vector3 & cross(const vector3 &a, const vector3 &b)
    {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
        return *this;
    }


  

    vector3 normalize()
    {
        float ilength = the::math::rsqrt(x * x + y * y + z * z);
        x *= ilength; y *= ilength; z *= ilength;
        return *this;
    }

    void serialize(pugi::xml_node &node)
    {
        node.append_attribute("x") = x;
        node.append_attribute("y") = y;
        node.append_attribute("z") = z;
    }
    void deserialize(const pugi::xml_node &node)
    {
        x = node.attribute("x").as_float();
        y = node.attribute("y").as_float();
        z = node.attribute("z").as_float();
    }
    friend std::ostream& operator<<(std::ostream& o, const vector3<T>& a)
    {
        o << "v3[" << a.x << "," << a.y << "," << a.z << "]";
        return o;
    }

};
}}



#endif
