#ifndef _THE_VECTOR_4_
#define _THE_VECTOR_4_

#include "TheEngine/comtypes.h"
#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
namespace the { namespace math {


template <class T> class vector4 
{
public:
    using cref = const vector4<T> &; 

    union
    {
        struct { T x, y, z, w; };
        struct { T r, g, b, a; };
    };
       
    vector4() : x(0),y(0),z(0),w(0) {}
    explicit vector4(const T _x, const T _y,const T _z, const T _w = 1) : x(_x), y(_y),z(_z), w(_w)  {}
    vector4(std::initializer_list<float> l) : x(*l.begin()), y(*(l.begin()+1)), z(*(l.begin()+2)), w(*(l.begin()+3)) {}
    vector4(const pugi::xml_node &node) {this->deserialize(node);}
    T   operator[](int index ) const  {return (&x)[index];}
    T & operator[](int index )        {return (&x)[index];}

    void zero() {x=y=z=w=0;}

    vector4 operator-() const {return vector4(-x,-y,-z,-w);}
    T operator*( const vector4 &a ) const {return x * a.x + y * a.y + z * a.z + w * a.w;}
    vector4 operator*( const T a ) const    {return vector4( x * a, y * a, z * a, w * a);}
    vector4 operator/( const T a ) const
    {
        T inva = 1.0f / a;
        return vector4(x * inva, y * inva,z * inva, w * inva);
    }
    vector4   operator+(const vector4 &a ) const {return vector4(x + a.x, y + a.y, z+ a.z, w + a.w);}
    vector4   operator-(const vector4 &a ) const {return vector4(x - a.x, y - a.y, z- a.z, w - a.w);}

    vector4 & operator+=(const vector4 &a )
    {
        x += a.x;
        y += a.y;
        z += a.z;
        w += a.w;
        return *this;
    }
    vector4 & operator-=( const vector4 &a )
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        w -= a.w;
        return *this;
    }
    vector4  & operator/=( const vector4 &a )
    {
        x /= a.x;
        y /= a.y;
        z /= a.z;
        w /= a.w;
        return *this;
    }
    vector4  & operator/=( const T a )
    {
        float inva = 1.0f / a;
        x *= inva;
        y *= inva;
        z *= inva;
        w *= inva;
        return *this;
    }
    vector4  & operator*=( const T a )
    {
        x *= a;
        y *= a;
        z *= a;
        w *= a;
        return *this;
    }
    bool compare(const vector4 &a) const
    {
        return ((x == a.x) && (y == a.y) && (z == a.z) && (w == a.w));
    }
    bool compare(const vector4 &a, const T epsilon) const
    {
        if (the::math::abs(x - a.x) > epsilon) return false;
        if (the::math::abs(y - a.y) > epsilon) return false;
        if (the::math::abs(z - a.z) > epsilon) return false;
        if (the::math::abs(w - a.w) > epsilon) return false;
        return true;
    }
    bool operator==( const vector4 &a ) const {return compare(a);}
    bool operator!=( const vector4 &a ) const {return !compare(a);}
    T length()  const {return (T) the::math::sqrt(x * x + y * y + z * z + w * w);}

    friend vector4 operator*( const T a, const vector4 b )
    {
        return vector4( b.x * a, b.y * a, b.z * a, b.w * a);
    }

    void clamp( const vector4 &min, const vector4 &max )
    {
        if ( x < min.x )        x = min.x;
        else if ( x > max.x )   x = max.x;
        if ( y < min.y )        y = min.y;
        else if ( y > max.y )   y = max.y;
        if ( z < min.z )        z = min.z;
        else if ( z > max.z )   z = max.z;
        if ( w < min.w )        w = min.w;
        else if ( w > max.w )   w = max.w;
    }
    void snap()
    {
        x = the::math::floor( x + 0.5f );
        y = the::math::floor( y + 0.5f );
        z = the::math::floor( y + 0.5f );
        w = the::math::floor( y + 0.5f );
    }
    const T *   ptr() const {return &x;}
    T *         ptr() {return &x;}
    void  lerp(const vector4 &v1, const vector4 &v2, const float l )
    {
        if ( l <= 0.0 )         (*this) = v1;
        else if ( l >= 1.0 )    (*this) = v2;
        else  (*this) = v1 + l * ( v2 - v1 );
    }

   void serialize(pugi::xml_node &node)
    {
        node.append_attribute("x") = x;
        node.append_attribute("y") = y;
        node.append_attribute("z") = z;
        node.append_attribute("w") = w;
    }
    void deserialize(const pugi::xml_node &node)
    {
        x = node.attribute("x").as_float();
        y = node.attribute("y").as_float();
        z = node.attribute("z").as_float();
        w = node.attribute("w").as_float();
    }

    void serializeColor(pugi::xml_node &node)
    {
        node.append_attribute("r") = x * 255.0;
        node.append_attribute("g") = y * 255.0;
        node.append_attribute("b") = z * 255.0;
        node.append_attribute("a") = w * 255.0;
    }

    void deserializeColor(const pugi::xml_node &node)
    {
        x = node.attribute("r").as_float() / 255.0;
        y = node.attribute("g").as_float() / 255.0;
        z = node.attribute("b").as_float() / 255.0;
        w = node.attribute("a").as_float() / 255.0;
    }

    friend std::ostream& operator<<(std::ostream& o, const vector4<T>& a)
    {
        o << "v4[" << a.x << "," << a.y << "," << a.z << "," << a.w << "]";
        return o;
    }

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}}

#endif
