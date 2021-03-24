
#include <iostream>
#include <string>
#include <vector>
#include "lsd_methods.h"

int main() {
    /*
     * Баги : 117 - Cправа не пропадает один отрезок из 5, если не юзать сшиватель. Видимо сшитый регион ведёт себя хуже во время сплита, чем не сшитый.
     * Может быть в таких случаях сшивать всё вместе?(все 3 региона: один верхний и 2 нижних, а уже потом на них делать сплит).
     * Это очень тонкий баг. Надо думать
     */

    std::vector<float> threshes = {0.70,0.72,0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.8, 0.81};//0.74 - max
    //evaluation(threshes, 462);
    int test_image = 117;
    //draw_all_regions_of_img(test_image);
    compute_and_draw_image(test_image, 0.74, 256);

    //speed_test(test_image, 500);

    return 0;
}
