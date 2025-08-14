#include "ColorAnimation.h"

bool operator>=(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second){
    return first.r >= second.r && first.g >= second.g && first.b >= second.b && first.a >= second.a;
}

bool operator<(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second) {
    return first.r < second.r && first.g < second.g && first.b < second.b && first.a < second.a;
};
bool operator>(const D2D1_COLOR_F& first, const D2D1_COLOR_F& second) {
    return first.r > second.r && first.g > second.g && first.b > second.b && first.a > second.a;
};