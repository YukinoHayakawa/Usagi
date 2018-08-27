#include "Mouse.hpp"

void usagi::Mouse::setImmersiveMode(const bool enable)
{
    if(enable == isImmersiveMode())
        return;
    if(enable)
    {
        showCursor(false);
        captureCursor();
    }
    else
    {
        releaseCursor();
        showCursor(true);
    }
    mImmersiveMode = enable;
}
