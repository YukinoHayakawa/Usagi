#pragma once

namespace usagi::ecs
{
struct GameContext;

class System
{
public:
    virtual ~System() = default;

    virtual void execute(GameContext &ctx) = 0;
};

class GameInitSystem : public System
{
public:
};

class GameFinalizeSystem : public System
{
public:
};

class FrameInitSystem : public System
{
public:
};

class FrameUpdateSystem : public System
{
public:
};

class FrameFinalizeSystem : public System
{
public:
};
}
