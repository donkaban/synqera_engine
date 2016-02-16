#ifndef _THE_UNIT_TRANSITION_
#define _THE_UNIT_TRANSITION_

#include "TheEngine/common.h"
#include "TheEngine/unit/TransitionNotification.h"


class Transition : 
	public abstract::object 
{
private:
	bool 			looped  	= false;
	std::string 	audio   	= "none";
	std::string 	animation 	= "none";
	callable 		callback  	= callable();
	std::unordered_map<int,TransitionNotification> notiffications;

public:
   
	Transition(strref animation, strref audio, bool looped, callable callback = callable()):
		looped(looped),
		audio(audio),
		animation(animation),
		callback(callback)
	{}

	Transition() {}

	inline strref getAudio()     const  {return audio;}
	inline strref getAnimation() const 	{return animation;}

	inline void setAudio(strref a)		{audio = a;}
	inline void setAnimation(strref a)	{animation = a;}

	inline bool isLooped() {return looped;}
	inline void setLoop(bool mode) {looped = mode;}
	inline bool hasCallback() {return callback != 0;}

	inline void invoke() {if(callback) callback();}

	inline void addNotification(TransitionNotification t) {notiffications[t.getFrame()] = t;}
	inline void removeNotification(int frame) {notiffications.erase(frame);}
	inline void removeNotification(TransitionNotification t) {removeNotification(t.getFrame());}

	inline void update(int frame)
	{
		auto it = notiffications.find(frame);
		if(it!=notiffications.end())
			it->second.invoke();
	}
	
	inline void setCallback(callable callback) {this->callback = callback;}
	
	friend std::ostream &operator<<(std::ostream &o, const Transition &a) {o << a.tostring();return o;}


};

typedef std::shared_ptr<Transition> TransitionPtr;

#endif 