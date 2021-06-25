
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


    int dataset_size = 20;
    //const std::string dataset_path = R"(C:\home\lsd_local\datasets\york)";
    //std::vector<float> threshes = {0.59, 0.62, 0.65, 0.68, 0.71, 0.74, 0.77, 0.8};
    //std::vector<float> threshes_for_cpp_param_search = {0.69, 0.70, 0.705, 0.71,
                                                        //0.715, 0.72, 0.73, 0.74,
                                                        //0.75, 0.76, 0.78, 0.79,
                                                        //0.8, 0.81, 0.82, 0.83};
    std::vector<float> threshes_for_cpp_param_search = {0.69, 0.70, 0.705, 0.71,
                                                        0.715, 0.72, 0.73, 0.74};
                                                        

    //std::vector<float> global_threshes = {0.01, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
    //std::vector<float> taus = {0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8,
                               //0.9};
    //std::vector<float> deviation_threshs = {0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4};//{0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
    std::vector<float> bins_sizes = {3, 5, 7, 9, 11, 13, 15};
    std::vector<float> discrete_sizes = {3, 5, 7, 9, 11, 13, 15};
    
    //std::vector<float> taus = {1};
    std::vector<std::string> model_names = {
            "t1mul2,4,8size288nf16par547,954C1lt1ac30af1",
    };
    //int test_image = 4; //00035608
    int test_image = 4;
    std::string models_predictions_path = R"(C:\home\lsd_local\raw_predictions_selected_test\)";
    //std::string models_predictions_path = R"(C:\home\lsd_local\test_images\)";
    
    
    std::map<std::string, float> algorithm_params{
                    {"global thresh",    0.4},
                    {"C",                2},
                    {"block size",       11},
                    {"tau",              0.1},
                    {"deviation thresh", 0.3},
                    {"min region size",  10},
                    {"bandwidth",        1},
                    {"bins size",        15},
                    {"discrete size",    15},
                    {"order",            1}
            };
    std::string &model_name = model_names[0];
    const std::string dataset_path = models_predictions_path + model_name;

    compute_and_draw_image(test_image, 0.705, dataset_path, algorithm_params);

    //evaluation(threshes_for_cpp_param_search, dataset_size, dataset_path, algorithm_params);
            
//    for (const std::string &model_name : model_names) {
//        const std::string dataset_path = models_predictions_path + model_name;
//        for (float discrete_size:discrete_sizes){
//        for (float bins_size:bins_sizes) {
//            std::map<std::string, float> algorithm_params{
//                    {"global thresh",    0.4},
//                    {"C",                2},
//                    {"block size",       11},
//                    {"tau",              0.1},
//                    {"deviation thresh", 0.3},
//                    {"min region size",  10},
//                    {"bandwidth",        1},
//                    {"bins size",        bins_size},
//                    {"discrete size",    discrete_size},
//                    {"order",            1}
//            };
//            //compute_and_draw_image(test_image, 0.705, dataset_path, algorithm_params);
//            evaluation(threshes_for_cpp_param_search, dataset_size, dataset_path, algorithm_params);
//        	}
//	}
//        //compute_and_draw_image(test_image, 0.75, dataset_path, algorithm_params);
//    }

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
