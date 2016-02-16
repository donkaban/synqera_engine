#if defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS) 

#include "TheEngine/openGL.h"
#include "TheEngine/common.h"
#include "TheEngine/timer.h"
#include "TheEngine/aux.h"
#include "TheEngine/platform/window.h"
#include "TheEngine/logger.h"
#include "TheEngine/math/util.h"
#include "TheEngine/platform/input.h"


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static Display * display;
static Window    xwindow;
static Window    root;    
static int       screen;
static Visual  * visual; 
static XIC       xic;
static XIM       xim;   

static std::unordered_map<int,int> keyMap = 
{
#ifdef PLATFORM_MACOS
    {59,  platform::KEY::BACKSPACE},
    {56,  platform::KEY::TAB},
    {61,  platform::KEY::ESC},
    {134, platform::KEY::UP},
    {133, platform::KEY::DOWN},
    {131, platform::KEY::LEFT},
    {132, platform::KEY::RIGHT},
    
#else
    {22,  platform::KEY::BACKSPACE},
    {23,  platform::KEY::TAB},
    {9,   platform::KEY::ESC},
    {111, platform::KEY::UP},
    {116, platform::KEY::DOWN},
    {113, platform::KEY::LEFT},
    {114, platform::KEY::RIGHT},
 
#endif
};

#ifdef RENDER_OPENGLES2  
    #include <EGL/egl.h>
    
    static EGLNativeWindowType egl_window;
    static EGLDisplay          egl_display;
    static EGLContext          egl_context;
    static EGLSurface          egl_surface;
    static EGLConfig           egl_config;

    static std::unordered_map<int,const char *> EGL_ERRORS = 
    {
        {EGL_SUCCESS,               "EGL_SUCCESS"},
        {EGL_BAD_ALLOC,             "EGL_BAD_ALLOC"},
        {EGL_BAD_ACCESS,            "EGL_BAD_ACCESS"},
        {EGL_BAD_CONTEXT,           "EGL_BAD_CONTEXT"},
        {EGL_BAD_CONFIG,            "EGL_BAD_CONFIG"},
        {EGL_BAD_CURRENT_SURFACE,   "EGL_BAD_CURRENT SURFACE"},
        {EGL_BAD_DISPLAY,           "EGL_BAD_DISPLAY"},
        {EGL_BAD_SURFACE,           "EGL_BAD_SURFACE"},
        {EGL_BAD_MATCH,             "EGL_BAD_MATCH"},
        {EGL_BAD_PARAMETER,         "EGL_BAD_PARAMETER"},
        {EGL_BAD_NATIVE_PIXMAP,     "EGL_BAD_NATIVE_PIXMAP"},
        {EGL_BAD_NATIVE_WINDOW,     "EGL_BAD_NATIVE_WINDOW"},
        {EGL_CONTEXT_LOST,          "EGL_CONTEXT_LOST"},
    };
    static void _checkEGLerrors()  
    {
        auto e = eglGetError();
        if(e != EGL_SUCCESS)
        {
            if(EGL_ERRORS.find(e) != EGL_ERRORS.end())
                {logger::error("[EGL] %s",EGL_ERRORS[e]);exit(-1); }
            else
                {logger::error("[EGL] unknown error");exit(-1); }
        }
    }   
#elif defined(RENDER_OPENGL)
    #include "GL/glx.h"
    static GLXContext          egl_context;

#endif

using namespace platform;


void window::_init()
{
   logger::debug("[X11] create native window");
  
    display = XOpenDisplay(NULL);
    if (!display) 
        FATAL_ERROR("[X11] can't open display");

    root   = XDefaultRootWindow(display);
    screen = XDefaultScreen(display);
    visual = XDefaultVisual(display, screen);
    XWindowAttributes x_attr;
 
    xwindow = XCreateSimpleWindow(display,root,0,0,_width, _height,0,0,0);
    XStoreName(display,xwindow, _title.c_str());
    XGetWindowAttributes(display,DefaultRootWindow(display),&x_attr);
    Atom wmEvents[] = 
    {
        XInternAtom(display, "WM_DELETE_WINDOW", True),
    };    
    XSetWMProtocols(display,xwindow, wmEvents, 1);
 
#if defined(RENDER_OPENGLES2)  
    logger::debug("[X11] init openGL|ES 2 API");
          
    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,EGL_NONE, EGL_NONE };
    
    EGLint numConfigs;
    EGLint majorVersion;
    EGLint minorVersion;

    EGLint attr[] =
#if defined(PLATFORM_ARM)    
    {
        EGL_RED_SIZE,           5,
        EGL_GREEN_SIZE,         6,
        EGL_BLUE_SIZE,          5,
        EGL_DEPTH_SIZE,         8,
        EGL_ALPHA_SIZE,         EGL_DONT_CARE,
        EGL_STENCIL_SIZE,       EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS,     EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT, 
        EGL_NONE
    };
#else
    {
        EGL_RED_SIZE,           8,
        EGL_GREEN_SIZE,         8,
        EGL_BLUE_SIZE,          8,
        EGL_DEPTH_SIZE,         24,
        EGL_ALPHA_SIZE,         EGL_DONT_CARE,
        EGL_STENCIL_SIZE,       EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS,     EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT, 
        EGL_NONE
    };
#endif

    egl_display   = eglGetDisplay(static_cast<EGLNativeDisplayType>(display)); 
    egl_window    = static_cast<EGLNativeWindowType>(xwindow);
    if(
        (!eglInitialize(egl_display, &majorVersion, &minorVersion)) || 
        (!eglBindAPI(EGL_OPENGL_ES_API))                            ||
        (!eglGetConfigs(egl_display,0,0,&numConfigs))               || 
        (!eglChooseConfig(egl_display, attr, &egl_config, 1, &numConfigs)))
   _checkEGLerrors(); 
   
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, egl_window, NULL);  _checkEGLerrors();
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, contextAttribs);_checkEGLerrors();
    while(!eglWaitGL()) {};
    if(!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))           _checkEGLerrors();
   
#elif defined(RENDER_OPENGL)
    logger::debug("[X11] init openGL API");
  
    int attr[] = 
    {
        GLX_USE_GL, True,
        GLX_RGBA,   True,
        GLX_DOUBLEBUFFER,
        GLX_BUFFER_SIZE,32,
        GLX_DEPTH_SIZE, 24,
        None
    };
    auto visual= glXChooseVisual(display, screen, attr);
    if (!visual)
        FATAL_ERROR("[GLX] unable to find visual");
    egl_context = glXCreateContext(display, visual, NULL, True);
    if (!egl_context)
        FATAL_ERROR("[GLX] unable to create window context");
    glXMakeCurrent(display,xwindow, egl_context);
#endif
    logger::extra("[X11] window config %dx%d", _width, _height);
}

void window::_resize()
{
    logger::extra("[X11] resize %dx%d",_width, _height);
    XSetWindowAttributes attr; 
    std::memset(&attr,0,sizeof(attr));
    attr.event_mask = StructureNotifyMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|KeyPressMask;
    attr.override_redirect  = _fullscreen;
    if(_fullscreen)  attr.background_pixel   = 0xFFFFFF00;
    else             attr.background_pixel   = 0xFFFF0000;
    
    XWithdrawWindow(display,xwindow, screen);  
    XChangeWindowAttributes(display,xwindow,CWBackPixel|CWOverrideRedirect|CWSaveUnder|CWEventMask|CWBorderPixel, &attr);
    XResizeWindow(display,xwindow,_width,_height);
    XRaiseWindow(display,xwindow);
    XMapWindow(display,xwindow);
    XMoveWindow(display,xwindow,0,0);
    XFlush(display);
    if(onResize) onResize(_width, _height);
    
    XIMStyles* imStyles;
    xim = XOpenIM(display, NULL, NULL, NULL);
    if (xim == NULL)
        FATAL_ERROR("[X11] XOpenIM returned NULL!");
    XGetIMValues(xim, XNQueryInputStyle, &imStyles, static_cast<char*>(NULL));
    if (imStyles->count_styles <= 0)
        FATAL_ERROR("[X11] XGetIMValues returned empty style list!");
    xic = XCreateIC(xim,XNInputStyle, imStyles->supported_styles[0],XNClientWindow, xwindow, static_cast<char*>(NULL));
    XFree(imStyles);
    if (xic == NULL)
        FATAL_ERROR("[X11] XCreateIC returned NULL!");
}

void window::_close()
{
#if defined(RENDER_OPENGLES2) ||  defined(RENDER_OPENGLES3)   
    eglDestroyContext(egl_display, egl_context);
#elif defined(RENDER_OPENGL)
    glXDestroyContext(display, egl_context);
#endif           
    XDestroyWindow(display,xwindow);
}

bool window::update()
{
    XEvent evt;
       
    for (int i = 0; i < XPending(display); i++)
    { 
        XNextEvent(display, &evt);

        switch (evt.type)
        {
            case KeyPress:
            {
                if(!onKeyPress) break;
                KeySym  key;
                char  buff[10];
                auto event  = (XKeyEvent *) &evt;
                int len = Xutf8LookupString(xic, event, buff, sizeof(buff), &key, nullptr);
                std::string strRep = (len > 0) ? std::string(buff, static_cast<size_t>(len)) : "";
                int k = platform::KEY::LETTER;
                if(keyMap.find(event->keycode) != keyMap.end())
                    k = keyMap[event->keycode];
                onKeyPress(k,strRep);
                break;
            }   
            case ButtonPress:
            {
                if(!onMousePress) break;
                auto event = (XButtonEvent *) &evt;
                onMousePress(event->x, event->y);
                XSetInputFocus(display,xwindow, RevertToNone, CurrentTime);
                break;
            }    
            case ButtonRelease: 
            {
                if(!onMouseRelease) break;
                auto event = (XButtonEvent *) &evt;
                onMouseRelease(event->x, event->y);
                break;
            }
            case MotionNotify:
            {
                if(!onMouseMove) break;
                auto event = (XMotionEvent *) &evt;
                onMouseMove(event->x, event->y);
                break;
            }    
            case ClientMessage:
            {
                logger::system("[X11] NATIVE EVENT: QUIT");
                return false;
            }   
            case MapNotify:
            {   
                logger::system("[X11] NATIVE EVENT: MAPPING");
                glViewport(0, 0, static_cast<GLsizei>(_width), static_cast<GLsizei>(_height));
                GL::TUNE();
                GL_CHECK("platformX11::MapNotify");
                break;
            }
          
            default:
                break;
        }
    }
    auto dt = _get_dt();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(onUpdate) onUpdate(dt);
#if defined(RENDER_OPENGLES2) || defined(RENDER_OPENGLES3)    
    eglSwapBuffers(egl_display, egl_surface);
#elif defined(RENDER_OPENGL)
    glXSwapBuffers(display,xwindow);
#endif   
    return true;        
}
 
#endif