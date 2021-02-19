//
// Created by leonid on 14.02.2021.
//

#include "region_growing.h"
#include <cmath>

region_growing::region_growing(int r, float tau, float deviation_thresh, int min_region_size) :
        r(r),
        tau(tau),
        deviation_thresh(deviation_thresh),
        min_region_size(min_region_size) {
}


std::map<int, std::vector<std::vector<int>>>
region_growing::get_regions(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin) {
    std::map<int, std::vector<std::vector<int>>> regions;

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

    std::vector<std::vector<bool>> U(cls_map.rows, std::vector<bool>(cls_map.cols));

    int i = 0;
    while (!S[0].empty()) {
        i++;

        int root[2];
        root[0] = S[0].back();
        root[1] = S[1].back();
        S[0].pop_back();
        S[1].pop_back();

        while (U[root[0]][root[1]]) {
            if (S[0].empty()) break;

            root[0] = S[0].back();
            root[1] = S[1].back();
            S[0].pop_back();
            S[1].pop_back();
        }
        if (S[0].empty()) break;
        std::vector<std::vector<int>> region = region_grouping(root, cls_map, angle_map, cls_bin, U);

        if (region[0].size() > min_region_size) {

            regions[i] = region;
        } else i--;
    }
    return regions;
}

std::vector<std::vector<int>>
region_growing::region_grouping(int root[2], const cv::Mat &cls_map, const cv::Mat &angle_map,
                                const cv::Mat &cls_bin, std::vector<std::vector<bool>> &U) const {
    std::vector<std::vector<int>> region(2);
    double Theta_region = angle_map.at<double>(root[0], root[1]) * M_PI;
    double region_mean = cls_map.at<double>(root[0], root[1]);
    double Vx = cos(Theta_region);
    double Vy = sin(Theta_region);

    region[0].push_back(root[0]);
    region[1].push_back(root[1]);

    int neighborhood_max_size = (2 * r + 1) * (2 * r + 1);

    std::vector<std::vector<int>> newly_added(2);
    newly_added[0].push_back(root[0]);
    newly_added[1].push_back(root[1]);
    U[root[0]][root[1]] = true;
    int region_len = 1;

    int **neighborhood;
    neighborhood = new int *[2];
    int neighborhood_size = 0;
    for (int i = 0; i < 2; ++i) {
        neighborhood[i] = new int[neighborhood_max_size];
    }


    while (!newly_added[0].empty()) {

        neighborhood_size = get_r_neighborhood(newly_added[0].back(), newly_added[1].back(), neighborhood, cls_bin, U);
        newly_added[0].pop_back();
        newly_added[1].pop_back();

        for (int j = 0; j < neighborhood_size; ++j) {
            int x = neighborhood[0][j];
            int y = neighborhood[1][j];
            double ang = angle_map.at<double>(x, y);
            double probability = cls_map.at<double>(x, y);
            if (2 * (1 - cos(2 * (ang * M_PI - Theta_region)))
                < tau and
                abs(region_mean - probability) < deviation_thresh) {
                region[0].push_back(x);
                region[1].push_back(y);
                newly_added[0].push_back(x);
                newly_added[1].push_back(y);

                U[x][y] = true;
                Vx += cos(ang * M_PI);
                Vy += sin(ang * M_PI);
                Theta_region = acos(Vx / sqrt(Vx * Vx + Vy * Vy));
                region_len++;
                region_mean = (region_mean * (region_len - 1) + probability) / region_len;
            }
        }


    }

//    if (region[0].size() > min_region_size) {
//        int b = 9;
//    }
    return region;
}

int region_growing::get_r_neighborhood(int x, int y, int **neighborhood, const cv::Mat &cls_bin,
                                       std::vector<std::vector<bool>> &U) const {
    assert(r == 1);
    int neighborhood_size = 0;
    int max_size = cls_bin.cols;
    if (x - 1 >= 0) {
        if (y - 1 >= 0 and cls_bin.at<bool>(x - 1, y - 1) and !U[x - 1][y - 1]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y - 1;
            neighborhood_size++;
        }
        if (cls_bin.at<bool>(x - 1, y) and !U[x - 1][y]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y;
            neighborhood_size++;
        }
        if (y + 1 < max_size and cls_bin.at<bool>(x - 1, y + 1) and !U[x - 1][y + 1]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y + 1;
            neighborhood_size++;
        }
    }
    if (y - 1 >= 0 and cls_bin.at<bool>(x, y - 1) and !U[x][y - 1]) {
        neighborhood[0][neighborhood_size] = x;
        neighborhood[1][neighborhood_size] = y - 1;
        neighborhood_size++;
    }
    if (y + 1 < max_size and cls_bin.at<bool>(x, y + 1) and !U[x][y + 1]) {
        neighborhood[0][neighborhood_size] = x;
        neighborhood[1][neighborhood_size] = y + 1;
        neighborhood_size++;
    }
    if (x + 1 < max_size) {
        if (y - 1 >= 0 and cls_bin.at<bool>(x + 1, y - 1) and !U[x + 1][y - 1]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y - 1;
            neighborhood_size++;
        }
        if (cls_bin.at<bool>(x + 1, y) and !U[x + 1][y]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y;
            neighborhood_size++;
        }
        if (y + 1 < max_size and cls_bin.at<bool>(x + 1, y + 1) and !U[x + 1][y + 1]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y + 1;
            neighborhood_size++;
        }
    }
    return neighborhood_size;
}