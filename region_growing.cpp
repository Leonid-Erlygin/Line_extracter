//
// Created by leonid on 14.02.2021.
//

#include "region_growing.h"
#include <cmath>
#include <iostream>
#include "queue"

region_growing::region_growing(int r, float tau, float deviation_thresh, int min_region_size) :
        r(r),
        tau(tau),
        deviation_thresh(deviation_thresh),
        min_region_size(min_region_size) {
}


std::vector<std::vector<std::vector<int>>>
region_growing::get_regions(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin) {
    std::vector<std::vector<std::vector<int>>> regions1;
    std::vector<std::vector<std::vector<int>>> regions2;
    std::vector<std::vector<std::vector<int>>> regions3;
    std::vector<std::vector<std::vector<int>>> regions4;

    std::vector<std::vector<bool>> U(angle_map.rows, std::vector<bool>(angle_map.cols));

    std::thread th1(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions1), std::ref(U), 0, 0, 0, angle_map.rows / 2,
                    angle_map.cols / 2);
    std::thread th2(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions2), std::ref(U), 0, 0, angle_map.cols / 2, angle_map.rows / 2,
                    angle_map.cols);
    std::thread th3(&region_growing::get_regions_one_thread, this, std::ref(cls_map), std::ref(angle_map),
                    std::ref(cls_bin), std::ref(regions3), std::ref(U), 0, angle_map.rows / 2, 0, angle_map.rows,
                    angle_map.cols / 2);

    //main_thread
    region_growing::get_regions_one_thread(cls_map, angle_map, cls_bin,
                                           regions4,
                                           U, 0, angle_map.rows / 2, angle_map.cols / 2, angle_map.rows,
                                           angle_map.cols);
    th1.join();
    th2.join();
    th3.join();

    std::vector<std::vector<std::vector<int>>> regions;
    regions.reserve(regions1.size() + regions2.size() + regions3.size() + regions4.size());

    std::vector<std::vector<int>> vert(2, std::vector<int>(cls_map.rows));
    std::vector<std::vector<int>> horiz(2, std::vector<int>(cls_map.cols));

    int length = cls_map.rows / 2;
    int double_length = length * 2;
    for (int i = 0; i < regions1.size(); ++i) {
        int n = regions1[i][0].size();
        for (int j = 0; j < n; ++j) {
            int x = regions1[i][0][j];
            int y = regions1[i][1][j];
            if (x == length - 1) {
                horiz[0][y] = i + 1;
            }
            if (y == length - 1) {
                vert[0][x] = i + 1;
            }
        }
    }
    for (int i = 0; i < regions2.size(); ++i) {
        int n = regions2[i][0].size();
        for (int j = 0; j < n; ++j) {
            int x = regions2[i][0][j];
            int y = regions2[i][1][j];
            if (x == length - 1) {
                horiz[0][y] = i + 1;
            }
            if (y == length) {
                vert[1][x] = i + 1;
            }
        }
    }
    for (int i = 0; i < regions3.size(); ++i) {
        int n = regions3[i][0].size();
        for (int j = 0; j < n; ++j) {
            int x = regions3[i][0][j];
            int y = regions3[i][1][j];
            if (x == length) {
                horiz[1][y] = i + 1;
            }
            if (y == length - 1) {
                vert[0][x] = i + 1;
            }
        }
    }

    for (int i = 0; i < regions4.size(); ++i) {
        int n = regions4[i][0].size();
        for (int j = 0; j < n; ++j) {
            int x = regions4[i][0][j];
            int y = regions4[i][1][j];
            if (x == length) {
                horiz[1][y] = i + 1;
            }
            if (y == length) {
                vert[1][x] = i + 1;
            }
        }
    }

//    for (int i = 0; i <vert[0].size(); ++i) {
//        std::cout<<i<<": "<<vert[0][i]<<" "<<vert[1][i]<<'\n';
//    }
    int i = 1;
    int j = 0;
    int k = 0;
    int l = 0;

    std::map<int, int> first_to_second;
    while (i < length) {
        while (i != length and !vert[0][i]) {
            i++;
        }
        if (i == length)break;
        j = i + 1;

        while (j != length and vert[0][j]) {
            j++;
        }
        k = i - 1;
        while (k != length and !vert[1][k]) {
            k++;
        }
        if (k == length)break;
        l = k + 1;
        while (l != length and vert[1][l]) {
            l++;
        }

        if (j - k >= 0 and l - i >= 0) {
            first_to_second[vert[0][i] - 1] = vert[1][k] - 1;
        }
        i = j;
    }
    std::map<int, int> second_to_forth;

    i = length + 1;
    j = 0;
    k = 0;
    l = 0;

    while (i < double_length) {
        while (i != double_length and !horiz[0][i]) {
            i++;
        }
        if (i == double_length)break;
        j = i + 1;

        while (j != double_length and horiz[0][j]) {
            j++;
        }
        k = i - 1;
        while (k != double_length and !horiz[1][k]) {
            k++;
        }
        if (k == double_length)break;
        l = k + 1;
        while (l != double_length and horiz[1][l]) {
            l++;
        }

        if (j - k >= 0 and l - i >= 0) {
            second_to_forth[horiz[0][i] - 1] = horiz[1][k] - 1;
        }
        i = j;
    }
    std::map<int, int> forth_to_third;

    i = length + 1;
    j = 0;
    k = 0;
    l = 0;

    while (i < double_length) {
        while (i != double_length and !vert[1][i]) {
            i++;
        }
        if (i == double_length)break;
        j = i + 1;

        while (j != double_length and vert[1][j]) {
            j++;
        }
        k = i - 1;
        while (k != double_length and !vert[0][k]) {
            k++;
        }
        if (k == double_length)break;
        l = k + 1;
        while (l != double_length and vert[0][l]) {
            l++;
        }

        if (j - k >= 0 and l - i >= 0) {
            forth_to_third[vert[1][i] - 1] = vert[0][k] - 1;
        }
        i = j;
    }
    std::map<int, int> third_to_first;

    i = 1;
    j = 0;
    k = 0;
    l = 0;

    while (i < length) {
        while (i != length and !horiz[1][i]) {
            i++;
        }
        if (i == length)break;
        j = i + 1;

        while (j != length and horiz[1][j]) {
            j++;
        }
        k = i - 1;
        while (k != length and !horiz[0][k]) {
            k++;
        }
        if (k == length)break;
        l = k + 1;
        while (l != length and horiz[0][l]) {
            l++;
        }

        if (j - k >= 0 and l - i >= 0) {
            third_to_first[horiz[1][i] - 1] = horiz[0][k] - 1;
        }
        i = j;
    }
    std::map<int, std::pair<int, int>> merge_map_1;
    std::map<int, std::pair<int, int>> merge_map_2;
    std::map<int, std::pair<int, int>> merge_map_3;
    std::map<int, std::pair<int, int>> merge_map_4;

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
    for (auto const &x:second_to_forth) {
        auto it = merge_map_2.find(x.first);
        if (it != merge_map_2.end()) {
            merge_map_4[x.second] = std::pair<int, int>(x.first, merge_map_2[x.first].first);
            merge_map_2[x.first].first = -1;
        } else {
            merge_map_4[x.second] = std::pair<int, int>(x.first, -1);
        }
    }
    for (auto const &x:forth_to_third) {
        auto it = merge_map_4.find(x.first);
        if (it != merge_map_4.end()) {
            merge_map_3[x.second] = std::pair<int, int>(x.first, merge_map_4[x.first].first);
            merge_map_4[x.first].first = -1;
        } else {
            merge_map_3[x.second] = std::pair<int, int>(x.first, -1);
        }
    }
    for (auto const &x:third_to_first) {
        if (third_to_first[x.first] == -1)continue;
        auto it = merge_map_3.find(x.first);
        if (it != merge_map_3.end()) {
            merge_map_1[x.second] = std::pair<int, int>(x.first, merge_map_3[x.first].first);
            merge_map_3[x.first].first = -1;
        } else {
            merge_map_1[x.second] = std::pair<int, int>(x.first, -1);
        }
    }

    //now we reassign regions according to maps above

    for (auto const &x:merge_map_1) {

        if (x.second.first == -1)continue;
        std::vector<std::vector<int>> &curr = regions1[x.first];
        std::vector<std::vector<int>> &prev = regions3[x.second.first];

        curr[0].insert(curr[0].end(), prev[0].begin(), prev[0].end());
        curr[1].insert(curr[1].end(), prev[1].begin(), prev[1].end());

        if (x.second.second == -1)continue;
        std::vector<std::vector<int>> &prev_prev = regions4[x.second.second];

        curr[0].insert(curr[0].end(), prev_prev[0].begin(), prev_prev[0].end());
        curr[1].insert(curr[1].end(), prev_prev[1].begin(), prev_prev[1].end());

    }
    for (auto const &x:merge_map_2) {
        if (x.second.first == -1)continue;
        std::vector<std::vector<int>> &curr = regions2[x.first];
        std::vector<std::vector<int>> &prev = regions1[x.second.first];

        curr[0].insert(curr[0].end(), prev[0].begin(), prev[0].end());
        curr[1].insert(curr[1].end(), prev[1].begin(), prev[1].end());

        if (x.second.second == -1)continue;
        std::vector<std::vector<int>> &prev_prev = regions3[x.second.second];

        curr[0].insert(curr[0].end(), prev_prev[0].begin(), prev_prev[0].end());
        curr[1].insert(curr[1].end(), prev_prev[1].begin(), prev_prev[1].end());

    }

    for (auto const &x:merge_map_4) {
        if (x.second.first == -1)continue;
        std::vector<std::vector<int>> &curr = regions4[x.first];
        std::vector<std::vector<int>> &prev = regions2[x.second.first];

        curr[0].insert(curr[0].end(), prev[0].begin(), prev[0].end());
        curr[1].insert(curr[1].end(), prev[1].begin(), prev[1].end());

        if (x.second.second == -1)continue;
        std::vector<std::vector<int>> &prev_prev = regions1[x.second.second];

        curr[0].insert(curr[0].end(), prev_prev[0].begin(), prev_prev[0].end());
        curr[1].insert(curr[1].end(), prev_prev[1].begin(), prev_prev[1].end());

    }
    for (auto const &x:merge_map_3) {
        if (x.second.first == -1)continue;
        std::vector<std::vector<int>> &curr = regions3[x.first];
        std::vector<std::vector<int>> &prev = regions4[x.second.first];

        curr[0].insert(curr[0].end(), prev[0].begin(), prev[0].end());
        curr[1].insert(curr[1].end(), prev[1].begin(), prev[1].end());

        if (x.second.second == -1)continue;
        std::vector<std::vector<int>> &prev_prev = regions2[x.second.second];

        curr[0].insert(curr[0].end(), prev_prev[0].begin(), prev_prev[0].end());
        curr[1].insert(curr[1].end(), prev_prev[1].begin(), prev_prev[1].end());

    }

    if (first_to_second.empty()) {
        regions.insert(regions.end(), regions1.begin(), regions1.end());
    } else {
        int size = first_to_second.size();
        int prev = 0;
        auto it = first_to_second.begin();
        for (int m = 0; m < size; ++m) {
            regions.insert(regions.end(), regions1.begin() + prev, regions1.begin() + it->first);
            prev = it->first + 1;
            it++;
        }
        regions.insert(regions.end(), regions1.begin() + prev, regions1.end());
    }

    if (second_to_forth.empty()) {
        regions.insert(regions.end(), regions2.begin(), regions2.end());
    } else {
        int size = second_to_forth.size();
        int prev = 0;
        auto it = second_to_forth.begin();
        for (int m = 0; m < size; ++m) {
            regions.insert(regions.end(), regions2.begin() + prev, regions2.begin() + it->first);
            prev =it->first + 1;
            it++;
        }
        regions.insert(regions.end(), regions2.begin() + prev, regions2.end());
    }

    if (forth_to_third.empty()) {
        regions.insert(regions.end(), regions4.begin(), regions4.end());
    } else {
        int size = forth_to_third.size();
        int prev = 0;
        auto it = forth_to_third.begin();
        for (int m = 0; m < size; ++m) {
            regions.insert(regions.end(), regions4.begin() + prev, regions4.begin() + it->first);
            prev = it->first + 1;
            it++;
        }
        regions.insert(regions.end(), regions4.begin() + prev, regions4.end());
    }

    if (third_to_first.empty()) {
        regions.insert(regions.end(), regions3.begin(), regions3.end());
    } else {
        int size = third_to_first.size();
        int prev = 0;
        auto it = third_to_first.begin();
        for (int m = 0; m < size; ++m) {
            regions.insert(regions.end(), regions3.begin() + prev, regions3.begin() + it->first);
            prev = it->first + 1;
            it++;
        }
        regions.insert(regions.end(), regions3.begin() + prev, regions3.end());
    }
//    regions.insert(regions.end(), regions1.begin(), regions1.end());
//    regions.insert(regions.end(), regions2.begin(), regions2.end());
//    regions.insert(regions.end(), regions3.begin(), regions3.end());
//    regions.insert(regions.end(), regions4.begin(), regions4.end());
    return regions;
}

void region_growing::get_regions_one_thread(const cv::Mat &cls_map, const cv::Mat &angle_map, const cv::Mat &cls_bin,
                                            std::vector<std::vector<std::vector<int>>> &regions,
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
        std::vector<std::vector<int>> region = region_grouping(root, cls_map, angle_map, cls_bin, U, top_corner_x,
                                                               top_corner_y, bot_corner_x, bot_corner_y);

        if (region[0].size() > min_region_size) {
            regions.push_back(region);
        } else i--;
    }

}

std::vector<std::vector<int>>
region_growing::region_grouping(int root[2], const cv::Mat &cls_map, const cv::Mat &angle_map,
                                const cv::Mat &cls_bin, std::vector<std::vector<bool>> &U, int top_corner_x,
                                int top_corner_y, int bot_corner_x, int bot_corner_y) const {
    std::vector<std::vector<int>> region(2);

    float region_mean = cls_map.at<float>(root[0], root[1]);
//    float Theta_region = angle_map.at<float>(root[0], root[1]) * M_PI;
//    float Vx = cos(Theta_region);
//    float Vy = sin(Theta_region);

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
//                Vx += cos(ang);
//                Vy += sin(ang);
//                Theta_region = acos(Vx / sqrt(Vx * Vx + Vy * Vy));
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
//    if (region[0].size() > min_region_size) {
//        int b = 9;
//    }
    return region;
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