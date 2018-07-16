#include <gtest/gtest.h>

#include <Usagi/Engine/Game/Game.hpp>

TEST(GameTest, InitTest)
{
    usagi::Game game;
    EXPECT_NO_THROW(game.initializeInput());
}
