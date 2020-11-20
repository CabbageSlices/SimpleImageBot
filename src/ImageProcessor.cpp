// #include "ImageProcessor.h"

// #include "screenshot.h"

// void ImageProcessor::update(const sf::Time &currentTime) {
//     if (!updateCycleCount(currentTime)) {
//         // new cycle didn't start, processors only run at start of new cycle
//         return;
//     }

//     executeReadyProcessess();
// }

// void ImageProcessor::every(unsigned numCycles, const string &processName,
//                            function<void(const cv::Mat &)> imageProcessingFunction) {
//     shared_ptr<ImageProcess> process = std::make_shared<ImageProcess>(numCycles, imageProcessingFunction);
//     processes[processName] = process;
// }

// bool ImageProcessor::updateCycleCount(const sf::Time &currentTime) {
//     if (timeThisCycleBegan + timeBetweenCycles >= currentTime) {
//         currentCycle++;
//         timeThisCycleBegan = currentTime;
//         return true;
//     }

//     return false;
// }

// void ImageProcessor::executeReadyProcessess() {
//     cv::Mat latestScreenshot;

//     for (auto &pair : processes) {
//         if (!pair.second->isReadyToExecute(currentCycle)) {
//             continue;
//         }

//         // generate a screenshot if there isn't one already
//         if (latestScreenshot.empty()) {
//             latestScreenshot = takeScreenshot();
//         }

//         // ready to execute, let's do stuff
//         pair.second->execute(currentCycle, latestScreenshot);
//     }
// }