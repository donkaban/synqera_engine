#ifndef _THE_SOUND_H_
#define _THE_SOUND_H_

#include "TheEngine/common.h"

namespace the {
class sound :
    public abstract::object
{
    THE_SHARED(sound)
public:
   
    static void init();
    static void close();

    sound(strref);
    ~sound();

    void  load(strref);
    void  play(bool loop = false);
    
    void  playUnLoop();
    void  playLoop();   
    void  stop();
    void  pause();
    
    void  setGain(float v);
    float getGain();
    bool  getLoop(); 
    
    bool  isPlaying();
    bool  isPaused();
    bool  isStopped();

private:
    void * _audio_data;

};
}
#endif