//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H

#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>

class Texture {
private:
    cv::Mat image_data;

public:
    Texture(const std::string &name) {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v) {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        //std::cout << u_img << " " << v_img << std::endl;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u, float v) {
        auto u0 = std::ceil(u * width);
        auto u1 = std::floor(u * width);
        auto v0 = std::ceil((1 - v) * height);
        auto v1 = std::floor((1 - v) * height);

        auto color0 = image_data.at<cv::Vec3b>(v0, u0);
        auto color1 = image_data.at<cv::Vec3b>(v0, u1);
        auto color2 = image_data.at<cv::Vec3b>(v1, u0);
        auto color3 = image_data.at<cv::Vec3b>(v1, u1);

        auto lerp_color0 = color0 + (color1 - color0) * (u * width - u0);
        auto lerp_color1 = color2 + (color3 - color2) * (u * width - u0);

        auto lerp_color = lerp_color0 + (lerp_color1 - lerp_color0) * ((1 - v) * height - v0);
        return Eigen::Vector3f(lerp_color[0], lerp_color[1], lerp_color[2]);
    }

};

#endif //RASTERIZER_TEXTURE_H
