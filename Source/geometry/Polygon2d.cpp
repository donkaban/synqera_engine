#include "TheEngine/geometry/Polygon2d.h"
#include "p2t/poly2tri.h"

Polygon2d::Polygon2d(std::vector<std::pair<double,double>> &verts):polygon{0,0,0},strip{0,0}
{
	gpc_vertex_list list={static_cast<int>(verts.size()),reinterpret_cast<gpc_vertex*>(&verts[0])};
	gpc_add_contour(&polygon,&list,0);
	gpc_polygon_to_tristrip(&polygon,&strip);
}

Polygon2d::~Polygon2d()
{
	gpc_free_tristrip(&strip);
	gpc_free_polygon(&polygon);
}

void Polygon2d::addHole(std::vector<std::pair<double,double>> vec)
{
	gpc_vertex_list list={static_cast<int>(vec.size()),reinterpret_cast<gpc_vertex*>(&vec[0])};
	gpc_add_contour(&polygon,&list,1);
	gpc_free_tristrip(&strip);
	gpc_polygon_to_tristrip(&polygon,&strip);
}

void Polygon2d::substract(Polygon2d &p)
{
	gpc_polygon_clip(GPC_DIFF, &polygon, &p.polygon, &polygon);
	gpc_free_tristrip(&strip);
	gpc_polygon_to_tristrip(&polygon,&strip);
}

void Polygon2d::add(Polygon2d &p)
{
	gpc_polygon_clip(GPC_UNION, &polygon, &p.polygon, &polygon);
	gpc_free_tristrip(&strip);
	gpc_polygon_to_tristrip(&polygon,&strip);
}

the::mesh::ptr Polygon2d::createMesh(strref name, float zOffset, bool flipTriangles)
{
	std::vector<std::vector<p2t::Point*>> polylines;
	std::vector<std::vector<p2t::Point*>> holes;

	std::vector<Vertex> engineVerts;
	std::vector<uint16_t> engineIndices;

	for(int i = 0;i<polygon.num_contours;i++)
	{
		std::vector<p2t::Point*> polyline;
		for(int j = 0; j < polygon.contour[i].num_vertices; j++)
			polyline.push_back(new p2t::Point(polygon.contour[i].vertex[j].x, polygon.contour[i].vertex[j].y));
		if(polygon.hole[i]==0)
		{
			polylines.push_back(polyline);
		}
		else
		{
			holes.push_back(polyline);
		}
	}

	for(auto i = 0u; i < polylines.size(); i++)
	{
		std::vector<p2t::Point*> polyline = polylines[i];

		p2t::CDT* cdt = new p2t::CDT(polyline);

		for(auto j = 0u; j < holes.size(); j++)
		{
			cdt->AddHole(holes[j]);
		}

		cdt->Triangulate();
		std::vector<p2t::Triangle*> currentOutput = cdt->GetTriangles();

		for(auto i = 0u; i < currentOutput.size(); i++)
		{
			p2t::Triangle *currentTriangle = currentOutput[i];
			Vertex v1 = {{static_cast<float>(currentTriangle->GetPoint(0)->x),static_cast<float>(currentTriangle->GetPoint(0)->y), zOffset}, {1, 1}, {0, 0, flipTriangles?-1.0f:1.0f}};
			Vertex v2 = {{static_cast<float>(currentTriangle->GetPoint(1)->x),static_cast<float>(currentTriangle->GetPoint(1)->y), zOffset}, {1, 1}, {0, 0, flipTriangles?-1.0f:1.0f}};
			Vertex v3 = {{static_cast<float>(currentTriangle->GetPoint(2)->x),static_cast<float>(currentTriangle->GetPoint(2)->y), zOffset}, {1, 1}, {0, 0, flipTriangles?-1.0f:1.0f}};
			int offset = engineVerts.size();

			int t1 = flipTriangles?1:2;
			int t2 = flipTriangles?2:1;

			engineIndices.push_back(offset+0);
			engineIndices.push_back(offset+t2);
			engineIndices.push_back(offset+t1);

			engineVerts.push_back(v1);
			engineVerts.push_back(v2);
			engineVerts.push_back(v3);
		}
		delete cdt;
	}

	for(auto i = 0u; i < polylines.size(); i++)
	{
		std::vector<p2t::Point*> &polyline = polylines[i];
		for(auto item:polyline)
			delete item;
	}
	for(auto i = 0u; i < holes.size(); i++)
	{
		std::vector<p2t::Point*> &hole = holes[i];
		for(auto item:hole)
			delete item;
	}

	auto g = std::make_shared<the::mesh>();
	g->setTag(name);
	g->addVertex(engineVerts);
	g->addSurface(std::move(the::surface(std::move(engineIndices), "plane")));
	g->bind();
	g->setMaterial(the::material::get("def::base"));
	g->setTexture(0, the::texture::get("def::base"));

	return g;
}