#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

bool isTriangle(std::vector<cv::Point> approx, std::string& name){
    if (approx.size() == 3){
        name = "triangle";
        return true;
    }
    else{
        return false;
    }   
}

bool isCircle(double circularity, std::string& name){
    if (circularity > 0.85){
        name = "circle";
        return true;
    }
    else{
        return false;
    }
}

bool isAngle90(std::vector<cv::Point> approx){
    bool result;
    if (approx.size() == 4){
        for (int i = 0; i < approx.size(); i++){
            cv::Point current = approx[i];
            cv::Point previous = approx[(i + 3) % 4];
            cv::Point next = approx[(i+1)%4];

            cv::Point vect1 = previous - current;
            cv::Point vect2 = next - current;
            
            double length1 = cv::norm(vect1);
            double length2 = cv::norm(vect2);

            if (length1 == 0 || length2 == 0){
                result = false;
            }
            else{
                int scal1 = vect1.x*vect2.x + vect1.y*vect2.y;
                double cos = scal1/(length1 * length2);
                if (std::abs(cos) > 0.1){
                    result = false;
                }
                else{
                    result = true;
                }
            }
        }
    }
    else{
        result = false;
    }
    return result;
};

bool isSquare(bool angle90, double circularity, std::vector<cv::Point>approx, std::string& name, float longSide, float shortSide){
    if ( angle90 == true && circularity < 0.85 && approx.size() == 4){
        if (shortSide/longSide >= 0.90){
            name = "square";
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

bool isRect(bool angle90, double circularity, std::vector<cv::Point>approx, std::string& name, float longSide, float shortSide){
    if ( angle90 == true && circularity < 0.85 && approx.size() == 4){
        if (shortSide/longSide < 0.90){
            name = "rectangle";
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}

int main(){
    cv::Mat image = cv::imread("images/shapes_test_02.png");

    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::imwrite("results/gray.png", gray);

    cv::Mat mask;
    cv::threshold(gray, mask, 0, 255, cv::THRESH_BINARY_INV);
    cv::imwrite("results/mask.png", mask);

    std::vector<std::vector<cv::Point>> contours;

    cv::findContours(
        mask,
        contours,
        cv::RETR_EXTERNAL,
        cv::CHAIN_APPROX_SIMPLE
    );

    cv::Mat result = image.clone();

    std::cout << contours.size() << "\n";

    std::string name = "unknown";

    for (int i = 0; i < contours.size(); i++){
        double s = cv::contourArea(contours[i]);
        double perimetr = cv::arcLength(contours[i], true);
        double circularity = (4*s*CV_PI)/(perimetr*perimetr);
        cv::Rect box = cv::boundingRect(contours[i]);
        cv::RotatedRect box2 = cv::minAreaRect(contours[i]);
        float width = box2.size.width;
        float height = box2.size.height;

        float longSide = cv::max(width, height);
        float shortSide = cv::min(width, height);

        int x = box.x;
        int y = box.y;

        std::vector<cv::Point> approx;
        cv::approxPolyDP(
            contours[i],
            approx,
            0.02*perimetr,
            true
        );

        bool angle90 = isAngle90(approx);
        if (isTriangle(approx, name) == true){
           std::cout << name << "\n";
        }
        else if (isRect(angle90, circularity, approx, name, longSide, shortSide)){
            std::cout << name << "\n";
        }
        else if (isSquare(angle90, circularity, approx, name, longSide, shortSide)){
            std::cout << name << "\n";
        }
        else if (isCircle(circularity, name) == true){
            std::cout << name << "\n";
        }
        else{
            name = "unknown";
            std::cout << name << "\n";
        }

        cv::putText(
            result,
            name,
            cv::Point(x,y),
            cv::FONT_HERSHEY_SIMPLEX,
            1.7,
            cv::Scalar(0, 255, 0),
            2
        );
    }

    cv::imwrite("results/result.png", result);
} 