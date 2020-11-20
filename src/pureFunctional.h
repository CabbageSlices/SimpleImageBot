#pragma once
#include <functional>

using std::function;

/**
 * @brief Returns a function F that when called will check if checkCondition() is true, if so, it will exceute
 * onConditionTrue
 *
 * @tparam T
 * @param checkCondition funciton that is called to determine if the onConditionTrue funciton shoudl execute. should
 * return true or false
 * @param onConditionTrue
 * @return function<T>
 */
template <typename T, typename Callable>
inline auto _when(function<bool(const T &)> checkCondition, Callable onConditionTrue) {
    return [&](const T &val) {
        if (checkCondition(val)) {
            onConditionTrue(val);
        }
    };
}

template <typename... T, typename Callable>
inline auto _when(function<bool(const T &...)> checkCondition, Callable onConditionTrue) {
    return [&](const T &... vals) {
        if (checkCondition(vals...)) {
            onConditionTrue(vals...);
        }
    };
}

// template <typename... T, typename Callable>
// inline auto _when(Callable &&checkCondition, function<void()> onConditionTrue) {
//     return [&](const T &... vals) {
//         if (checkCondition(vals...)) {
//             onConditionTrue();
//         }
//     };
// }

template <typename... T>
inline function<void(const T &...)> _and(function<bool(const T &...)> checkCondition1,
                                         function<bool(const T &...)> checkCondition2) {
    return [&](const T &... vals) { return checkCondition1(vals...) && checkCondition2(vals...); };
}

template <typename... T>
inline function<void(const T &...)> _or(function<bool(const T &...)> checkCondition1,
                                        function<bool(const T &...)> checkCondition2) {
    return [&](const T &... vals) { return checkCondition1(vals...) || checkCondition2(vals...); };
}