#ifndef _THE_QUATERNION_
#define _THE_QUATERNION_

#include "TheEngine/math/constants.h"
#include "TheEngine/math/util.h"
#include "TheEngine/math/vector3.h"

class quaternion
{
private:	
	float s; 
	vec3  v; 
public:

	quaternion();
	quaternion(float, float, float,  float);
	quaternion(float , const vec3 &);

	const quaternion operator /(float scale) const {return quaternion(s/scale,v/scale);}
	const quaternion operator *(float scale) const {return quaternion(s*scale,v*scale);}
	const quaternion operator *(quaternion q) const {
		quaternion result;
	    result.s = s*q.s - v * q.v;
	    result.v = v.cross(q.v)+s*q.v+q.s*v;
	    return result;
	}
	const quaternion operator +(const quaternion &q) const	{return quaternion(s+q.s, v+q.v);}
	const quaternion operator -(const quaternion &q) const	{return quaternion(s-q.s, v-q.v);}
	const quaternion operator -() const {return quaternion(-s, -v);}

	float length() const ;
	void normalize();
	quaternion normalized() const;
	vec3 euler(bool homogenous=true) const;
	mat4 getMat4();

	void fromAxisAngle(vec3 axis, float angle);
	void fromEuler(float xAngle, float yAngle, float zAngle);
	
	static float dot(const quaternion &q1, const quaternion &q2) { return q1.v*q2.v + q1.s*q2.s; }
	static quaternion lerp(const quaternion &q1, const quaternion &q2, float t) { return (q1*(1-t) + q2*t).normalized(); }
	static quaternion slerp(const quaternion &q1, const quaternion &q2, float t); 

	void serialize(pugi::xml_node &);
    void deserialize(const pugi::xml_node &);
	
	friend std::ostream& operator<<(std::ostream& o, const quaternion& a)
    {
        o << "quaternion";
        return o;
    }	

};
#endif 