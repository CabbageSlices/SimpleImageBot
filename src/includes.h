#pragma once
#include <windows.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// opencv
#include <opencv2/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>

// must come after windows.h
#include <gdiplus.h>

using std::cin;
using std::cout;
using std::endl;
using std::function;
using std::map;
using std::shared_ptr;
using std::string;
using std::vector;