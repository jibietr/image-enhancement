#include <opencv2/opencv.hpp>
#include <string>
#include <algorithm>

void removeBordersFromImage(const cv::Mat& src, cv::Mat& dst, bool display_images = false, bool verbose = false)
{

    const int threshVal = 20;
    const float borderThresh = 0.05f; // 5%
    cv::Point tl, br;

    cv::Mat thresholded;
    // Prepare new grayscale image
    cv::Mat input_img_gray;
    // Convert to img to Grayscale
    cv::cvtColor(src, input_img_gray, CV_RGB2GRAY);
    // Threshold uses grayscale image
    cv::threshold(input_img_gray, thresholded, threshVal, 255, cv::THRESH_BINARY);
    cv::morphologyEx(thresholded, thresholded, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)),
                     cv::Point(-1, -1), 2, cv::BORDER_CONSTANT, cv::Scalar(0));

    if (display_images)
    {
        //cv::namedWindow("Original image", cv::WINDOW_AUTOSIZE);
        cv::imshow("Original image", src);
        cv::imshow("Thresholded", thresholded);
    }

    if (verbose)
    {
        std::cout << "Width : " << src.size().width << std::endl;
        std::cout << "Height: " << src.size().height << std::endl;
    }


    // create ROI based on thresholded image
    for (int row = 0; row < thresholded.rows; row++)
    {
        if (cv::countNonZero(thresholded.row(row)) > borderThresh * thresholded.cols)
        {
            tl.y = row;
            break;
        }
    }

    for (int col = 0; col < thresholded.cols; col++)
    {
        if (cv::countNonZero(thresholded.col(col)) > borderThresh * thresholded.rows)
        {
            tl.x = col;
            break;
        }
    }

    for (int row = thresholded.rows - 1; row >= 0; row--)
    {
        if (cv::countNonZero(thresholded.row(row)) > borderThresh * thresholded.cols)
        {
            br.y = row;
            break;
        }
    }

    for (int col = thresholded.cols - 1; col >= 0; col--)
    {
        if (cv::countNonZero(thresholded.col(col)) > borderThresh * thresholded.rows)
        {
            br.x = col;
            break;
        }
    }

    cv::Rect roi(tl, br);
    dst = src(roi);

    //if (display_images)
    //{
    //    cv::imshow("Cropped", cropped);
    //    cv::waitKey(0);
    //}
    //return cropped;
}