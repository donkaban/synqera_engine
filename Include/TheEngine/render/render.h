#ifndef _THE_RENDER_
#define _THE_RENDER_

#include "TheEngine/common.h"
#include "TheEngine/math/all.h"
#include "TheEngine/timer.h"
#include "TheEngine/objects/node.h"
#include "TheEngine/render/scene.h"
#include "TheEngine/render/plotter.h"
#include "TheEngine/render/fbuffer.h"
namespace the {
class renderer
{
public:
    using ptr  = std::shared_ptr<renderer>;
    using cref =  const ptr &; 
    
    using nodeList = std::list<the::node::ptr> &;

    renderer(uint, uint);
    virtual ~renderer(); 
   
    the::plotter::ptr     & getPlotter();
   
    void   render(the::scene::cref,the::node::cref);

    size_t      getNumTriangles() const; 
    void        addBuffer(strref,uint,uint);
    the::image::ptr  renderToImage(strref);
    void        resize(uint w, uint h);
   

private:
    render::state         state;
    the::timer            dtTimer;
    the::plotter::ptr     plotter;
    dictionary<the::fbuffer::ptr> buffers;
   
    uint   _width     = 800;
    uint   _height    = 800;
    size_t _triangles = 0;

    void _opaquePass(the::node::cref, nodeList);
    void _blendPass (the::node::cref, nodeList);
};
}

#endif
