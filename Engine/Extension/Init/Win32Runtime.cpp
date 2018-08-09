﻿#ifdef _WIN32

#include <Usagi/Engine/Runtime/Runtime.hpp>

#include <Usagi/Engine/Extension/Vulkan/VulkanGpuDevice.hpp>
#include <Usagi/Engine/Extension/Win32/Win32Helper.hpp>
#include <Usagi/Engine/Extension/Win32/Window/Win32WindowManager.hpp>
#include <Usagi/Engine/Extension/Win32/Input/Win32InputManager.hpp>

usagi::Runtime::Runtime()
{
    win32::patchConsole();
}

usagi::Runtime::~Runtime()
{
    // delete unique_ptrs here.
}

void usagi::Runtime::initGpu()
{
    if(mGpu) return;
    mGpu = std::make_unique<VulkanGpuDevice>();
}

void usagi::Runtime::initInput()
{
    if(mInputManager) return;
    mInputManager = std::make_unique<Win32InputManager>();
}

void usagi::Runtime::initWindow()
{
    if(mWindowManager) return;
    mWindowManager = std::make_unique<Win32WindowManager>();
}

#endif
