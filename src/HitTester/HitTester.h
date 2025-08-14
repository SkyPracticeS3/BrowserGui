#pragma once
#include <Windows.h>
#include <memory>
#include <vector>

class GuiElement; 

class HitTester {
public:
    static std::shared_ptr<GuiElement> getTopElementAtPos(std::shared_ptr<GuiElement>& elms,
        POINT pos);
};