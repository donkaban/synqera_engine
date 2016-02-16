#ifndef _THE_POLYGON_2D_
#define _THE_POLYGON_2D_

#include "TheEngine/common.h"
#include "TheEngine/objects/mesh.h"

extern "C"
{
#include "gpc/gpc.h"
}

class Polygon2d
{
protected:
	gpc_polygon polygon;
	gpc_tristrip strip;

public:
	Polygon2d(std::vector<std::pair<double,double>> &vec);
	virtual ~Polygon2d();

	void addHole(std::vector<std::pair<double,double>> vec);

	void substract(Polygon2d &p);
	void add(Polygon2d &p);

	the::mesh::ptr createMesh(strref name, float zOffset=0, bool flipTriangles = false);
};

#endif