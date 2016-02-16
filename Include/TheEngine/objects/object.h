#ifndef _THE_OBJECT_
#define _THE_OBJECT_

#include "TheEngine/common.h"
#include "TheEngine/surface.h"
#include "TheEngine/objects/node.h"
#include "TheEngine/render/rstate.h"
#include "TheEngine/geometry/BoundingBox.h"
#include "TheEngine/geometry/Intersection.h"
#include "TheEngine/behavior/Action.h"

namespace the {

class object : 
    public the::node, 
    public serialize::binary,
    public serialize::xml
{
    THE_SHARED(object)
    friend class Render;
public:
    object();
    object(strref);
    object(strref, strref);
    
    virtual ~object(); 
   
    virtual void bind()                                 override;  
    virtual void unbind()                               override;
    virtual void log()                                  override;
    virtual void serialize(pugi::xml_node &)            override;    
    virtual void deserialize(const pugi::xml_node &)    override;
    virtual void serialize(strref)                      override;
    virtual void deserialize(the::filesystem::stream &) override;
    virtual void onRender(render::state &)              override;
    virtual void load(strref);
    virtual size_t getNumTriangles() override;

    std::vector<Vertex>& getVertices();

    void clear();
    uint getNumSurfaces () const;
   
    void addVertex  (const Vertex  &);
    void addVertex  (const std::initializer_list<Vertex> &); 
    void addVertex  (const std::vector<Vertex> &); 
    
    void addSurface (the::surface &&); 
    void addSurface (const std::initializer_list<the::surface> &); 
    void addSurface (const std::vector<the::surface> &);

    the::surface & getSurface(int);
    the::surface & getSurface(strref);
    int       getSurfaceId(strref);

    void setTexture(int,   int, the::texture::cref);
    void setTexture(int,   int, strref);
    void setTexture(strref,int, the::texture::cref);
    void setTexture(strref,int, strref);
    void setTexture(int, the::texture::cref);
    void setTexture(int, strref);

    void setCubemap(int, int, the::cubetex::cref);
    void setCubemap(strref, int, the::cubetex::cref);
    void setCubemap(int, int, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6);
    void setCubemap(strref, int, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6);
    void setCubemap(int, strref t1, strref t2,strref t3,strref t4,strref t5,strref t6);
    
    void setMaterial(int, the::material::cref);
    void setMaterial(int, strref);
    void setMaterial(strref, the::material::cref);
    void setMaterial(strref, strref);
    void setMaterial(the::material::cref);
    void setMaterial(strref);
    
    void setBlending(int   , bool);
    void setBlending(strref, bool);
    
    void setMaterialParam(strref , strref , float );
    void setMaterialParam(strref , strref , const vec4 & );
    
    void setMaterialParam(int , strref , float );
    void setMaterialParam(int , strref , const vec4 & );
  
    void orderSort();

    void addAction(strref, strref, Action::cbRef);
    void addAction(strref, int, Action::cbRef);
   
    void addTapAction(strref, strref, Action::cbRef);
    void addTapAction(strref, int, Action::cbRef);
   
    void removeAction(strref);
    void removeAllActions();

    void setSurfaceEnabled(strref, bool);
    void setSurfaceEnabled(int, bool);
    void setSurfaceEnabled(bool);

    std::vector<size_t> getAllHandlersId();
    
    virtual std::vector<Intersection> getIntersection(vec3 p0, vec3 p1);
    BoundingBox getBoundingBox(); 
    void forEachSurface(std::function<void(the::surface &)>);   

    void computeTBN();
    void computeTBN(the::surface &);
   
protected:
    std::vector<Vertex>  vertexes;
    std::vector<the::surface> surfaces;
    dictionary<size_t>   tags;
    size_t               triangles = 0;
    struct header
    {
        int id;
        int vertexSize;
        int surfaceSize;
    };
  
    BoundingBox boundingBox;
    void recalculateBoundingBox();
    void remap();

    dictionary<Action>    actions;
   
    size_t mousePressHandler = 0;
    size_t mouseTapHandler   = 0;

    void checkAction(const Action::type &type, int x, int y);
    void addAction(strref, Action &&);
    void initHandlers();


};
}
#endif