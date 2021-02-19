//
// Created by leonid on 14.02.2021.
//

#ifndef LINE_EXTRACTER_REGION_GROWING_H
#define LINE_EXTRACTER_REGION_GROWING_H

#include "opencv2/core/core.hpp"
#include <vector>
#include "map"
#include <numeric>
#include <algorithm>
class region_growing {
public:
    region_growing(int r, float tau, float deviation_thresh, int min_region_size);
    static std::map<int, std::vector<std::pair<int, int>>> get_regions(const cv::Mat& cls_map, const cv::Mat& angle_map, const cv::Mat& cls_bin);
private:
    std::vector<std::pair<int, int>> region_grouping(std::pair<int, int> root);
    std::vector<std::pair<int, int>> get_r_neighborhood(std::pair<int, int> pixel);
    int r;
    float tau;
    float deviation_thresh;
    int min_region_size;
};


#endif //LINE_EXTRACTER_REGION_GROWING_H
