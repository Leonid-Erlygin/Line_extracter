//
// Created by leonid on 14.02.2021.
//

#include "region_growing.h"
#include <cmath>
#include <iostream>
#include "queue"

#define SAW 1

region_growing::region_growing(int r, float tau, float deviation_thresh, int min_region_size) :
        r(r),
        tau(tau),
        deviation_thresh(deviation_thresh),
        min_region_size(min_region_size) {
}

void region_growing::fill_horiz_and_vert(std::vector<std::vector<std::vector<int>>> const &regions,
                                         std::vector<std::vector<int>> &vert,
                                         std::vector<std::vector<int>> &horiz, int length, int x_shift, int y_shift,
                                         int horiz_idx, int vert_idx) {
    for (int i = 0; i < regions.size(); ++i) {
        int n = regions[i][0].size();
        for (int j = 0; j < n; ++j) {
            int x = regions[i][0][j];
            int y = regions[i][1][j];
            if (x == length - x_shift) {
                horiz[horiz_idx][y] = i + 1;
                int p = 1;
            }
            if (y == length - y_shift) {
                vert[vert_idx][x] = i + 1;
            }
        }
    }
}

void
region_growing::find_mappings(int i, std::map<int, int> &mapping, int length, const std::vector<std::vector<int>> &line,
                              const std::vector<std::vector<float>> &regions_first_mean_angle,
                              const std::vector<std::vector<float>> &regions_second_mean_angle,
                              bool first_index) const {

    int j;
    int k;
    int l;
    while (i < length) {
        while (i != length and !line[first_index][i]) {
            i++;
        }
        if (i == length)break;
        int i_index = line[first_index][i];
        j = i + 1;

        while (j != length and line[first_index][j] == i_index) {
            j++;
        }
        k = i - 1;
        while (k != length and !line[!first_index][k]) {
            k++;
        }
        if (k == length)break;
        int k_index = line[!first_index][k];
        l = k + 1;
        while (l != length and line[!first_index][l] == k_index) {
            l++;
        }

        if (j - k >= 0 and l - i >= 0) {
            int first_region_index = line[first_index][i] - 1;
            int second_region_index = line[!first_index][k] - 1;
            float V_mean_x_first = regions_first_mean_angle[0][first_region_index];
            float V_mean_y_first = regions_first_mean_angle[1][first_region_index];
            float V_mean_x_second = regions_second_mean_angle[0][second_region_index];
            float V_mean_y_second = regions_second_mean_angle[1][second_region_index];

            if ((V_mean_x_first - V_mean_x_second) * (V_mean_x_first - V_mean_x_second) +
                (V_mean_y_first - V_mean_y_second) * (V_mean_y_first - V_mean_y_second)
                < tau) {
                mapping[first_region_index] = second_region_index;
            }

        }
        i = j;
    }

}

void region_growing::compute_merge_map(const std::map<int, int> &mapping,
                                       std::map<int, std::pair<int, int>> &merge_map_target,
                                       std::map<int, std::pair<int, int>> &merge_map_origin) {

    for (auto const &x:mapping) {
        if (x.second == -1)continue;
        
        auto it = merge_map_origin.find(x.first);
        if (it != merge_map_origin.end()) {
            merge_map_target[x.second] = std::pair<int, int>(x.first, merge_map_origin[x.first].first);
            merge_map_origin[x.first].first = -1;
        } else {
            merge_map_target[x.second] = std::pair<int, int>(x.first, -1);
        }
    }
}

void region_growing::compute_merge_map_for_second(std::map<int, int> &third_to_first,
                                                  std::map<int, std::pair<int, int>> &merge_map_2,
                                                  const std::map<int, int> &first_to_second) {
    std::map<int, int> third_to_first_inv;
    for (auto const &x : third_to_first) {
        third_to_first_inv[x.second] = x.first;
    }
    for (auto const &x:first_to_second) {
        auto it = third_to_first_inv.find(x.first);
        if (it != third_to_first_inv.end()) {
            merge_map_2[x.second] = std::pair<int, int>(x.first, it->second);
            third_to_first[it->second] = -1; //do not need to use it again
        } else {
            merge_map_2[x.second] = std::pair<int, int>(x.first, -1);
        }
    }
}

void reassign_regions(std::map<int, std::pair<int, int>> &merge_map,
                      std::vector<std::vector<std::vector<int>>> &regions_target,
                      const std::vector<std::vector<std::vector<int>>> &regions_prev,
                      const std::vector<std::vector<std::vector<int>>> &regions_prev_prev
) {
    for (auto const &x:merge_map) {

        if (x.second.first == -1)continue;
        std::vector<std::vector<int>> &curr = regions_target[x.first];
        const std::vector<std::vector<int>> &prev = regions_prev[x.second.first];

        curr[0].insert(curr[0].end(), prev[0].begin(), prev[0].end());
        curr[1].insert(curr[1].end(), prev[1].begin(), prev[1].end());

        if (x.second.second == -1)continue;
        const std::vector<std::vector<int>> &prev_prev = regions_prev_prev[x.second.second];

        curr[0].insert(curr[0].end(), prev_prev[0].begin(), prev_prev[0].end());
        curr[1].insert(curr[1].end(), prev_prev[1].begin(), prev_prev[1].end());

    }
}

void merge_fixed_regions(std::vector<std::vector<std::vector<int>>> &regions, std::map<int, int> &mapping,
                         const std::vector<std::vector<std::vector<int>>> &regions_origin) {
    if (mapping.empty()) {
        regions.insert(regions.end(), regions_origin.begin(), regions_origin.end());
    } else {
        int size = mapping.size();
        int prev = 0;
        auto it = mapping.begin();
        for (int m = 0; m < size; ++m) {
            regions.insert(regions.end(), regions_origin.begin() + prev, regions_origin.begin() + it->first);
            prev = it->first + 1;
            it++;
        }
        regions.insert(regions.end(), regions_origin.begin() + prev, regions_origin.end());
    }
}

std::vector<std::vector<std::vector<int>>>
region_growing::get_regions(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin) {
    std::vector<std::vector<std::vector<int>>> regions1;
    std::vector<std::vector<std::vector<int>>> regions2;
    std::vector<std::vector<std::vector<int>>> regions3;
    std::vector<std::vector<std::vector<int>>> regions4;

    std::vector<std::vector<float>> regions1_mean_angle(2);
    std::vector<std::vector<float>> regions2_mean_angle(2);
    std::vector<std::vector<float>> regions3_mean_angle(2);
    std::vector<std::vector<float>> regions4_mean_angle(2);

    std::vector<std::vector<bool>> U(angle_map.rows, std::vector<bool>(angle_map.cols));

    std::thread th1(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions1), std::ref(regions1_mean_angle), std::ref(U), 0, 0, 0,
                    angle_map.rows / 2,
                    angle_map.cols / 2);
    std::thread th2(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions2), std::ref(regions2_mean_angle), std::ref(U), 0, 0,
                    angle_map.cols / 2, angle_map.rows / 2,
                    angle_map.cols);
    std::thread th3(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions3), std::ref(regions3_mean_angle), std::ref(U), 0,
                    angle_map.rows / 2, 0, angle_map.rows,
                    angle_map.cols / 2);

    //main_thread
    region_growing::get_regions_one_thread(cls_map, angle_map, cls_bin,
                                           regions4, std::ref(regions4_mean_angle),
                                           U, 0, angle_map.rows / 2, angle_map.cols / 2, angle_map.rows,
                                           angle_map.cols);
    th1.join();
    th2.join();
    th3.join();

    std::vector<std::vector<std::vector<int>>> regions;
    regions.reserve(regions1.size() + regions2.size() + regions3.size() + regions4.size());

#if SAW
    std::vector<std::vector<int>> vert(2, std::vector<int>(cls_map.rows));
    std::vector<std::vector<int>> horiz(2, std::vector<int>(cls_map.cols));
    int length = cls_map.rows / 2;
    int double_length = length * 2;

    //fill values
    fill_horiz_and_vert(regions1, vert, horiz, length, 1, 1, 0, 0);
    fill_horiz_and_vert(regions2, vert, horiz, length, 1, 0, 0, 1);
    fill_horiz_and_vert(regions3, vert, horiz, length, 0, 1, 1, 0);
    fill_horiz_and_vert(regions4, vert, horiz, length, 0, 0, 1, 1);

    //find mappings
    std::map<int, int> first_to_second;
    std::map<int, int> second_to_forth;
    std::map<int, int> forth_to_third;
    std::map<int, int> third_to_first;
//    for (int i = length; i < length * 2; ++i) {
//        std::cout << i << " : " << horiz[0][i] << " " << horiz[1][i] << '\n';
//    }
//    for (int i = 0; i < length; ++i) {
//        std::cout << i << " : " << horiz[0][i] << " " << horiz[1][i] << '\n';
//    }
    find_mappings(1, first_to_second, length, vert, regions1_mean_angle, regions2_mean_angle, false);
    find_mappings(length + 1, second_to_forth, double_length, horiz, regions2_mean_angle, regions4_mean_angle, false);
    find_mappings(length + 1, forth_to_third, double_length, vert, regions4_mean_angle, regions3_mean_angle, true);
    find_mappings(1, third_to_first, length, horiz, regions3_mean_angle, regions1_mean_angle, true);


    //here we compute merge_maps
    std::map<int, std::pair<int, int>> merge_map_1;
    std::map<int, std::pair<int, int>> merge_map_2;
    std::map<int, std::pair<int, int>> merge_map_3;
    std::map<int, std::pair<int, int>> merge_map_4;

    compute_merge_map_for_second(third_to_first, merge_map_2, first_to_second);
    compute_merge_map(second_to_forth, merge_map_4, merge_map_2);
    compute_merge_map(forth_to_third, merge_map_3, merge_map_4);
    compute_merge_map(third_to_first, merge_map_1, merge_map_3);

    //now we reassign regions according to maps above
    reassign_regions(merge_map_1, regions1, regions3, regions4);
    reassign_regions(merge_map_2, regions2, regions1, regions3);
    reassign_regions(merge_map_4, regions4, regions2, regions1);
    reassign_regions(merge_map_3, regions3, regions4, regions2);

    //and finally, we merge all regions, without deleted ones
    merge_fixed_regions(regions, first_to_second, regions1);
    merge_fixed_regions(regions, second_to_forth, regions2);
    merge_fixed_regions(regions, forth_to_third, regions4);
    merge_fixed_regions(regions, third_to_first, regions3);

#else
    regions.insert(regions.end(), regions1.begin(), regions1.end());
regions.insert(regions.end(), regions2.begin(), regions2.end());
regions.insert(regions.end(), regions3.begin(), regions3.end());
regions.insert(regions.end(), regions4.begin(), regions4.end());


#endif
    return regions;
}

void region_growing::get_regions_one_thread(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin,
                                            std::vector<std::vector<std::vector<int>>> &regions,
                                            std::vector<std::vector<float>> &regions_mean_angle,
                                            std::vector<std::vector<bool>> &U, int shift, int top_corner_x,
                                            int top_corner_y, int bot_corner_x, int bot_corner_y) {
    std::vector<std::vector<int>> G(2);
    std::vector<float> D;


    for (int i = top_corner_x; i < bot_corner_x; ++i) {
        for (int j = top_corner_y; j < bot_corner_y; ++j) {
            if (cls_bin.at<uchar>(i, j)) {
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
    int i = 0;
    int S_index = S[0].size() - 1;

    while (S_index >= 0) {
        i++;
        int root[2];
        root[0] = S[0][S_index];
        root[1] = S[1][S_index];
        S_index--;
        while (U[root[0]][root[1]]) {
            if (S_index < 0) break;

            root[0] = S[0][S_index];
            root[1] = S[1][S_index];
            S_index--;
        }
        if (S_index < 0) break;
        std::pair<std::vector<std::vector<int>>, std::pair<float, float>> region_and_angle = region_grouping(root,
                                                                                                             cls_map,
                                                                                                             angle_map,
                                                                                                             cls_bin, U,
                                                                                                             top_corner_x,
                                                                                                             top_corner_y,
                                                                                                             bot_corner_x,
                                                                                                             bot_corner_y);

        if (region_and_angle.first[0].size() > min_region_size) {
            regions.push_back(region_and_angle.first);
            regions_mean_angle[0].push_back(region_and_angle.second.first);
            regions_mean_angle[1].push_back(region_and_angle.second.second);
        } else i--;
    }

}

std::pair<std::vector<std::vector<int>>, std::pair<float, float>>
region_growing::region_grouping(int root[2], const cv::Mat &cls_map, const cv::Mat &angle_map,
                                const cv::Mat &cls_bin, std::vector<std::vector<bool>> &U, int top_corner_x,
                                int top_corner_y, int bot_corner_x, int bot_corner_y) const {
    std::vector<std::vector<int>> region(2);

    float region_mean = cls_map.at<float>(root[0], root[1]);
    float ang = angle_map.at<float>(root[0], root[1]) * M_PI;
    float cos0 = std::cos(ang);
    float sin0 = std::sin(ang);
    float V_mean_x = cos0 * cos0 - sin0 * sin0;
    float V_mean_y = 2 * sin0 * cos0;

    region[0].push_back(root[0]);
    region[1].push_back(root[1]);

    int neighborhood_max_size = (2 * r + 1) * (2 * r + 1);
    std::vector<std::queue<int>> newly_added(2);
    newly_added[0].push(root[0]);
    newly_added[1].push(root[1]);
    U[root[0]][root[1]] = true;
    int region_len = 1;

    int **neighborhood;
    neighborhood = new int *[2];
    int neighborhood_size = 0;
    for (int i = 0; i < 2; ++i) {
        neighborhood[i] = new int[neighborhood_max_size];
    }
    while (!newly_added[0].empty()) {

        neighborhood_size = get_r_neighborhood(newly_added[0].front(), newly_added[1].front(), neighborhood, cls_bin,
                                               U, top_corner_x, top_corner_y, bot_corner_x, bot_corner_y);
        newly_added[0].pop();
        newly_added[1].pop();

        for (int j = 0; j < neighborhood_size; ++j) {
            int x = neighborhood[0][j];
            int y = neighborhood[1][j];
            ang = angle_map.at<float>(x, y) * M_PI;
            cos0 = std::cos(ang);
            sin0 = std::sin(ang);
            float V_x = cos0 * cos0 - sin0 * sin0;
            float V_y = 2 * sin0 * cos0;

            float probability = cls_map.at<float>(x, y);
            if ((V_mean_x - V_x) * (V_mean_x - V_x) + (V_mean_y - V_y) * (V_mean_y - V_y)
                < tau and
                abs(region_mean - probability) < deviation_thresh) {
                region[0].push_back(x);
                region[1].push_back(y);
                newly_added[0].push(x);
                newly_added[1].push(y);

                U[x][y] = true;

                region_len++;
                V_mean_x = (V_mean_x * (region_len - 1) + V_x) / region_len;
                V_mean_y = (V_mean_y * (region_len - 1) + V_y) / region_len;

                region_mean = (region_mean * (region_len - 1) + probability) / region_len;
            }
        }
    }
    for (int i = 0; i < 2; ++i) {
        delete[] neighborhood[i];
    }
    delete[] neighborhood;

    std::pair<std::vector<std::vector<int>>, std::pair<float, float>> ret_value(region,
                                                                                std::pair<float, float>(V_mean_x,
                                                                                                        V_mean_y));
    return ret_value;
}

inline int region_growing::get_r_neighborhood(int x, int y, int **neighborhood, const cv::Mat &cls_bin,
                                              std::vector<std::vector<bool>> &U, int top_corner_x, int top_corner_y,
                                              int bot_corner_x, int bot_corner_y) const {
    assert(r == 1);
    //??? TO OPTIMISE . x - 1 >= 0 and so on is always true, due to the construction of cls_bin(no active borders)?
    int neighborhood_size = 0;
    int max_size = cls_bin.cols;
    if (x - 1 >= top_corner_x) {
        if (y - 1 >= top_corner_y and cls_bin.at<uchar>(x - 1, y - 1) and !U[x - 1][y - 1]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y - 1;
            neighborhood_size++;
        }
        if (cls_bin.at<uchar>(x - 1, y) and !U[x - 1][y]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y;
            neighborhood_size++;
        }
        if (y + 1 < bot_corner_y and cls_bin.at<uchar>(x - 1, y + 1) and !U[x - 1][y + 1]) {
            neighborhood[0][neighborhood_size] = x - 1;
            neighborhood[1][neighborhood_size] = y + 1;
            neighborhood_size++;
        }
    }
    if (y - 1 >= top_corner_y and cls_bin.at<uchar>(x, y - 1) and !U[x][y - 1]) {
        neighborhood[0][neighborhood_size] = x;
        neighborhood[1][neighborhood_size] = y - 1;
        neighborhood_size++;
    }
    if (y + 1 < bot_corner_y and cls_bin.at<uchar>(x, y + 1) and !U[x][y + 1]) {
        neighborhood[0][neighborhood_size] = x;
        neighborhood[1][neighborhood_size] = y + 1;
        neighborhood_size++;
    }
    if (x + 1 < bot_corner_x) {
        if (y - 1 >= top_corner_y and cls_bin.at<uchar>(x + 1, y - 1) and !U[x + 1][y - 1]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y - 1;
            neighborhood_size++;
        }
        if (cls_bin.at<uchar>(x + 1, y) and !U[x + 1][y]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y;
            neighborhood_size++;
        }
        if (y + 1 < bot_corner_y and cls_bin.at<uchar>(x + 1, y + 1) and !U[x + 1][y + 1]) {
            neighborhood[0][neighborhood_size] = x + 1;
            neighborhood[1][neighborhood_size] = y + 1;
            neighborhood_size++;
        }
    }
    return neighborhood_size;
}