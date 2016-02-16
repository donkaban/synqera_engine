#ifdef SOUND_NULL 

#include "TheEngine/resource/sound.h" 

the::sound::sound(strref f)
{
	_audio_data = nullptr;
}
the::sound::~sound()		
{}

void the::sound::init() {}
void the::sound::close() {}

void  the::sound::play(bool loop)	{}
void  the::sound::playUnLoop()		{}
void  the::sound::playLoop()		{}
bool  the::sound::getLoop()			{return false;}
void  the::sound::setGain(float v)	{}
float the::sound::getGain()			{return 1.0f;}
void  the::sound::stop()			{}
void  the::sound::pause()			{}
void  the::sound::load(strref f)	{}
bool  the::sound::isPlaying()		{return false;}
bool  the::sound::isPaused()		{return false;}
bool  the::sound::isStopped()		{return false;}



#endif