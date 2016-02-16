#ifndef _THE_VECTOR_2_
#define _THE_VECTOR_2_

#include "TheEngine/comtypes.h"
#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"

namespace the { namespace math {
template <typename T> class vector2
{
public:
   using cref = const vector2<T> &; 

    union
     {
         struct { T x, y; };
         struct { T min, max; };
         T v[2];
     };
    vector2(): x(0),y(0) {}
    vector2(std::initializer_list<float> l) : x(*l.begin()), y(*(l.begin()+1)) {}
    vector2(const pugi::xml_node &node) {this->deserialize(node);}
    explicit vector2(const T _x, const T _y) : x(_x), y(_y) {}

    void zero() {x=y=0;}

    T operator[](int index ) const  {return (&x)[index];}
    T & operator[](int index )        {return (&x)[index];}

    vector2 operator-() const {return vector2(-x,-y);}
    T operator*( const vector2 &a ) const {return x * a.x + y * a.y;}
    vector2 operator*( const T a ) const    {return vector2( x * a, y * a );}
    vector2 operator/( const T a ) const
    {
        T inva = 1.0f / a;
        return vector2(x * inva, y * inva );
    }
    vector2   operator+( const vector2 &a ) const {return vector2( x + a.x, y + a.y );}
    vector2   operator-( const vector2 &a ) const {return vector2(x - a.x,y - a.y);}
    vector2 & operator+=( const vector2 &a )
    {
        x += a.x;
        y += a.y;
        return *this;
    }
    vector2 & operator-=( const vector2 &a )
    {
        x -= a.x;
        y -= a.y;
        return *this;
    }
    vector2  & operator/=( const vector2 &a )
    {
        x /= a.x;
        y /= a.y;
        return *this;
    }
    vector2  & operator/=( const T a )
    {
        float inva = 1.0f / a;
        x *= inva;
        y *= inva;
        return *this;
    }
    vector2  & operator*=( const T a )
    {
        x *= a;
        y *= a;
        return *this;
    }
    friend vector2 operator*( const T a, const vector2 b ) {return vector2( b.x * a, b.y * a );}
    bool compare(const vector2 &a) const
    {
        return ((x == a.x) && (y == a.y));
    }
    bool compare(const vector2 &a, const T epsilon) const
    {
        if (the::math::abs(x - a.x) > epsilon) return false;
        if (the::math::abs(y - a.y) > epsilon) return false;
        return true;
    }
    bool operator==( const vector2 &a ) const {return compare(a);}
    bool operator!=( const vector2 &a ) const {return !compare(a);}
    T length()  const {return (T) the::math::sqrt(x * x + y * y);}
    void clamp( const vector2 &min, const vector2 &max )
    {
        if ( x < min.x )        x = min.x;
        else if ( x > max.x )   x = max.x;
        if ( y < min.y )        y = min.y;
        else if ( y > max.y )   y = max.y;
    }
    void snap()
    {
        x = the::math::floor( x + 0.5f );
        y = the::math::floor( y + 0.5f );
    }
    const T *   ptr() const {return &x;}
    T *         ptr() {return &x;}
    void  lerp(const vector2 &v1, const vector2 &v2, const float l)
    {
        if ( l <= 0.0 )         (*this) = v1;
        else if ( l >= 1.0 )    (*this) = v2;
        else  (*this) = v1 + l * ( v2 - v1 );
    }

    T normalize()
    {
        float sqrLength, invLength;
        sqrLength = x * x + y * y;
        invLength = 1.0/the::math::rsqrt(sqrLength);
        x *= invLength;
        y *= invLength;
        return invLength * sqrLength;
    }

    void deserialize(const pugi::xml_node &node)
    {
        x = node.attribute("x").as_float();
        y = node.attribute("y").as_float();
    }
 
    friend std::ostream& operator<<(std::ostream& o, const vector2<T>& a)
    {
        o << "v2[" << a.x << "," << a.y << "]";
        return o;
    }
};
}}







#endif