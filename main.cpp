
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include "lsd_methods.h"

int main() {
    /*
     * Баги : 117 - Cправа не пропадает один отрезок из 5, если не юзать сшиватель. Видимо сшитый регион ведёт себя хуже во время сплита, чем не сшитый.
     * Может быть в таких случаях сшивать всё вместе?(все 3 региона: один верхний и 2 нижних, а уже потом на них делать сплит).
     * Это очень тонкий баг. Надо думать (ИСПРАВЛЕН)
     * Баг в 251: первый пиксель, который мы не учитываем при формировании отображений, оказался существенным.
     */


    const std::string dataset_name = "selected_test";
    std::vector<float> threshes = {0.70, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78};//0.74 - max
    //evaluation(threshes, 20, dataset_name);
    int test_image = 5;
    //draw_all_regions_of_img(test_image);
    //compute_and_draw_image(test_image, 0.74, 256, dataset_name);
    size_t sum_time = 0;

    int max_image = -1;
    std::cin.ignore();
    std::cout <<  speed_test(test_image, dataset_name, 500);
    //std::cout<<1<<'\n';
//    for (int i = 0; i < 20; ++i) {
//        size_t time = speed_test(i, dataset_name, 500);
//        sum_time+=time;
//    }
//    std::cout << "Time difference = "
//              << sum_time/20
//              << "[mu_s]" << std::endl;
//    std::cout<<max_image << std::endl;

    return 0;
}
