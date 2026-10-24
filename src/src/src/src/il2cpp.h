#pragma once
#include "dobby.h"

struct Vec3 { float x, y, z; };

namespace IL {
    bool  Init();
    void *Klass(const char *ns, const char *name);
    void *Method(const char *ns, const char *klass, const char *name, int argc);
    void *HookMethod(const char *ns, const char *klass, const char *name,
                     int argc, void *hook, void **orig);
    size_t FieldOff(const char *ns, const char *klass, const char *field);

    void *LocalPlayer();
    Vec3  GetPosition(void *player);
    Vec3  GetRotation(void *player);
    void  SetRotation(void *player, Vec3 rot);
    Vec3  GetForward(void *player);
    Vec3  GetHeadBone(void *player);
}
