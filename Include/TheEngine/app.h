#ifndef _THE_ENGINE_APPLICATION
#define _THE_ENGINE_APPLICATION

#include "TheEngine/engine.h"
namespace the {

class app
{
public:
	using ptr = std::shared_ptr<app>;
	void bind()
	{
		logger::debug("[Bundle] bind event callbacks");
		using std::placeholders::_1;
    	using std::placeholders::_2;
		engine = the::engine::get();
		engine->addInitHandler  (std::bind(&app::onInit, this));
		engine->addExitHandler  (std::bind(&app::onExit, this));
		engine->addUpdateHandler(std::bind(&app::onUpdate, this,_1));

		engine->getInput()->addMoveHandler(std::bind(&app::onMouseMove, this,_1,_2));
		engine->getInput()->addPressHandler(std::bind(&app::onMousePress, this,_1,_2));
		engine->getInput()->addReleaseHandler(std::bind(&app::onMouseRelease, this,_1,_2));
	}


	~app()
	{}
	virtual void onInit()                   {};
	virtual void onExit()                   {};
	virtual void onUpdate(float)            {};
	virtual void onMousePress(uint, uint)   {};
	virtual void onMouseMove(uint, uint)    {};
	virtual void onMouseRelease(uint, uint) {};
	
protected:
	the::engine::ptr engine;
};




}
#endif