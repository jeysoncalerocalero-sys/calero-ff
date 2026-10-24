#pragma once
// =====================================================================
//  Si algo no engancha, es ACÁ donde lo arreglás. Nada más.
//  Los nombres salen de dump.cs (te explico abajo). 3 líneas, 5 minutos.
// =====================================================================

#define TARGET_ACTIVITY "com/dts/freefireth/FFMainActivity"

namespace CFG {
    // 1) funcion que decide la parte del cuerpo del disparo -> headshot
    inline const char *HS_CLASS   = "Player";
    inline const char *HS_METHOD  = "GetHitPart";
    inline int         HS_ARGC    = 0;

    // 2) funcion por frame del jugador -> aim lock
    inline const char *AIM_CLASS  = "Player";
    inline const char *AIM_METHOD = "LateUpdate";
    inline int         AIM_ARGC   = 0;

    // 3) funcion que devuelve el retroceso -> no recoil
    inline const char *REC_CLASS  = "Weapon";
    inline const char *REC_METHOD = "GetRecoil";
    inline int         REC_ARGC   = 0;
}
