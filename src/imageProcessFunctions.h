// #pragma once
// #include "imageUtility.h"
// #include "includes.h"
// #include "pureFunctional.h"

// template <typename Callable>
// inline auto generatePlayerAvoidanceFunction(cv::Rect mapRegion, const cv::Mat &playerMapIcon,
//                                             Callable onEncounterOtherPlayer) {
//     auto searchForPlayer = function<bool(const cv::Mat &)>(generateImageSearch(playerMapIcon, mapRegion));
//     return _when(searchForPlayer, onEncounterOtherPlayer);
// }