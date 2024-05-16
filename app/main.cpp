#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "../include/tcp_server.hpp"
#include "../include/base64.hpp"
#include <string>
#include <ctime>

using namespace cv;

double fps = 0;
int frameCount = 0;
auto start = std::chrono::steady_clock::now();

void read(const char *data, boost::system::error_code error_code, std::size_t) {
    if (!error_code) {
        try {
            auto payload = base64_decode(data);

            std::vector<unsigned char> decoded_data(payload.begin(), payload.end());

            //std::cout << decoded_data.size() << '\n';

            cv::Mat data_mat(decoded_data, true);

            cv::Mat image(cv::imdecode(decoded_data, 1));

            putText(image, "FPS: " + std::to_string(fps), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1,
                    cv::Scalar(0, 0, 255), 2, cv::LINE_AA, false);

            cv::imshow("receiving", image);
            cv::waitKey(1);

            // Calculate FPS
            auto end = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() /
                           1000.0; // Convert to seconds
            if (elapsed >= 1.0) {
                fps = frameCount / elapsed;
                frameCount = 0;
                start = end;
            }

            frameCount++;

            payload.clear();
            decoded_data.clear();
            image.release();
        } catch (std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    } else {
        std::cout << "Error " << error_code << '\n';
    }
}

void write(const char *, boost::system::error_code, std::size_t) {
}

int main() {
    try {
        boost::asio::io_context io_context;

        tcp_server tcp_server(io_context, 8756, &read, &write);
        std::cout << "Server running\n";

        io_context.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
