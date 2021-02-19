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

    std::map<int, std::vector<std::vector<int>>>
    get_regions(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin);

private:
    std::vector<std::vector<int>>
    region_grouping(int root[2], const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin,
                    std::vector<std::vector<bool>> &U) const;

    int get_r_neighborhood(int x, int y, int **neighborhood, const cv::Mat &cls_bin,
                           std::vector<std::vector<bool>> &U) const;

    int r;
    float tau;
    float deviation_thresh;
    int min_region_size;
};


#endif //LINE_EXTRACTER_REGION_GROWING_H
