#include "TheEngine/platform/input.h"
#include "TheEngine/timer.h"
#include "TheEngine/engine.h"

namespace platform {

input::input() :
    _keyMap     ("onKey"),
    _moveMap    ("onMove"),
    _pressMap   ("onPress"),
    _releaseMap ("onRelease"),
    _tapMap     ("onTap"),
    _dtapMap    ("onDoubleTap")
{
    logger::info("[Input  ] create instance");
}

input::~input()
{
    logger::info("[Input  ] destroy instance");
}

keyT input::addKeyHandler(const keyHdl &hdl)     {return _keyMap.add(hdl);}
keyT input::addMoveHandler(const pntHdl &hdl)    {return _moveMap.add(hdl);}
keyT input::addPressHandler(const pntHdl &hdl)   {return _pressMap.add(hdl);}
keyT input::addReleaseHandler(const pntHdl &hdl) {return _releaseMap.add(hdl);}
keyT input::addTapHandler(const pntHdl &hdl)     {return _tapMap.add(hdl);}
keyT input::addDTapHandler(const pntHdl &hdl)    {return _dtapMap.add(hdl);}

void input::delKeyHandler(keyT k)          		 {_keyMap.del(k);}
void input::delMoveHandler(keyT k)         		 {_moveMap.del(k);}
void input::delPressHandler(keyT k)        		 {_pressMap.del(k);}
void input::delReleaseHandler(keyT k)      		 {_releaseMap.del(k);}
void input::delTapHandler(keyT k)          		 {_tapMap.del(k);}
void input::delDTapHandler(keyT k)         		 {_dtapMap.del(k);}

void input::pauseKeyHandler(keyT k)              {_keyMap.pause(k);}
void input::pauseMoveHandler(keyT k)             {_moveMap.pause(k);}
void input::pausePressHandler(keyT k)            {_pressMap.pause(k);}
void input::pauseReleaseHandler(keyT k)          {_releaseMap.pause(k);}
void input::pauseTapHandler(keyT k)              {_tapMap.pause(k);}
void input::pauseDTapHandler(keyT k)             {_dtapMap.pause(k);}

void input::unpauseKeyHandler(keyT k)            {_keyMap.unpause(k);}
void input::unpauseMoveHandler(keyT k)           {_moveMap.unpause(k);}
void input::unpausePressHandler(keyT k)          {_pressMap.unpause(k);}
void input::unpauseReleaseHandler(keyT k)        {_releaseMap.unpause(k);}
void input::unpauseTapHandler(keyT k)            {_tapMap.unpause(k);}
void input::unpauseDTapHandler(keyT k)           {_dtapMap.unpause(k);}


void input::onKey(int key,std::string data)  
{
	_keyMap.emit(key,data).update();
}

void input::onMove(int x, int y)
{
    mouse.lastX = x;
    mouse.lastY = y;
	if(TAP_THRESHOLD<std::abs(mouse.X-x) || TAP_THRESHOLD<std::abs(mouse.Y-y))
    	mouse.pressed = false;
    _moveMap.emit(x,y).update();	
}

void input::onPress(int x, int y)
{
	mouse.pressed = true;
	mouse.X = x;
	mouse.Y = y;
    mouse.lastX = x;
    mouse.lastY = y;
    _pressMap.emit(x,y).update();   
}
void input::onRelease(int x, int y)
{
    mouse.lastX = x;
    mouse.lastY = y;
	if(mouse.pressed
		&& TAP_THRESHOLD>=std::abs(mouse.X-x)
		&& TAP_THRESHOLD>=std::abs(mouse.Y-y))
	{
    	auto curTime = the::timer::globalTime();
    	if(curTime-mouse.lastPressedTime < DOUBLE_TAP_DELTA)
    	{
        	onDTap(x,y);
        	mouse.lastPressedTime = -DOUBLE_TAP_DELTA;
    	}
    	else
        	mouse.lastPressedTime = curTime;
		onTap(x,y);
	}

	mouse.pressed = false;
	_releaseMap.emit(x,y).update();  
}
void input::onTap(int x, int y)     
{
    _tapMap.emit(x,y).update();      
}
void input::onDTap(int x, int y)    
{
    _dtapMap.emit(x,y).update();   
}

void input::clearState()
{
 	state.keydata = "";
    state.keypressed = false;
    state.moved      = false; 
    state.pressed    = false;
    state.released   = false;
}
  
void input::updateState()
{
	if(state.empty()) 		 return;
	if(state.keypressed) 	 onKey(state.key,state.keydata);
	if(state.pressed)    	 onPress(state.mx, state.my);
    if(state.moved)          onMove(state.mx, state.my);
	if(state.released)       onRelease(state.mx, state.my);
	clearState();
}


void input::pauseAllMouseHandlers(const std::vector<keyT> & except)
{
    _moveMap.pauseAll();
    _pressMap.pauseAll();
    _releaseMap.pauseAll();
    _tapMap.pauseAll();
    _dtapMap.pauseAll();
    for(auto k: except)
    {
        if(_moveMap.check(k))    _moveMap.unpause(k);
        if(_pressMap.check(k))   _pressMap.unpause(k);
        if(_releaseMap.check(k)) _releaseMap.unpause(k);
        if(_tapMap.check(k))     _tapMap.unpause(k);
        if(_dtapMap.check(k))    _dtapMap.unpause(k);
    }
}

void input::unpauseAllMouseHandlers()
{
    _moveMap.unpauseAll();
    _pressMap.unpauseAll();
    _releaseMap.unpauseAll();
    _tapMap.unpauseAll();
    _dtapMap.unpauseAll();
}

void input::pauseAllKeyHandlers(const std::vector<keyT> &except)
{
    _keyMap.pauseAll();
    for(auto k: except)
    {
        if(_keyMap.check(k)) _keyMap.unpause(k);
    }
}

void input::unpauseAllKeyHandlers()
{
    _keyMap.unpauseAll();
}

int input::getMouseX() const {return mouse.lastX;}
int input::getMouseY() const {return mouse.lastY;}

float input::getNormalizedMouseX(int val) const {return static_cast<float>(val) / static_cast<float>(aux::engine->getViewport().x);}
float input::getNormalizedMouseY(int val) const {return static_cast<float>(val) / static_cast<float>(aux::engine->getViewport().y);}
float input::getNormalizedMouseX()        const {return getNormalizedMouseX(mouse.lastX);}
float input::getNormalizedMouseY()        const {return getNormalizedMouseY(mouse.lastY);}

static vec3 get_arcball_vector(int x, int y) 
{
    auto screen = aux::engine->getViewport();
    vec3 P = vec3(1.0 * x/screen.x * 2 - 1.0,
                  1.0 * y/screen.y * 2 - 1.0,
                    0);
    P.y = -P.y;
    float OP_squared = P.x * P.x + P.y * P.y;
    if (OP_squared <= 1*1)
        P.z = sqrt(1*1 - OP_squared);  
    else
        P = P.normalize(); 
    return P;
}

vec3 input::getArcball(float *angle)
{
    vec3 va = get_arcball_vector(mouse.lastX, mouse.lastY);
    vec3 vb = get_arcball_vector(mouse.X, mouse.Y);
    *angle = acos(the::math::min(1.0f, va *  vb));
    return va.cross(vb);
}    

}

#undef TAP_THRESHOLD