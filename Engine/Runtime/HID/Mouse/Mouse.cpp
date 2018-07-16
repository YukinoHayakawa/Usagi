#include "Mouse.hpp"

void usagi::Mouse::addEventListener(MouseEventListener *listener)
{
    mMouseEventListeners.push_back(listener);
}

void usagi::Mouse::setImmersiveMode(bool enable)
{
    if(enable == isImmersiveMode())
        return;
    if(enable)
    {
        captureCursor();
        showCursor(false);
    }
    else
    {
        releaseCursor();
        showCursor(true);
    }
    mImmersiveMode = enable;
}
