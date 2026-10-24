#include "hooks.h"
#include "Config.h"
#include "il2cpp.h"
#include "dobby.h"
#include <math.h>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"PHANTOM",__VA_ARGS__)
#define PI 3.14159265f

struct Vec3 { float x, y, z; };

static void *o_hs = nullptr, *o_aim = nullptr, *o_rec = nullptr;
static void *localPlayer = nullptr;
static void *lockedTarget = nullptr;

/* ---------- 1. AUTO HEADSHOT ---------- */
static int h_getHitPart(void *self) {
    int orig = o_hs ? ((int(*)(void*))o_hs)(self) : 0;
    if (headshot && lockedTarget && self == localPlayer) return 1;  // 1 = cabeza
    return orig;
}

/* ---------- 2. AIM LOCK ---------- */
static Vec3 myPos, myFwd, enemyHead;
static bool haveEnemy = false;

static void angleTo(Vec3 from, Vec3 to, Vec3 &out) {
    float dx = to.x - from.x, dy = to.y - from.y, dz = to.z - from.z;
    out.x = atan2f(dx, dz) * 180.f / PI;                       // yaw
    out.y = -atan2f(dy, sqrtf(dx*dx + dz*dz)) * 180.f / PI;    // pitch
    out.z = 0.f;
}

static float angleDiff(Vec3 fwd, Vec3 dir) {
    float len = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
    if (len < 0.001f) return 999.f;
    float dot = (dir.x*fwd.x + dir.y*fwd.y + dir.z*fwd.z) / len;
    dot = fmaxf(-1.f, fminf(1.f, dot));
    return acosf(dot) * 180.f / PI;
}

static void h_playerUpdate(void *self) {
    if (o_aim) ((void(*)(void*))o_aim)(self);

    if (!aimlock || !localPlayer || !self) return;
    if (self == localPlayer) return;

    // distancia y ángulo al enemigo
    Vec3 p = IL::GetPosition(self);
    Vec3 d { p.x - myPos.x, p.y - myPos.y, p.z - myPos.z };

    if (angleDiff(myFwd, d) <= fov) {
        lockedTarget = self;
        haveEnemy = true;

        Vec3 target = IL::GetHeadBone(self);
        Vec3 want; angleTo(myPos, target, want);

        Vec3 cur = IL::GetRotation(localPlayer);
        float k = fminf(1.f, fmaxf(0.05f, smooth));
        Vec3 next {
            cur.x + (want.x - cur.x) * k,
            cur.y + (want.y - cur.y) * k,
            0.f
        };
        IL::SetRotation(localPlayer, next);
    }
}

/* ---------- 3. NO RECOIL ---------- */
static float h_getRecoil(void *self) {
    if (norecoil) return 0.0f;
    return o_rec ? ((float(*)(void*))o_rec)(self) : 0.f;
}

/* ---------- INSTALACION ---------- */
void H::Install() {
    localPlayer = IL::LocalPlayer();

    report.clear();
    report.push_back({ "Auto Headshot", false });
    report.push_back({ "Aim Lock",      false });
    report.push_back({ "No Recoil",     false });

    if (IL::HookMethod("", CFG::HS_CLASS, CFG::HS_METHOD, CFG::HS_ARGC,
                       (void*)h_getHitPart, &o_hs))
        report[0].ok = true;

    if (IL::HookMethod("", CFG::AIM_CLASS, CFG::AIM_METHOD, CFG::AIM_ARGC,
                       (void*)h_playerUpdate, &o_aim))
        report[1].ok = true;

    if (IL::HookMethod("", CFG::REC_CLASS, CFG::REC_METHOD, CFG::REC_ARGC,
                       (void*)h_getRecoil, &o_rec))
        report[2].ok = true;

    for (auto &r : report) LOGI("%s -> %s", r.name.c_str(), r.ok ? "OK" : "NO");
}

/* ---------- por frame: actualizar mi posicion/mirada ---------- */
void H::Tick() {
    if (!localPlayer) { localPlayer = IL::LocalPlayer(); return; }
    myPos = IL::GetPosition(localPlayer);
    myFwd = IL::GetForward(localPlayer);
}
