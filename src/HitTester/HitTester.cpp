#include "HitTester.h"
#include "../GuiHirarichy/GuiElements.h"
#include <winuser.h>

std::shared_ptr<GuiElement> HitTester::getTopElementAtPos(std::shared_ptr<GuiElement>& elm,
    POINT pos) {
    auto deep_first_elements = elm->reverseFlatten();
    for(auto& element : deep_first_elements){
        RECT rc = element->getWin32Rc();
        if(PtInRect(&rc, pos))
            return element;
    }
    return nullptr;
};