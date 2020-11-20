#pragma once
#include "includes.h"

// checks if the given image contains the target being searched for
// TODO only check red channel
bool imageContains(const cv::Mat &imageToSearchIn, const cv::Mat &targetToSearchFor) {
    cv::Mat results(imageToSearchIn.rows - targetToSearchFor.rows + 1, imageToSearchIn.cols - targetToSearchFor.cols + 1,
                    CV_32FC1);
    cv::matchTemplate(imageToSearchIn, targetToSearchFor, results, cv::TM_CCOEFF_NORMED);

    double minValue, maxValue;
    cv::Point positionOfMin, positionOfMax;

    cv::minMaxLoc(results, &minValue, &maxValue, &positionOfMin, &positionOfMax);

    cv::Mat output = imageToSearchIn.clone();

    auto &matchValue = maxValue;
    auto &matchPos = positionOfMax;

    if (matchValue > 0.74) {
        cv::rectangle(output, matchPos, matchPos + cv::Point(targetToSearchFor.cols, targetToSearchFor.rows),
                      cv::Scalar(255, 0, 0, 1), 4, cv::LINE_4, 0);
        imshow("match LOC", output);
        cout << "found match at " << matchPos << ";    accuracy: " << matchValue << endl;
        return true;
    }

    return false;
}

auto generateImageSearch(const cv::Mat &targetToSearchFor, const cv::Rect &subRegionToSearch = cv::Rect(0, 0, 0, 0)) {
    return [=](const cv::Mat &imageToSearchIn) {
        if (subRegionToSearch.area() == 0) {
            return imageContains(imageToSearchIn, targetToSearchFor);
        }

        cv::Mat subRegion = imageToSearchIn(subRegionToSearch);
        return imageContains(subRegion, targetToSearchFor);
    };
}