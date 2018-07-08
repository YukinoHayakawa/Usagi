#include "Mouse.hpp"

void usagi::Mouse::addMouseEventListener(
    std::shared_ptr<MouseEventListener> listener)
{
    mMouseEventListeners.push_back(std::move(listener));
}

void usagi::Mouse::setImmersiveMode(bool enable)
{
    if(enable == isImmersiveMode()) return;
    if(enable)
    {
        _captureCursor();
        showCursor(false);
    }
    else
    {
        _releaseCursor();
        showCursor(true);
    }
    mImmersiveMode = enable;
}

bool usagi::Mouse::isImmersiveMode() const
{
    return mImmersiveMode;
}
