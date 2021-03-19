
#include <iostream>
#include <string>
#include <vector>
#include "lsd_methods.h"

int main() {
    /*
     * Баги : 117 - перекос слева однозначно вызван сшивателем отрезков. Без него всё норм, а также справа не пропадает один отрезок из 5, если не юзать сшиватель.
     *
     */

    std::vector<float> threshes = {0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.8, 0.81};
    int test_image = 117;
    //draw_all_regions_of_img(test_image);
    compute_and_draw_image(test_image, 0.73, 256);
    //speed_test(test_image, 100);

    return 0;
}
