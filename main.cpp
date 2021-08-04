
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include "map"
#include "lsd_methods.h"

int main() {
    /*
     * Баги : 117 - Cправа не пропадает один отрезок из 5, если не юзать сшиватель. Видимо сшитый регион ведёт себя хуже во время сплита, чем не сшитый.
     * Может быть в таких случаях сшивать всё вместе?(все 3 региона: один верхний и 2 нижних, а уже потом на них делать сплит).
     * Это очень тонкий баг. Надо думать (ИСПРАВЛЕН)
     * Баг в 251: первый пиксель, который мы не учитываем при формировании отображений, оказался существенным.
     */

//    std::vector<float> threshes_for_cpp_param_search = {0.69, 0.70, 0.705, 0.71,
//                                                        0.715, 0.72, 0.73, 0.74};

//    std::vector<std::string> model_names = {
//            "t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1_selected_test",
//            "t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1_wireframe_test",
//            "t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1_york"
//    };
//    std::vector<int>dataset_sizes{20, 462, 102};
//
//    std::string models_predictions_path = R"(C:\home\lsd_local\raw_predictions\)";


    std::vector<std::string> models_predictions_paths = {
            R"(t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1\birdseye_imgs)",
            R"(t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1\imgs)",
            R"(t3mul2,4,8size288nf16par501,762reg0C1lt1ac30af1\town)",
    };
    std::vector<int>dataset_sizes{55, 131, 14};

    std::string models_predictions_path = R"(C:\home\lsd_local\raw_predictions\)";
    //std::vector<float> threshes_for_cpp_param_search = {0.36, 0.38, 0.4, 0.42, 0.44, 0.5, 0.52, 0.54, 0.56, 0.58, 0.6, 0.64};
    std::vector<float> threshes_for_cpp_param_search = {0.6, 0.62, 0.64, 0.66, 0.68, 0.70};
    std::map<std::string, float> algorithm_params{
                    {"global thresh",    0.4},
                    {"C",                2},
                    {"block size",       11},
                    {"tau",              0.05},
                    {"deviation thresh", 0.3},
                    {"min region size",  10},
                    {"bandwidth",        1},
                    {"bins size",        15},
                    {"discrete size",    15},
                    {"order",            1}
            };

    for (int i = 0; i < models_predictions_paths.size(); ++i) {
        std::string conrecte_pred_path = models_predictions_paths[i];
        int  dataset_size = dataset_sizes[i];
        const std::string dataset_path = models_predictions_path + conrecte_pred_path;
        evaluation(threshes_for_cpp_param_search, dataset_size, dataset_path, algorithm_params);
    }





    int test_image = 0;
    //compute_and_draw_image(test_image, 0.71, dataset_path, algorithm_params);


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


//    double_t sum_time = 0;
//    int N = 100;
//
//    int num_test_images = 50;
//    std::cin.ignore();
//    for (int i = 0; i < num_test_images; ++i) {
//        double_t time = speed_test(i, dataset_path, N, algorithm_params);
//        sum_time+=time;
//    }
//    std::cout << "prediction time per image = "
//              << sum_time/num_test_images
//              << "[mu_s]" << std::endl;


    return 0;
}
