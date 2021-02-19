//
// Created by leonid on 14.02.2021.
//

#include "region_growing.h"


region_growing::region_growing(int r, float tau, float deviation_thresh, int min_region_size) :
        r(r),
        tau(tau),
        deviation_thresh(deviation_thresh),
        min_region_size(min_region_size) {
}


std::map<int, std::vector<std::pair<int, int>>>
region_growing::get_regions(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin) {
    std::map<int, std::vector<std::pair<int, int>>> regions;

    std::vector<std::vector<int>> G(2);
    std::vector<float> D;
    for (int i = 0; i < cls_bin.rows; ++i) {
        for (int j = 0; j < cls_bin.cols; ++j) {
            if (cls_bin.at<int>(i, j) == 255) {
                G[0].push_back(i); // TO OPTIMISE . maybe we can preallocate G
                G[1].push_back(j);
                D.push_back(cls_map.at<float>(i, j));

            }
        }
    }
    // here we need to do argsort of D

    // initialize original index locations
    std::vector<size_t> idx(D.size());
    iota(idx.begin(), idx.end(), 0);
    // sort indexes based on comparing values in v
    // using std::stable_sort instead of std::sort
    // to avoid unnecessary index re-orderings
    // when v contains elements of equal values
    stable_sort(idx.begin(), idx.end(),
                [&D](size_t i1, size_t i2) { return D[i1] < D[i2]; });

    std::vector<std::vector<int>> S(2, std::vector<int>(D.size()));

    for (size_t i = 0; i < idx.size(); i++) {
        int idx_i = idx[i];
        S[0][i] = G[0][idx_i];
        S[1][i] = G[1][idx_i];
    }


}

std::vector<std::pair<int, int>> region_growing::region_grouping(std::pair<int, int> root) {

}

std::vector<std::pair<int, int>> region_growing::get_r_neighborhood(std::pair<int, int> pixel) {

}