#include <gtest/gtest.h>

#include <eigen3/Eigen/Core>

#include <Usagi/Engine/Entity/Game.hpp>
#include <Usagi/Engine/Entity/Subsystem.hpp>
#include <Usagi/Engine/Entity/Entity.hpp>
#include <Usagi/Engine/Entity/Component.hpp>
#include <Usagi/Engine/Event/Event.hpp>

using namespace yuki;

namespace test
{
struct PositionComponent : Component
{
    Eigen::Vector3f position;
};

struct PhysicalComponent : Component
{
    Eigen::Vector3f velocity;
    Eigen::Vector3f acceleration;
};

class PhysicsSubsystem : public Subsystem
{
public:
    void update(const std::chrono::milliseconds &dt) override
    {
        // fetch all entities with PositionComponent
    }
};

struct TestEvent : Event
{
    const int hello = 233;
};
}

TEST(ECSTest, SubsystemNameConflict)
{
    Game game;
    {
        SubsystemInfo info;
        info.name = "Physics";
        info.subsystem = std::make_unique<test::PhysicsSubsystem>();
        EXPECT_NO_THROW(game.addSubsystem(std::move(info)));
    }
    {
        SubsystemInfo info;
        info.name = "Physics";
        info.subsystem = std::make_unique<test::PhysicsSubsystem>();
        EXPECT_THROW(game.addSubsystem(std::move(info)), std::runtime_error
        );
    }
}

TEST(ECSTest, EventBubblingTest)
{
    using namespace test;

    // Entity tree structure
    //
    //        root
    //        /  \
    //       a    b
    //      /
    //     c

    Game game;
    auto r = game.getRootEntity();
    auto a = r->addChild();
    auto b = r->addChild();
    auto c = a->addChild();
    int dr = 0, da = 0, db = 0, dc = 0;
    r->addEventListener<TestEvent>([&](auto &e)
    {
        dr = e.hello;
    });
    a->addEventListener<TestEvent>([&](auto &e)
    {
        da = e.hello;
        e.stopBubbling();
    });
    b->addEventListener<TestEvent>([&](auto &e)
    {
        db = e.hello;
    });
    c->addEventListener<TestEvent>([&](auto &e)
    {
        dc = e.hello;
    });
    c->fireEvent(TestEvent());

    EXPECT_EQ(dr, 0);
    EXPECT_EQ(db, 0);
    EXPECT_EQ(da, 233);
    EXPECT_EQ(dc, 233);
}

TEST(ECSTest, EventCancelTest)
{
    using namespace test;

    Game game;
    auto r = game.getRootEntity();
    bool a = false, b = false, c = false;
    r->addEventListener<TestEvent>([&](auto &e)
    {
        a = true;
    });
    r->addEventListener<TestEvent>([&](auto &e)
    {
        b = true;
        e.cancel();
    });
    r->addEventListener<TestEvent>([&](auto &e)
    {
        c = true;
    });
    r->fireEvent(TestEvent());

    EXPECT_TRUE(a);
    EXPECT_TRUE(b);
    EXPECT_FALSE(c);
}

TEST(ECSTest, EventReferenceTest)
{
    using namespace test;

    Game game;
    TestEvent e;
    game.getRootEntity()->fireEvent(e);
    game.getRootEntity()->fireEvent(std::move(e));
}

TEST(ECSTest, ComponentSystemInteractionTest)
{
    Game game;
    {
        SubsystemInfo info;
        info.name = "Physics";
        info.subsystem = std::make_unique<test::PhysicsSubsystem>();
        EXPECT_NO_THROW(game.addSubsystem(std::move(info)));
    }
    auto r = game.getRootEntity();
    r->addComponent(std::make_unique<test::PositionComponent>());

    // world.update()
}

#include "../GTestMain.hpp"
