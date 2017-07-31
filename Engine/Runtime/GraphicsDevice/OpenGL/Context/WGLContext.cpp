#include <GL/glew.h>
#include <GL/wglew.h>
#include <stdexcept>

#include "../OpenGLCommon.hpp"

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
        throw std::runtime_error("wglMakeCurrent() failed");
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
        throw std::runtime_error("ChoosePixelFormat() failed");
    if(!SetPixelFormat(dc, pfmt, &pfd))
        throw std::runtime_error("SetPixelFormat() failed");

    // this creates an old OpenGL context (version <= 2.1), to use 3+ we have to
    // use WGL extension like below.
    mContext = HGLRCWrapper { dc, wglCreateContext(dc) };
    if(!mContext.glrc) throw std::runtime_error("wglCreateContext() failed");
    mContext.setCurrent();

    auto create_context = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
        wglGetProcAddress("wglCreateContextAttribsARB")
    );
    // http://developer.download.nvidia.com/opengl/specs/WGL_ARB_create_context.txt
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, 0, // WGL_CONTEXT_DEBUG_BIT_ARB,
        // creates core profile
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    mContext = HGLRCWrapper { dc, create_context(dc, nullptr, attribs) };
    mContext.setCurrent();

    GLenum err = glewInit();
    if(GLEW_OK != err) throw std::runtime_error("glewInit() failed");
}

void yuki::WGLContext::setCurrent()
{
    mContext.setCurrent();
}

void yuki::WGLContext::swapBuffer()
{
    SwapBuffers(mContext.dc);
}
