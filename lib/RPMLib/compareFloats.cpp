#include "compareFloats.h"
#include <cmath>


// stolen from https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
bool compareFloats(float A, float B, float maxRelDiff) {
    float diff = std::fabs(A - B);
    A = std::fabs(A);
    B = std::fabs(B);
    float largest = (B > A) ? B : A;

    if (diff <= largest * maxRelDiff)
        return true;
    return false;
}
