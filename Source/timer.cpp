#include "TheEngine/timer.h"
#include "TheEngine/logger.h"

#include <iostream>
#include <iomanip>
#include <ctime>

timepoint the::timer::globalStart = std::chrono::system_clock::now();
bool      the::timer::globalLock  = true;

std::vector<the::timer *> the::timer::timers;

the::timer::timer(strref tag) :
    abstract::object("Timer",tag),
    start(std::chrono::system_clock::now()),
    last(std::chrono::system_clock::now()),
    current(std::chrono::system_clock::now())
{
    logger::debug("[Timer  ] create %s",tag.c_str());
    timers.push_back(this);
}

the::timer::timer() : the::timer("--")
{}

the::timer::~timer()
{
    logger::debug("[Timer  ] delete %s",getTag().c_str());
    timers.erase(std::remove(timers.begin(), timers.end(), this), timers.end());
}

void the::timer::lock()   
{
    logger::system("[Timer  ] TIMER FREEZE");   
    globalStart = std::chrono::system_clock::now();
    the::timer::globalLock = true;
}
void the::timer::unlock() 
{
    logger::system("[Timer  ] TIMER UNFREEZE"); 
    globalStart = std::chrono::system_clock::now();
    the::timer::globalLock = false;
}

float the::timer::getTime() 
{
    auto cur = std::chrono::system_clock::now();
    std::chrono::duration<float> dt(cur - start);
    return dt.count();
}
float the::timer::globalTime()
{
    if(globalLock)
    {
        globalStart = std::chrono::system_clock::now();
        return 0;
    }    
    std::chrono::duration<float> dt(std::chrono::system_clock::now() - globalStart);
    return dt.count();
}

std::string the::timer::timestamp()
{
    static char buff[32]; 
    time_t t;
    std::time(&t);
    struct tm *ti = std::localtime(&t);
    std::sprintf(buff,"%2d_%.2d_%.2d",ti->tm_hour,ti->tm_min,ti->tm_sec);
    return std::string(buff);
}

float the::timer::_getDT() 
{
    current = std::chrono::system_clock::now();
    std::chrono::duration<float> dt(current - last);
    last = current;
    return dt.count();   
}

void the::timer::reset() 
{
    current = last = start = std::chrono::system_clock::now();
}

void the::timer::update()
{
    if(!globalLock)
    {
        auto dt = _getDT();
        tasks.foreach([&](the::timed_task *t) {return t->invoke(dt);});
    }    
}  

uint the::timer::addCallback(float dt, const callable & task, bool looped)
{
    logger::debug("[Timer  ] add callback, dt: %0.2f loop: %s",dt, looped? "true" : "false");
    auto t = new the::timed_task(dt,task,looped);
    return tasks.push(t);
}

void the::timer::unloopCallback(uint id) {tasks[id]->looped(false);}
void the::timer::removeCallback(uint id) {tasks.erase(id);}
void the::timer::update_all() 
{
    if(!globalLock)
        for (auto & v : timers) v->update();
}

the::timed_task::timed_task(float delay, const callable & task, bool looped) : 
    abstract::object("Task","TimerCallback"),
    _delay(delay),
    _looped(looped),
    _task(task)

{}

bool the::timed_task::invoke(float dt)
{
    _current += dt;
    if(_current >= _delay)
    {    
        _current = 0;
        _task();    
       return !_looped;
    }   
    return false;
}

the::timed_task::~timed_task()
{}

bool the::timed_task::looped() const {return _looped;}
void the::timed_task::looped(bool looped) {_looped = looped;}


