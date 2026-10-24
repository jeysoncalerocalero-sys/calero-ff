#pragma once
#include <vector>
#include <string>

struct HookReport { std::string name; bool ok; };

namespace H {
    inline bool headshot = false;
    inline bool aimlock  = false;
    inline bool norecoil = false;
    inline float fov     = 90.f;
    inline float smooth  = 0.35f;
    inline std::vector<HookReport> report;

    void Install();
    void Tick();          // se llama cada frame desde el hook de render
}
