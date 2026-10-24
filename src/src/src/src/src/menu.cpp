#include "menu.h"
#include "hooks.h"
#include "imgui.h"

extern bool g_MenuOpen;

void ApplyPhantomTheme() {
    ImGuiStyle &s = ImGui::GetStyle();
    s.WindowRounding = 10; s.FrameRounding = 7; s.GrabRounding = 7;
    s.WindowBorderSize = 1; s.FramePadding = ImVec2(12,7);
    auto *c = s.Colors;
    c[ImGuiCol_WindowBg]      = ImVec4(0.03f,0.03f,0.04f,0.95f);
    c[ImGuiCol_Border]        = ImVec4(1.00f,0.10f,0.10f,0.70f);
    c[ImGuiCol_TitleBgActive] = ImVec4(0.78f,0.03f,0.03f,1.00f);
    c[ImGuiCol_Button]        = ImVec4(0.70f,0.03f,0.03f,1.00f);
    c[ImGuiCol_CheckMark]     = ImVec4(1.00f,0.20f,0.20f,1.00f);
    c[ImGuiCol_SliderGrab]    = ImVec4(1.00f,0.20f,0.20f,1.00f);
    c[ImGuiCol_Header]        = ImVec4(0.70f,0.03f,0.03f,0.65f);
}

void DrawMenu() {
    if (!g_MenuOpen) return;

    ImGui::SetNextWindowSize(ImVec2(290,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(25,160), ImGuiCond_FirstUseEver);
    ImGui::Begin("PHANTOM", &g_MenuOpen, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Auto Headshot", &H::headshot);
    ImGui::Checkbox("Aim Lock",      &H::aimlock);
    ImGui::Checkbox("No Recoil",     &H::norecoil);

    ImGui::Separator();
    ImGui::SliderFloat("FOV",    &H::fov,    30.f, 180.f, "%.0f");
    ImGui::SliderFloat("Smooth", &H::smooth, 0.f,   1.f, "%.2f");

    ImGui::Separator();
    if (ImGui::CollapsingHeader("estado")) {
        for (auto &r : H::report)
            ImGui::TextColored(r.ok ? ImVec4(0.3f,1,0.3f,1) : ImVec4(1,0.3f,0.3f,1),
                               "%s  %s", r.ok ? "[ok]" : "[x]", r.name.c_str());
    }
    ImGui::End();
}
