#ifndef _THE_NODE_
#define _THE_NODE_

#include "TheEngine/common.h"
#include "TheEngine/render/rstate.h"
#include "TheEngine/math/all.h"
#include "TheEngine/logger.h"

namespace the {

class node : 
    public abstract::object,
    public abstract::gapi,
    public serialize::xml, 
    public std::enable_shared_from_this<node> 
{
    THE_SHARED(node)
public:
  
    static ptr load(const pugi::xml_node &, strref defpath = "");
    static ptr load(strref,strref defpath = "");

    std::vector<ptr> childs;    
    
    node();
    node(strref);
    node(strref,strref);
  
    virtual ~node(); 
     
   
    virtual void serialize(pugi::xml_node &)          override;
    virtual void deserialize(const pugi::xml_node &)  override;
   
    virtual void bind()      override {}  
    virtual void unbind()    override {} 
    virtual void log();  
   
    virtual void orderSort() {}
    virtual void update()    {}

    virtual void onRender(render::state &);
    virtual ptr  getThis();
    virtual size_t getNumTriangles() {return 0;}

    void   foreachChild(const std::function<void(cref)> &);
    void   addChild(cref);
    void   removeChild(cref);
    void   setParent(cref);

    bool   getEnabled()        const;
    bool   getBlend()          const;
    int    getRenderOrder()    const;
    int    getNumChilds()      const;
    strref getDefaultPath()    const;
    ptr    getParent()         const; 
    ptr    getChild(int) ;
    ptr    getChild(strref);
    
    void   setEnabled(bool); 
    void   setRenderOrder(int);
    void   setBlend(bool);
    void   setDefaultPath(strref);

    const mat4 & getLocalTransform() const;
    const mat4 & getWorldTransform() const;

    mat4 getWorldTransformCopy() const;
    mat4 getLocalTransformCopy() const;

    vec3   getLocalPosition()  const;
    vec3   getWorldPosition()  const;
    vec3   getEuler() const;    
   
    void   setLocalTransform(const mat4 &); 
    void   setWorldTransform(const mat4 &); 
    void   setLocalPosition(const vec3 &);  
    void   setWorldPosition(const vec3 &); 
    void   setScale(const vec3 &); 
    vec3   getScale(); 

    void   rotateX(float);     
    void   rotateY(float);     
    void   rotateZ(float);     
    void   rotate(const vec3 &); 
    void   rotate(const vec3 &, float);  
    void   translate(const vec3 &); 
    void   scale(const vec3 &);

    template <typename T>
    static std::shared_ptr<T> ptr_cast(cref node) {return std::dynamic_pointer_cast<T>(node);}
    friend std::ostream &operator<<(std::ostream &o, const node &a) {o << a.tostring();return o;}
  
private:
    std::weak_ptr<node> parent;
    
    mat4 localTransform = mat4_identity;
    mat4 worldTransform = mat4_identity;
      
    void localTraversal();
    void worldTraversal();

    std::string defaultPath = "";
    bool  _enabled  = true;
    bool  _blending = false; 
    int   _order    = 0;
 
    static ptr  _firstPass (const pugi::xml_node &, std::unordered_map<uint, ptr> &, strref defpath = "");
    static void _secondPass(const pugi::xml_node &, std::unordered_map<uint, ptr> &);
};
}


#endif 
