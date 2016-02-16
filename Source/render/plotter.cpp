#include "TheEngine/render/plotter.h"
#include "TheEngine/math/all.h"

the::plotter::plotter() : 
	mat(the::material::get("def::line")),
	pos(mat->attributes[NDX::POSITION]),
	col(mat->attributes[NDX::BONE_ID]),
	mpv(mat->defParameters["mpv"])
{
	logger::debug("[Plotter] create");
	if(pos == -1 || mpv == -1 || col == -1)
		FATAL_ERROR("bad line painter shader");			
}

void the::plotter::addLine(const vec3 &p1, const vec3 &p2, const vec4 &c) 
{
	lines.push_back(p1);
	lines.push_back(p2);
	colors.push_back(c);
	colors.push_back(c);
}

void the::plotter::addPolyLine(const std::initializer_list<vec3> &l,const vec4 &color )
{
	for (auto v = l.begin(); v < l.end()-1; v++)
		addLine(*(v), *(v+1),color);
}
void the::plotter::addPolyLine(const std::vector<vec3> &l,const vec4 &color )
{
	for (auto v = l.begin(); v < l.end()-1; v++)
		addLine(*(v), *(v+1),color);
}
void the::plotter::addBezier(p_t p1, p_t p2, p_t p3, p_t p4, int slice,const c_t c)
{
	float delta = 1.0/slice;
	float t     = 0.0;
	std::vector<vec3> pnt;
	while(t <= 1.0)
	{
		pnt.push_back(the::math::interpolate::bezier(p1, p2, p3, p4, t));
		t+=delta;
	}	
	addPolyLine(pnt,c);
}

void the::plotter::addBasis(p_t origin, p_t basis)
{
	addLine(origin,{origin.x + basis.x, origin.y, origin.z}, vec4(1,0,0,1));
	addLine(origin,{origin.x ,origin.y + basis.y, origin.z}, vec4(0,1,0,1));
	addLine(origin,{origin.x ,origin.y, origin.z + basis.z}, vec4(0,0,1,1));
	addLine(origin,origin+basis, vec4(1,1,1,1));
}

void the::plotter::setTickness(float val)
{
	glLineWidth(val);
}

void the::plotter::clear()                 
{
	lines.clear();
	colors.clear();
} 

void the::plotter::draw(const mat4 & m)
{
	if(lines.empty()) return;
	glUseProgram(mat->getID());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   	glEnableVertexAttribArray(pos);
   	glEnableVertexAttribArray(col);
  	glVertexAttribPointer(pos,3, GL_FLOAT,GL_FALSE,0, &lines[0]);
  	glVertexAttribPointer(col,4, GL_FLOAT,GL_FALSE,0, &colors[0]);
  	glUniformMatrix4fv(mpv, 1,GL_FALSE,m.ptr());
    glDrawArrays(GL_LINES,0, lines.size());
    glDisableVertexAttribArray(pos);
    GL_CHECK("the::plotter::draw");
}
