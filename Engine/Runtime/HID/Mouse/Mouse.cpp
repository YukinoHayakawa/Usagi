﻿#include "Mouse.hpp"

void yuki::Mouse::addMouseEventListener(std::shared_ptr<MouseEventListener> listener)
{
    mMouseEventListeners.push_back(std::move(listener));
}

void yuki::Mouse::setImmersiveMode(bool enable)
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

bool yuki::Mouse::isImmersiveMode() const
{
    return mImmersiveMode;
}