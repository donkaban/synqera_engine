#include "TheEngine/render/render.h"
#include "TheEngine/logger.h"
#include "TheEngine/engine.h"


the::renderer::renderer(uint w, uint h) :
    dtTimer(the::timer("render::dt_timer")),
    plotter(std::make_shared<the::plotter>()),
    _width(w),
    _height(h)
{
    logger::info("[Render ] create  %dx%d",_width,_height);
    glViewport(0,0,static_cast<GLuint>(_width),static_cast<GLuint>(_height));
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    GL_CHECK("the::renderer::Render");
}

the::renderer::~renderer() 
{
    logger::info("[Render ] destroy  %dx%d",_width,_height);
    buffers.clear();
}

void the::renderer::resize(uint w, uint h)
{
    _width = w;
    _height = h;
    glViewport(0,0,static_cast<GLuint>(_width),static_cast<GLuint>(_height));
      
}

the::plotter::ptr & the::renderer::getPlotter() {return plotter;}
size_t  the::renderer::getNumTriangles() const {return _triangles;} 

void the::renderer::addBuffer(strref tag,uint w,uint h)
{
    the::fbuffer::addToCache(std::make_shared<the::fbuffer>(w,h), tag);
}


void the::renderer::_opaquePass(the::node::cref node, nodeList blendList )
{
    if(!node->getEnabled()) return;
    if(!node->getBlend()) 
    {
        _triangles+=node->getNumTriangles();
        node->onRender(state);
    }
    else
    {
        auto it = std::find_if(blendList.begin(),blendList.end(),[&](the::node::ptr i) 
        {
            return node->getRenderOrder() < i->getRenderOrder();
        }); 
        blendList.insert(it,node);
    }    
    
    for (const auto & child : node->childs)
        _opaquePass(child,blendList);
}

void the::renderer::_blendPass(the::node::cref node,nodeList blendList)
{
    for(const auto &n : blendList)
    { 
        _triangles+=n->getNumTriangles();
        n->onRender(state);
    }
}

the::image::ptr  the::renderer::renderToImage(strref tag)
{
    auto buff = the::fbuffer::get(tag);
    buff->setCurrent();
    render(aux::engine->getScene(),aux::engine->getRoot());
    auto image = buff->getImage();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,static_cast<GLuint>(_width),static_cast<GLuint>(_height));
 
    return image;
}

void the::renderer::render(the::scene::cref scene, the::node::cref node) 
{    
    std::list<the::node::ptr> blendList; 
    _triangles = 0;
    state.currentTime = the::timer::globalTime();
    scene->onRender(state);
    plotter->draw(state.prjViewMatrix);
    _opaquePass(node,blendList);
    _blendPass(node,blendList);
}