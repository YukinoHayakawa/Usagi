#include "Mouse.hpp"

void usagi::Mouse::setImmersiveMode(const bool enable)
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
