#ifndef _PLATFORM_INPUT_INTERFACE_
#define _PLATFORM_INPUT_INTERFACE_

#include "TheEngine/common.h"
#include "TheEngine/aux.h"
#include "TheEngine/logger.h"
#include "TheEngine/common/taskQueue.h"

#define TAP_THRESHOLD    20
#define DOUBLE_TAP_DELTA 0.3f

namespace platform {

enum KEYBOARD_TYPE
{
    PLAIN_TEXT  = 0,
    NUMBER      = 1,
    MAIL        = 2,
    PHONE       = 3,
    PASSWORD    = 4
};

enum KEY
{
    LETTER      = 0,
    BACKSPACE   = 1,
    TAB         = 2,
    HZ          = 3,
    ESC         = 4,
    UP          = 5,
    DOWN        = 6,
    LEFT        = 7,
    RIGHT       = 8,
};

using  pntHdl = std::function<void(int, int)>;
using  keyHdl = std::function<void(int,strref)>;
using  keyT   = size_t;

class input
{

public:
    using ptr  = std::shared_ptr<input>;
    using cref =  const ptr &; 
  
    struct state_t
    {
        uint mx     {};
        uint my     {}; 
        uint key    {};
        std::string keydata = "";
        bool keypressed     = false;
        bool moved          = false; 
        bool pressed        = false;
        bool released       = false;
        bool empty() {return !(keypressed || moved || pressed || released);}
    } state;


    input();
    ~input();
    
    keyT addKeyHandler(const keyHdl &hdl);    
    keyT addMoveHandler(const pntHdl &hdl);    
    keyT addPressHandler(const pntHdl &hdl);   
    keyT addReleaseHandler(const pntHdl &hdl); 
    keyT addTapHandler(const pntHdl &hdl);    
    keyT addDTapHandler(const pntHdl &hdl);   
 
    void pauseAllMouseHandlers(const std::vector<keyT> &except);
    void pauseAllKeyHandlers(const std::vector<keyT> &except);
    void unpauseAllMouseHandlers();
    void unpauseAllKeyHandlers();

    void delKeyHandler(keyT k);        
    void delMoveHandler(keyT k);         
    void delPressHandler(keyT k);       
    void delReleaseHandler(keyT k);     
    void delTapHandler(keyT k);         
    void delDTapHandler(keyT k);         
 
    void pauseKeyHandler(keyT k);        
    void pauseMoveHandler(keyT k);         
    void pausePressHandler(keyT k);       
    void pauseReleaseHandler(keyT k);     
    void pauseTapHandler(keyT k);         
    void pauseDTapHandler(keyT k);         
 
    void unpauseKeyHandler(keyT k);        
    void unpauseMoveHandler(keyT k);         
    void unpausePressHandler(keyT k);       
    void unpauseReleaseHandler(keyT k);     
    void unpauseTapHandler(keyT k);         
    void unpauseDTapHandler(keyT k);         
 

    void onKey(int key, std::string data);
    void onMove(int x, int y);
    void onPress(int x, int y);
    void onRelease(int x, int y);
    void onTap(int x, int y);
    void onDTap(int x, int y);
 
    int getMouseX() const;
    int getMouseY() const;

    float getNormalizedMouseX(int val) const;
    float getNormalizedMouseY(int val) const ;
    float getNormalizedMouseX() const;
    float getNormalizedMouseY() const;

    vec3  getArcball(float *);

    void clearState();    
    void updateState();
    
private:
    taskQueue<void(int, std::string)>   _keyMap;
    taskQueue<void(int, int)>           _moveMap;
    taskQueue<void(int, int)>           _pressMap;
    taskQueue<void(int, int)>           _releaseMap;
    taskQueue<void(int, int)>           _tapMap;
    taskQueue<void(int, int)>           _dtapMap;

    struct mouse_t
    {
        bool  pressed = false;
        int   X       = 0;
        int   Y       = 0;
        int   lastX   = 0;
        int   lastY   = 0;
        float lastPressedTime = -DOUBLE_TAP_DELTA;
    } mouse ;

};



}
#undef ID
#endif