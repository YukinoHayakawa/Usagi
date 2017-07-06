#include <GL/glew.h>
#include <GL/wglew.h>

#include "WGLContext.hpp"

void yuki::WGLContext::HGLRCWrapper::clear()
{
    dc = nullptr;
    glrc = nullptr;
}

void yuki::WGLContext::HGLRCWrapper::destroy()
{
    if(glrc)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(glrc);
    }
}

void yuki::WGLContext::HGLRCWrapper::setCurrent()
{
    if(!wglMakeCurrent(dc, glrc))
        throw OpenGLException("wglMakeCurrent() failed");
}

yuki::WGLContext::HGLRCWrapper::HGLRCWrapper()
{
}

yuki::WGLContext::HGLRCWrapper::HGLRCWrapper(HDC dc, HGLRC handle)
    : dc { dc }
    , glrc { handle }
{
}

yuki::WGLContext::HGLRCWrapper::~HGLRCWrapper()
{
    destroy();
}

yuki::WGLContext::HGLRCWrapper::HGLRCWrapper(HGLRCWrapper &&other) noexcept
    : dc(other.dc)
    , glrc(other.glrc)
{
    other.clear();
}

yuki::WGLContext::HGLRCWrapper & yuki::WGLContext::HGLRCWrapper::operator=(HGLRCWrapper &&other) noexcept
{
    if(this == &other)
        return *this;
    destroy();
    dc = other.dc;
    glrc = other.glrc;
    other.clear();
    return *this;
}

yuki::WGLContext::WGLContext(HDC dc)
{
    // setup pixel format
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // set the pixel format of DC to which we like
    int pfmt = ChoosePixelFormat(dc, &pfd);
    if(!pfmt)
        throw OpenGLException("ChoosePixelFormat() failed");
    if(!SetPixelFormat(dc, pfmt, &pfd))
        throw OpenGLException("SetPixelFormat() failed");

    // this creates an old OpenGL context (version <= 2.1), to use 3+ we have to
    // use WGL extension like below.
    mContext = HGLRCWrapper { dc, wglCreateContext(dc) };
    if(!mContext.glrc) throw OpenGLException("wglCreateContext() failed");
    mContext.setCurrent();

    GLenum err = glewInit();
    if(GLEW_OK != err) throw OpenGLException("glewInit() failed");

    if(wglewIsSupported("WGL_ARB_create_context") == 1)
    {
        int attribs[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 1,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };
        mContext = HGLRCWrapper { dc, wglCreateContextAttribsARB(dc, nullptr, attribs) };
    }
    else
    {
        throw OpenGLException("WGL_ARB_create_context is not supported");
    }
}

void yuki::WGLContext::setCurrent()
{
    mContext.setCurrent();
}

void yuki::WGLContext::swapBuffer()
{
    SwapBuffers(mContext.dc);
}
