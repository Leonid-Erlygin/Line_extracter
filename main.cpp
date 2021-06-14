
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include "lsd_methods.h"

int main() {
    /*
     * Баги : 117 - Cправа не пропадает один отрезок из 5, если не юзать сшиватель. Видимо сшитый регион ведёт себя хуже во время сплита, чем не сшитый.
     * Может быть в таких случаях сшивать всё вместе?(все 3 региона: один верхний и 2 нижних, а уже потом на них делать сплит).
     * Это очень тонкий баг. Надо думать (ИСПРАВЛЕН)
     * Баг в 251: первый пиксель, который мы не учитываем при формировании отображений, оказался существенным.
     */


    int dataset_size = 462;
    //const std::string dataset_path = R"(C:\home\lsd_local\datasets\york)";
    //std::vector<float> threshes = {0.68,0.69,0.70, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78};//0.74 - max
    std::vector<float> threshes = {0.59, 0.62, 0.65, 0.68, 0.71, 0.74, 0.77, 0.8};

    std::vector<std::string> model_names = {
            "t1mul2,4,8size288nf16par547,954C1lt1ac30af1",
            //"t1mul2,3,5size288nf25par742,177C1lt1ac30af1",
            "t1mul3,4,5size288nf16par471,778C1lt1ac30af1",
            "t1mul3,4,5size288nf20par735,882C1lt1ac30af1",
            "t1mul4,4,5size288nf16par559,666C1lt1ac30af1"
    };
    for (const std::string &model_name : model_names) {
        //std::string s = std::to_string(size);
        //std::string model_name = "t1mul2,4,8,10size"+s+"nf16par1,592,370C1lt1ac30af1";
        std::cout<<model_name<<'\n';
        const std::string dataset_path = R"(C:\home\lsd_local\raw_predictions\)" + model_name;
        int test_image = 17;
        compute_and_draw_image(test_image, 0.74, dataset_path);
        //evaluation(threshes, dataset_size, dataset_path);
    }

//    std::string model_name = "t1mul2,4,8,10size368nf16par1,592,370C1lt1ac30af1";
//    const std::string dataset_path = R"(C:\home\lsd_local\raw_predictions\)"+ model_name;
    //std::cout<<dataset_path<<'\n';
    //evaluation(threshes, dataset_size, dataset_path);

//    std::cin.ignore();
//    for (int size:sizes) {
//        std::string s = std::to_string(size);
//        std::string model_name = "t1mul2,4,8,10size"+s+"nf16par1,592,370C1lt1ac30af1";
//        const std::string dataset_path = R"(C:\home\lsd_local\raw_predictions\)"+ model_name;
//        double_t sum_time = 0;
//        for (int i = 0; i < dataset_size; ++i) {
//            double_t time = speed_test(i, dataset_path, 50);
//            sum_time+=time;
//        }
//        std::cout << "Time difference on " + s << " time: "
//              << sum_time/dataset_size
//              << "[mu_s]" << " FPS cpp is " << 1000000/(sum_time/dataset_size)<< std::endl;
//
//    }

    size_t sum_time = 0;

    int max_image = -1;
//    std::cin.ignore();
    //std::cout <<  speed_test(test_image, dataset_path, 500);
    //std::cout<<1<<'\n';
//    for (int i = 0; i < 20; ++i) {
//        size_t time = speed_test(i, dataset_path, 500);
//        sum_time+=time;
//    }
//    std::cout << "Time difference = "
//              << sum_time/20
//              << "[mu_s]" << std::endl;
//    std::cout<<max_image << std::endl;

    return 0;
}
