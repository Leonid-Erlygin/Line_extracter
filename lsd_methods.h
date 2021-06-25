//
// Created by leonid on 16.03.2021.
//

#ifndef LINE_EXTRACTER_LSD_METHODS_H
#define LINE_EXTRACTER_LSD_METHODS_H
void evaluation(std::vector<float> &threshes, int data_set_size, const std::string& dataset_name, std::map<std::string, float>& algorithm_params);
double_t speed_test(int img_index, const std::string &dataset_name, int N, std::map<std::string, float>& algorithm_params);
void compute_and_draw_image(int img_index, float thresh, const std::string& dataset_name, std::map<std::string, float>& algorithm_params);
void draw_all_regions_of_img(int img_index);
std::string type2str(int type);

#endif //LINE_EXTRACTER_LSD_METHODS_H
