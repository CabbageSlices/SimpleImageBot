// #pragma once
// #include "ImageProcess.h"
// #include "includes.h"

// class ImageProcess;
// /**
//  * @brief Keeps track of a bunch of processes that can be run on an image. At a fixed internal it takes a screenshot of
//  * desktop and runs processes on it. Screenshots are taken whenever a process is ready to fire. If multiple processes are
//  * ready to fire in the same interval then they will use the same image
//  */
// typedef unsigned long CycleCount;
// class ImageProcessor {
//   public:
//     ImageProcessor(const sf::Time& _timeBetweenCycles)
//         : timeBetweenCycles(_timeBetweenCycles),
//           timeThisCycleBegan(sf::milliseconds(0)),
//           currentCycle(0){};
//     void update(const sf::Time& currentTime);

//     void every(unsigned numCycles, const string& processName, function<void(const cv::Mat&)> imageProcessingFunction);
//     void stop(const string& processName);

//     void addProcess(const string& processName, CycleCount cyclesBetweenExecution, shared_ptr<ImageProcess> process);

//   private:
//     bool updateCycleCount(const sf::Time& currentTime);
//     void executeReadyProcessess();

//     sf::Time timeBetweenCycles;
//     sf::Time timeThisCycleBegan;
//     unsigned long currentCycle;

//     map<string, shared_ptr<ImageProcess>> processes;
// };