//
// Created by leonid on 14.02.2021.
//

#include "compute_eigen_coords.h"

compute_eigen_coords::compute_eigen_coords() = default;

std::vector<std::vector<float>> compute_eigen_coords::apply_trans(const std::vector<std::vector<float>> &transformation,
                                                                  const std::vector<std::vector<int>> &region,
                                                                  const cv::Mat &cls_map) {
    std::vector<std::vector<float>> new_coords(2, std::vector<float>(region[0].size()));
    float cos_alpha = transformation[0][0];
    float sin_alpha = transformation[1][0];
    float S0 = 0;
    float curr_region_mean = 0;
    for (int i = 0; i < region[0].size(); ++i) {
        float x = (region[0][i] - x_mean);
        float y = (region[1][i] - y_mean);
        new_coords[0][i] = cos_alpha * x + sin_alpha * y;
        new_coords[1][i] = -sin_alpha * x + cos_alpha * y;
        float weight = 1;
        if (cls_map.data!=NULL) {
            weight = cls_map.at<float>(region[0][i], region[1][i]);
        }
        curr_region_mean += (new_coords[1][i] * weight);
        S0 += weight;
    }

    region_mean = curr_region_mean / S0;
    confidence = S0/region[0].size();
    return new_coords;
}

std::vector<std::vector<float>> compute_eigen_coords::apply_inverse_trans(
        const std::vector<std::vector<float>> &transformation,
        const std::vector<std::vector<float>> &points_new) const {
    std::vector<std::vector<float>> points(2, std::vector<float>(points_new[0].size()));
    float cos_alpha = transformation[0][0];
    float sin_alpha = transformation[1][0];
    for (int i = 0; i < points_new[0].size(); ++i) {
        float x = points_new[0][i];
        float y = points_new[1][i];
        points[0][i] = cos_alpha * x - sin_alpha * y + x_mean;
        points[1][i] = sin_alpha * x + cos_alpha * y + y_mean;
    }
    return points;
}

std::vector<std::vector<float>> compute_eigen_coords::compute_transformation_matrix(
        const std::vector<std::vector<int>> &region, const cv::Mat &cls_map) {

    std::vector<int> x = region[0];
    std::vector<int> y = region[1];

    float mxx = 0;
    float myy = 0;
    float mxy = 0;

    int n = x.size();
    if (cls_map.data == NULL) {

        x_mean = std::accumulate(x.begin(), x.end(), 0.0) / n;
        y_mean = std::accumulate(y.begin(), y.end(), 0.0) / n;


        for (int i = 0; i < n; ++i) {
            float x_i = x[i] - x_mean;
            float y_i = y[i] - y_mean;
            mxx += (x_i * x_i);
            myy += (y_i * y_i);
            mxy += (x_i * y_i);
        }
        mxx = mxx / n;
        myy = myy / n;
        mxy = mxy / n;

    } else {
        float S = 0;
        float curr_x_sum = 0;
        float curr_y_sum = 0;
        for (int i = 0; i < n; ++i) {
            float weight = cls_map.at<float>(x[i], y[i]);
            curr_x_sum += (x[i] * weight);
            curr_y_sum += (y[i] * weight);
            S += weight;
        }
        x_mean = curr_x_sum / S;
        y_mean = curr_y_sum / S;

        for (int i = 0; i < n; ++i) {
            float weight = cls_map.at<float>(x[i], y[i]);
            float x_i = x[i] - x_mean;
            float y_i = y[i] - y_mean;
            mxx += (x_i * x_i * weight);
            myy += (y_i * y_i * weight);
            mxy += (x_i * y_i * weight);
        }
        mxx = mxx / S;
        myy = myy / S;
        mxy = mxy / S;
    }
    float M[2][2] = {
            {myy, mxy},
            {mxy, mxx}
    };
    float lamb = (M[0][0] + M[1][1] - sqrt(4 * M[0][1] * M[0][1] + (M[0][0] - M[1][1]) * (M[0][0] - M[1][1]))) / 2;
    float direction[2] = {M[0][1], (lamb - M[0][0])};

    float f = (direction[0] >= 0) ? 1 : -1;
    float alpha = acos(f * (direction[1]) / pow(pow(direction[1], 2) + pow(direction[0], 2), 0.5)) - M_PI / 2;

    float sin_alpha = std::sin(alpha);
    float cos_alpha = std::cos(alpha);

    std::vector<std::vector<float>> A = {
            {cos_alpha, -sin_alpha},
            {sin_alpha, cos_alpha}
    };

    return A;
}