// #pragma once
// #include "includes.h"

// class ImageProcess {
//   public:
//     ImageProcess(cv::Rect _regionToActOn, std::function<void(const cv::Mat &)> _imageFunction)
//         : regionToActOn(_regionToActOn),
//           imageFunction(_imageFunction){};

//     void operator()(const cv::Mat &image) {
//         if (imageFunction) {
//             imageFunction(image);
//         }
//     }

//     void setRegionToActOn(cv::Rect newRegion) { regionToActOn = newRegion; }
//     void setImageFunction(std::function<void(const cv::Mat &)> newFunction) { imageFunction = newFunction; }

//   private:
//     cv::Rect regionToActOn;
//     std::function<void(const cv::Mat &)> imageFunction;
// };