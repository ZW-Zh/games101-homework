#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) {
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 5) {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
                  << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) {
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                     3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) {
    // TODO: Implement de Casteljau's algorithm
    cv::Point2f point = control_points[0] * (1 - t) + t * control_points[1];
    return point;
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) {
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    std::vector<cv::Point2f> results;
    size_t index = 0;
    for (double t = 0.0; t <= 1.0; t += 0.001) {
        //line
        for (size_t i = 0; i != control_points.size(); ++i) {
            //column
            for (auto j = 0; j != control_points.size() - i - 1; ++j) {
                std::vector<cv::Point2f> points;
                if (i == 0) {
                    //i=0,j:0~2
                    points.push_back(control_points.at(j));
                    points.push_back(control_points.at(j + 1));
                } else {
                    //i=1,j:0~1
                    //i=2,j:0
                    points.push_back(results.at(index));
                    points.push_back(results.at(index + 1));
                    index++;
                }
                results.push_back(recursive_bezier(points, t));
                points.clear();
            }
            if (i != 0) index++;
        }

        //antialiasing
        cv::Point2f point = {(*(results.end() - 1)).y, (*(results.end() - 1)).x};
//        int min_x = std::floor(point.x);
//        int max_x = std::ceil(point.x);
//        int min_y = std::floor(point.y);
//        int max_y = std::ceil(point.y);
        //std::cout<<point.x<<" "<<point.y;
        int min_x = std::max(0, (int) floor(point.x));
        int max_x = std::min(window.cols - 1, (int) ceil(point.x));
        int min_y = std::max(0, (int) floor(point.y));
        int max_y = std::min(window.rows - 1, (int) ceil(point.y));

        window.at<cv::Vec3b>(min_x, min_y)[1] = 255 * sqrt(pow(point.x - min_x, 2) + pow(point.y - min_y, 2)) / sqrt(2);
        window.at<cv::Vec3b>(min_x, max_y)[1] = 255 * sqrt(pow(point.x - max_x, 2) + pow(point.y - min_y, 2)) / sqrt(2);
        window.at<cv::Vec3b>(max_x, min_y)[1] = 255 * sqrt(pow(point.x - min_x, 2) + pow(point.y - max_y, 2)) / sqrt(2);
        window.at<cv::Vec3b>(max_x, max_y)[1] = 255 * sqrt(pow(point.x - max_x, 2) + pow(point.y - max_y, 2)) / sqrt(2);

        window.at<cv::Vec3b>((*(results.end() - 1)).y, (*(results.end() - 1)).x)[1] = 255;
    }
}

int main() {
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) {
        for (auto &point : control_points) {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 5) {
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

    return 0;
}
