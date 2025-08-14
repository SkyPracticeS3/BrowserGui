#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <minwinbase.h>
#include <synchapi.h>
#include <threadpoollegacyapiset.h>
#include <winnt.h>

struct AnimationInfo {
    HANDLE timer_handle;
    D2D1_COLOR_F target_clr;
    D2D1_COLOR_F* current_clr;
    HWND target_hwnd;
    float r_increment;
    float g_increment;
    float b_increment;
    float a_increment;
    CRITICAL_SECTION* crit_sect;
    bool target_bigger;
};

bool operator>=(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second);
bool operator<(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second);
bool operator>(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second);

class ColorAnimation {
public:
    CRITICAL_SECTION critical_section;
    ColorAnimation(){
        InitializeCriticalSection(&critical_section);
    }
    static void doAnimation(void* anim_info, BOOLEAN){
        AnimationInfo* info = static_cast<AnimationInfo*>(anim_info);
        EnterCriticalSection(info->crit_sect);

        info->current_clr->r += info->r_increment;
        info->current_clr->g += info->g_increment;
        info->current_clr->b += info->b_increment;
        info->current_clr->a += info->a_increment;

        if(info->target_bigger && info->target_clr > *info->current_clr){
            delete info;
            DeleteTimerQueueTimer(NULL, info->timer_handle, NULL);
        }
        else if(!info->target_bigger && info->target_clr < *info->current_clr){
            delete info;
            DeleteTimerQueueTimer(NULL, info->timer_handle, NULL);
        }

        LeaveCriticalSection(info->crit_sect);
    }
    void animate(D2D1_COLOR_F target_clr, D2D1_COLOR_F* current_clr, HWND target_hwnd,
        float duration) {
        float frame_rate = 30;
        float r_increment = ((target_clr.r - current_clr->r) / frame_rate) * duration;
        float g_increment = ((target_clr.g - current_clr->g) / frame_rate) * duration;
        float b_increment = ((target_clr.b - current_clr->b) / frame_rate) * duration;
        float a_increment = ((target_clr.a - current_clr->a) / frame_rate) * duration;
        HANDLE new_timer;
        AnimationInfo* info = new AnimationInfo({new_timer, target_clr, 
            current_clr, target_hwnd, r_increment, g_increment, b_increment,
                a_increment, &critical_section, target_clr > *current_clr});
        
        CreateTimerQueueTimer(&new_timer, NULL, &ColorAnimation::doAnimation,
            (void*)info, 0, 1000 / frame_rate, NULL);
    };
    ~ColorAnimation() {
        DeleteCriticalSection(&critical_section);
    }
};