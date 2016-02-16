#include "TheEngine/openGL.h"
#include "TheEngine/platform/window.h"
#include "TheEngine/logger.h"
#include "TheEngine/math/util.h"


using namespace platform;
     
float  window::fps()        const {return _fps;}
bool   window::fullscreen() const {return _fullscreen;}
uint   window::width()      const {return _width;}
uint   window::height()     const {return _height;}
void   window::width(uint w)      {_width = w;}
void   window::height(uint h)     {_height = h;}

window::~window() 
{
    _close();
    logger::info("[Platform] native window closed!");
}

window::window() : window(640,480,false)
{}


window::window(uint w,uint h, bool fs) :
    _width(w),
    _height(h),
    _fullscreen(fs),
    dt_start(std::chrono::system_clock::now()),
    dt_current(std::chrono::system_clock::now())
       
{
    logger::debug("[Platform] create native window from parameters w: %d; h: %d; fulscreen: %s",w,h,fs ? "true" : "false" );
    _init();
    _resize();
 }   

void window::resize(uint w, uint h)
{
    _width  = w;
    _height = h;
    _resize();
}

float window::_get_dt()
{
    static float _fpstime;
    dt_current = std::chrono::system_clock::now();
    dt = dt_current - dt_start;
    dt_start = dt_current;
    _fpstime += dt.count();
    if(++_frame == fpsframes())
    {
        _fps = fpsframes()/_fpstime; 
        _frame   = 0;
        _fpstime = 0;
    }    
    return dt.count();    
}

void window::setMoveHandler(const pntHdl &&hdl)    {onMouseMove = std::move(hdl);}
void window::setPressHandler(const pntHdl &&hdl)   {onMousePress = std::move(hdl);}
void window::setReleaseHandler(const pntHdl &&hdl) {onMouseRelease = std::move(hdl);}
void window::setKeyHandler(const keyHdl &&hdl)     {onKeyPress = std::move(hdl);}
void window::updateHandler(const updHdl &&hdl)     {onUpdate = std::move(hdl);}
void window::postupdateCall(const updHdl &&hdl)    {onPostUpdate = std::move(hdl);}
void window::resizeHandler(const  sizHdl &&hdl)     {onResize = std::move(hdl);} 
  
