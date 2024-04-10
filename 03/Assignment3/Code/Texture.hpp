//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>

class Texture
{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }


    /**
     * \brief 
     * \param u 坐标x
     * \param v 坐标y
     * \return 双线性插值
     */
    Eigen::Vector3f getColorBilinear(float u, float v)
    {
        float u_img = u * width;
        float v_img = (1 - v) * height;

        // find center coordinates
        int cx = u_img;
        int cy = v_img;
        cx = (u_img - cx) > 0.5 ? std::ceil(u_img) : std::floor(u_img);
        cy = (v_img - cy) > 0.5 ? std::ceil(v_img) : std::floor(v_img);

        // 注意 image_data 第一个坐标对应 v，参考getColor()
        // 并且uv map和image的 v 是相反方向 ！！
        auto u00 = image_data.at<cv::Vec3b>(cy + 0.5, cx - 0.5);
        auto u10 = image_data.at<cv::Vec3b>(cy + 0.5, cx + 0.5);
        auto u01 = image_data.at<cv::Vec3b>(cy - 0.5, cx - 0.5);
        auto u11 = image_data.at<cv::Vec3b>(cy - 0.5, cx + 0.5);

        float s = u * width - (cx - 0.5);
        float t = (1 - v) * height - (cy - 0.5);

        auto u0 = (1 - s) * u00 + s * u10; // at the top
        auto u1 = (1 - s) * u01 + s * u11; // at the bottom 

        auto res = (1 - t) * u1 + t * u0;
        return Eigen::Vector3f(res[0], res[1], res[2]);
    }
};
#endif //RASTERIZER_TEXTURE_H