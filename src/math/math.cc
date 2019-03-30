#include "math.h"

double math::angle::to_radians(double degrees) {
    return degrees * PI / 180.0;
}

double math::angle::to_degrees(double radians) {
    return radians * 180.0 / PI;
}
