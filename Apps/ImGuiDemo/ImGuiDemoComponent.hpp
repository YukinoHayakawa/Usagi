#pragma once

#include <Usagi/Extension/ImGui/ImGui.hpp>
#include <Usagi/Extension/ImGui/ImGuiComponent.hpp>

namespace usagi
{
struct ImGuiDemoComponent : ImGuiComponent
{
    void draw(const Clock &clock) override
    {
        ImGui::ShowDemoWindow();
        ImGui::ShowMetricsWindow();
    }
};
}
