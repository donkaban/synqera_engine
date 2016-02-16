#ifndef _PLATFORM_WINDOW_INTERFACE_
#define _PLATFORM_WINDOW_INTERFACE_

#include "TheEngine/comtypes.h"

namespace platform {

using uint   = unsigned int;
using ulong  = unsigned long;
using pntHdl = std::function<void(int, int)>;
using keyHdl = std::function<void(int, strref)>;
using updHdl = std::function<void(float)>; 
using sizHdl = std::function<void(uint, uint)>;

class window
{

 public:
    using ptr  = std::shared_ptr<window>;
    using cref =  const ptr &; 
    
    window(window const &) = delete;
    window & operator = (window const &) = delete;

    window();
    window(uint, uint, bool fs = false);

    virtual ~window();
   
    bool   update();
   
    float  fps()        const;
    bool   fullscreen() const;
    uint   width()      const; 
    uint   height()     const; 

  
    void   width(uint);
    void   height(uint);
    void   resize(uint ,uint);
  
    void   updateHandler(const updHdl &&hdl);
    void   postupdateCall(const updHdl &&hdl);
   
    void   resizeHandler(const sizHdl &&hdl);
    void   setKeyHandler(const keyHdl &&hdl);
    void   setPressHandler(const pntHdl &&hdl);
    void   setReleaseHandler(const pntHdl &&hdl);
    void   setMoveHandler(const pntHdl &&hdl);

private:
    uint  _width  = 0;
    uint  _height = 0;
    
    bool  _fullscreen = false;
    int   _frame  = 0;
    float _fps    = 0; 
    
    std::string _title = "theEngine";
    
    timepoint dt_start;
    timepoint dt_current;

    std::chrono::duration<float> dt;
    std::chrono::duration<float> cut_dt;
    
    keyHdl onKeyPress       {};
    pntHdl onMousePress     {};
    pntHdl onMouseRelease   {};
    pntHdl onMouseMove      {};
    pntHdl onMouseTap       {};
    pntHdl onMouseDoubleTap {};
    updHdl onUpdate         {};
    updHdl onPostUpdate     {};
    sizHdl onResize         {};
    
    static constexpr uint sleeptick()  {return 5;}
    static constexpr int  fpsframes()  {return 30;}  

    void  _init();       
    void  _resize();     
    void  _close();      
    float _get_dt();  
};
}



#endif  
