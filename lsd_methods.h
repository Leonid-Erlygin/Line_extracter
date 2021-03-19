//
// Created by leonid on 16.03.2021.
//

#ifndef LINE_EXTRACTER_LSD_METHODS_H
#define LINE_EXTRACTER_LSD_METHODS_H
void evaluation(std::vector<float> &threshes, int data_set_size);
void speed_test(int img_index, int N);
void compute_and_draw_image(int img_index, float thresh,int img_size);
void draw_all_regions_of_img(int img_index);
std::string type2str(int type);

#endif //LINE_EXTRACTER_LSD_METHODS_H
