#ifndef _THE_TRIANGLE_
#define _THE_TRIANGLE_

#include "TheEngine/common.h"
#include "TheEngine/geometry/Plane.h"
#include "TheEngine/geometry/vertex.h"


class Triangle
{
private:
    struct IntersectPosition
    {
        int type;
        vec3 position;
        vec3 normal;
        vec2 uv;
    };

    IntersectPosition segmentPlaneIntersect(Vertex v1, Vertex v2, Plane p)
    {
        vec3    u = v2.position - v1.position;
        vec3    w = v1.position - p.v0;

        float     D = p.n*u;
        float     N = -(p.n*w);

        if (fabs(D) < the::math::EPSILON) {
            if (N == 0)
                return {2,vec3(0,0,0),vec3(0,0,0), vec2(0,0)};
            else
                return {0,vec3(0,0,0),vec3(0,0,0), vec2(0,0)};
        }
        float sI = N / D;
        if (sI < 0 || sI > 1)
            return {0,vec3(0,0,0),vec3(0,0,0), vec2(0,0)};

        vec3 out = v1.position + sI * u;
        float len1 = u.length();
        float len2 = (out-v1.position).length();
        return {1,out,the::math::lerp<vec3>(v1.normal,v2.normal,len2/len1).normalize(), the::math::lerp<vec2>(v1.texcoord,v2.texcoord,len2/len1)};
    }

    int getNumBound(Plane p)
    {
        int bnd = 0;

        for(uint i =0;i<vertexes.size();i++)
        {
            vec3    w = vertexes[i].position - p.v0;
            float     N = p.n*w;
            if(N > 0.0f)
                bnd++;
        }

        return bnd;
    }
    std::vector<Vertex> vertexes;
public:
    Triangle(Vertex v1, Vertex v2, Vertex v3)
    {
        vertexes.insert(vertexes.begin(),{v1,v2,v3});
    }

    std::vector<Vertex> getIntersected(Plane p)
    {
        IntersectPosition res12 = segmentPlaneIntersect(vertexes[0],vertexes[1],p);
        IntersectPosition res23 = segmentPlaneIntersect(vertexes[1],vertexes[2],p);
        IntersectPosition res31 = segmentPlaneIntersect(vertexes[2],vertexes[0],p);

        std::vector<Vertex> verts;

        int cutCnt = getNumBound(p);

        if(cutCnt == 1)
        {
            std::vector<IntersectPosition> ipos = {res12,res23,res31};
            int idxNone = find_if(ipos.begin(),ipos.end(), [] (const IntersectPosition &e) {return e.type != 1.0;}) - ipos.begin();
            int idx2 = (idxNone + 1)%3;
            int idx3 = (idxNone + 2)%3;

            verts.push_back(vertexes[(idxNone+2)%3]);
            verts.push_back(Vertex(ipos[idx3].position, ipos[idx3].uv, ipos[idx3].normal));
            verts.push_back(Vertex(ipos[idx2].position, ipos[idx3].uv, ipos[idx3].normal));

            
        }

        if(cutCnt == 2)
        {
            std::vector<IntersectPosition> ipos = {res12,res23,res31};
            int idxNone = find_if(ipos.begin(),ipos.end(), [] (const IntersectPosition &e) {return e.type != 1.0;}) - ipos.begin();
            int idx2 = (idxNone + 1)%3;
            int idx3 = (idxNone + 2)%3;


            verts.push_back(vertexes[(idxNone+1)%3]);
            verts.push_back(Vertex(ipos[idx2].position, ipos[idx2].uv, ipos[idx2].normal));
            verts.push_back(Vertex(ipos[idx3].position, ipos[idx3].uv, ipos[idx3].normal));

            verts.push_back(Vertex(ipos[idx3].position, ipos[idx3].uv, ipos[idx3].normal));
            verts.push_back(vertexes[(idxNone)%3]);
            verts.push_back(vertexes[(idxNone+1)%3]);
        }

        if(cutCnt == 3)
        {
            verts.insert(verts.begin(),vertexes.begin(),vertexes.end());
        }

        return verts;
    }

};

#endif