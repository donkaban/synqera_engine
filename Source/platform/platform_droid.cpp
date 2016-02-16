#ifdef PLATFORM_ANDROID 

#ifndef __ANDROID__  // only for clang analizer :)
#define __ANDROID__
#endif

#include "TheEngine/logger.h"
#include "TheEngine/aux.h"
#include "TheEngine/platform/window.h"
#include "TheEngine/openGL.h"

using namespace platform;

void window::_init()
{
    logger::debug("init Android platform done");
}

void window::_close()  
{
    logger::debug("close Android Platform");
} 

void window::_resize()
{
    logger::extra("[Android] resize %dx%d", _width, _height);
    glViewport(0, 0, _width, _height);
    GL::TUNE();
    GL_CHECK("platform::_resize");
}

bool window::update()
{
    auto dt = _get_dt();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(onUpdate)     onUpdate(dt);
    return true;
}

#endif