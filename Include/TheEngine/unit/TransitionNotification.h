#ifndef _THE_UNIT_NOTIFICATION_H_
#define _THE_UNIT_NOTIFICATION_H_

#include "TheEngine/common.h"

class TransitionNotification
{
private:
	int frame;
	std::function<void(int)> callback;
public:
	inline int getFrame() { return frame;}
	inline void invoke() { if(callback) callback(frame);}
	TransitionNotification(int frame,const  std::function<void(int)> &callback): frame(frame), callback(callback) {}
	TransitionNotification(){}
	~TransitionNotification(){}
};

#endif 