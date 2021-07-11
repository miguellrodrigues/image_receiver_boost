#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "../include/tcp_server.hpp"
#include "../include/base64.hpp"
#include <string>

using namespace cv;

void read(const char *data, boost::system::error_code error_code, std::size_t) {
    if (!error_code) {
        try {
            auto payload = base64_decode(data);

            std::vector<unsigned char> decoded_data(payload.begin(), payload.end());

            //std::cout << decoded_data.size() << '\n';

            //cv::Mat data_mat(decoded_data, true);

            cv::Mat image(cv::imdecode(decoded_data, 1));

            cv::imshow("receiving", image);
            cv::waitKey(1);

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

        server tcp_server(io_context, 25565, &read, &write);

        io_context.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
