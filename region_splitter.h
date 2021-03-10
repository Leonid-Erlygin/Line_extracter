//
// Created by leonid on 14.02.2021.
//

#ifndef LINE_EXTRACTER_REGION_SPLITTER_H
#define LINE_EXTRACTER_REGION_SPLITTER_H


#include <vector>
#include <string>
#include <algorithm>
#include "opencv2/core/core.hpp"
#include <numeric>
class region_splitter {
public:
    region_splitter(int n_bins, float , float);

    std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>>
    get_splitted_regions_with_conf_and_weighted_mean(const std::vector<std::vector<int>> &region , const std::vector<std::vector<float>> &new_coords, const cv::Mat &cls_map) const;

private:
    int n_bins ;
    float pool_tolerance;
    float pool_depth;
};


#endif //LINE_EXTRACTER_REGION_SPLITTER_H
