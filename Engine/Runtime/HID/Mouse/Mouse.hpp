#pragma once

#include <memory>
#include <vector>

#include <eigen3/Eigen/Core>

namespace yuki
{

enum class KeyCode;

class Mouse
{
protected:
    std::vector<std::shared_ptr<class MouseEventListener>> mMouseEventListeners;

public:
    virtual ~Mouse() = default;

    void addMouseEventListener(std::shared_ptr<MouseEventListener> listener);

    virtual Eigen::Vector2f getMouseCursorWindowPos() = 0;
};

}
