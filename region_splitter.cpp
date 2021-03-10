//
// Created by leonid on 14.02.2021.
//


#include "region_splitter.h"

region_splitter::region_splitter(int n_bins, float pool_depth, float pool_tolerance) : n_bins(n_bins),
                                                                                       pool_depth(pool_depth),
                                                                                       pool_tolerance(
                                                                                               pool_tolerance) {

}

std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>>
region_splitter::get_splitted_regions_with_conf_and_weighted_mean(const std::vector<std::vector<int>> &region,
                                                                  const std::vector<std::vector<float>> &new_coords,
                                                                  const cv::Mat &cls_map) const {
    /* Here we return list of splitted regions (std::vector<std::vector<int>> 2 x region_len).
     * first in pair contains confidence and region_mean for each region in the list
     * it has shape : 2 x number_of_regions
     */

    std::vector<float> y = new_coords[1];
    std::vector<int> idx(y.size());
    iota(idx.begin(), idx.end(), 0);
    //argsort
    stable_sort(idx.begin(), idx.end(),
                [&y](size_t i1, size_t i2) { return y[i1] < y[i2]; });
    //y sort using argsort
    for (int i = 0; i < y.size(); ++i) {
        y[i] = new_coords[1][idx[i]];
    }

    std::vector<float> hist(n_bins);

    uint n = y.size();
    float delta_y = (y[n - 1] - y[0]) / n_bins + 0.000001;
    float r = y[0] + delta_y;
    int j = 0;

    //МОЖНО УСКОРИТЬ, ЕСЛИ ИСПОЛЬЗОВАТЬ БИН ПОИСК
    for (int i = 0; i < n; ++i) {
        if (y[i] > r) {
            j++;
            r += delta_y;
        }
        hist[j]++;
    }
    for (int i = 0; i <n_bins; ++i) {
        hist[i]/=y.size();
    }
    //here we will find local minimums of hist
    //we start the check from sliding_window_size th bin

    std::vector<float> minimums;
    float prev_value = hist[0];
    int i = 1;
//    if (n == 85){
//        int x = 1;
//    }
    while (i < n_bins) {
        int k = 0;

        if (hist[i] < prev_value - pool_depth) {
            k = i;
            //here we start pool area
            i++;
            while (i != n_bins and hist[i] - hist[i - 1] > pool_tolerance) {
                i++;
            }
            if (i != n_bins) {
                minimums.push_back(y[0] + delta_y * (i + k) / 2);
            }
        }
        prev_value = hist[i];
        i++;
    }
//    if (minimums.size() == 1) {
//        int x = 1;
//    }
//     if around of our bin_i exists bin_j such that bim_j.value <= bin_i.value, then bin_i is not min
//    for (int i = sliding_window_size; i < hist.size() - sliding_window_size; ++i) {
//        bool i_is_min = true;
//        int i_count = hist[i];
//        for (int k = -sliding_window_size; k <= sliding_window_size; ++k) {
//            if (k == 0) continue;
//            if (hist[i + k] - diff_tolerance <= i_count) {
//                i_is_min = false;
//                break;
//            }
//        }
//        if (i_is_min)minimums.push_back(y[0] + i * delta_y + delta_y / 2);
//    }

    std::vector<std::vector<std::vector<float>>> splitted_regions(minimums.size() + 1,
                                                                  std::vector<std::vector<float>>(2));

    std::vector<std::vector<float>> y_means_and_conf(2, std::vector<float>(minimums.size() + 1));

    minimums.push_back(INFINITY);


    float rm = minimums[0];
    j = 0;
    for (int i = 0; i < y.size(); ++i) {
        if (y[i] > rm) {
            j++;
            rm = minimums[j];
        }
        int index = idx[i];
        splitted_regions[j][0].push_back(new_coords[0][index]);
        splitted_regions[j][1].push_back(new_coords[1][index]);
        float conf = cls_map.at<float>(region[0][index], region[1][index]);
        y_means_and_conf[0][j] += (y[i] * conf);
        y_means_and_conf[1][j] += conf;
    }

    for (int i = 0; i < y_means_and_conf[0].size(); ++i) {
        y_means_and_conf[0][i] /= y_means_and_conf[1][i];
        y_means_and_conf[1][i] /= splitted_regions[i][0].size();
    }

    std::pair<std::vector<std::vector<float>>, std::vector<std::vector<std::vector<float>>>> ret(y_means_and_conf,
                                                                                                 splitted_regions);
    return ret;
}