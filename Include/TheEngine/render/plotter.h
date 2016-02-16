#ifndef _THE_PLOTTER_
#define _THE_PLOTTER_

#include "TheEngine/material.h"
#include "TheEngine/logger.h"
namespace the {
class plotter
{
public:
	using ptr  = std::shared_ptr<plotter>;
    using cref =  const ptr &; 
    using p_t = const vec3 &;
    using c_t = const vec4 &;

	plotter();
	void addLine(p_t, p_t, c_t color = vec4(1,1,0,1)); 
	void addPolyLine(const std::initializer_list<vec3> &,const c_t = vec4(1,0,0,1));
	void addPolyLine(const std::vector<vec3> &,const c_t = vec4(1,0,0,1));
	void addBezier(p_t, p_t, p_t, p_t, int slice = 20,const c_t = vec4(1,0,1,1));
	void addBasis(p_t, p_t);
	void setTickness(float);

	void clear();                 
	void draw(const mat4 &);

private:
	std::vector<vec3> lines;
	std::vector<vec4> colors;
	
	the::material::ptr 	   mat = the::material::get("def::line");
	int pos;
	int col;
	int mpv;

};
}




#endif