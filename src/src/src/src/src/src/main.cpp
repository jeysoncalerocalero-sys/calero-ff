#include <jni.h>
#include <pthread.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <android/input.h>
#include <android/log.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "dobby.h"
#include "il2cpp.h"
#include "menu.h"
#include "hooks.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"PHANTOM",__VA_ARGS__)

bool g_MenuOpen = true;
JavaVM *g_VM = nullptr;

static bool g_Ready = false;
static int  g_W = 0, g_H = 0;
static EGLBoolean (*o_swap)(EGLDisplay, EGLSurface) = nullptr;

static uintptr_t baseOf(const char *lib) {
    FILE *fp = fopen("/proc/self/maps","r"); if (!fp) return 0;
    char l[512]; uintptr_t b = 0;
    while (fgets(l,sizeof(l),fp))
        if (strstr(l,lib)) { b = strtoull(l,nullptr,16); break; }
    fclose(fp); return b;
}

static void waitFor(const char *lib, int ms = 30000) {
    for (int i = 0; i < ms/50 && !baseOf(lib); ++i) usleep(50000);
}

static void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.DisplaySize = ImVec2((float)g_W,(float)g_H);
    ApplyPhantomTheme();
    ImGui_ImplOpenGL3_Init("#version 300 es");
    g_Ready = true;
    LOGI("ImGui listo %dx%d", g_W, g_H);
}

static EGLBoolean h_swap(EGLDisplay d, EGLSurface s) {
    if (!g_Ready) {
        eglQuerySurface(d,s,EGL_WIDTH,&g_W);
        eglQuerySurface(d,s,EGL_HEIGHT,&g_H);
        if (g_W > 0 && g_H > 0) initImGui();
    }
    if (g_Ready) {
        H::Tick();
        ImGui::GetIO().DisplaySize = ImVec2((float)g_W,(float)g_H);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        DrawMenu();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    return o_swap(d,s);
}

typedef void (*t_ime)(void*, void*, void*);
static t_ime o_ime = nullptr;

static void h_ime(void *self, void *ev, void *msg) {
    if (g_Ready && g_MenuOpen) {
        int32_t a = AMotionEvent_getAction((const AInputEvent*)ev);
        int32_t m = a & AMOTION_EVENT_ACTION_MASK;
        size_t  i = (size_t)(a >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT);
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent(AMotionEvent_getX((const AInputEvent*)ev,i),
                            AMotionEvent_getY((const AInputEvent*)ev,i));
        if (m==AMOTION_EVENT_ACTION_DOWN || m==AMOTION_EVENT_ACTION_POINTER_DOWN)
            io.AddMouseButtonEvent(0,true);
        else if (m==AMOTION_EVENT_ACTION_UP || m==AMOTION_EVENT_ACTION_POINTER_UP ||
                 m==AMOTION_EVENT_ACTION_CANCEL)
            io.AddMouseButtonEvent(0,false);
    }
    o_ime(self,ev,msg);
}

static void *boot(void *) {
    waitFor("libEGL.so");
    void *e = dlopen("libEGL.so", RTLD_NOW);
    DobbyHook(dlsym(e,"eglSwapBuffers"), (void*)h_swap, (void**)&o_swap);

    void *sym = DobbySymbolResolver("libinput.so",
      "_ZN7android13InputConsumer21initializeMotionEventEPNS_11MotionEventEPKNS_12InputMessageE");
    if (sym) DobbyHook(sym,(void*)h_ime,(void**)&o_ime);

    waitFor("libil2cpp.so");
    if (IL::Init()) H::Install();
    LOGI("PHANTOM cargado");
    return nullptr;
}

extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *) {
    g_VM = vm;
    pthread_t t;
    pthread_create(&t,nullptr,boot,nullptr);
    return JNI_VERSION_1_6;
}
