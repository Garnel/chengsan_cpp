//
//  utils.h
//  chengsan
//
//  Created by Garnel Mao on 7/7/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>

using namespace std;

struct Utils {
    template<typename T>
    static T random(const T low, const T high) {
        if (low == high) {
            return low;
        }

        return rand() % (high - low + 1) + low;
    }
    
    template<typename T>
    static T random(const vector<T>& vec) {
        return vec[random<size_t>(0, vec.size() - 1)];
    }
    
    static size_t random(unordered_set<size_t>& s, size_t minBound = 0, size_t maxBound = 23) {
        size_t r = rand() % (maxBound - minBound + 1) + minBound;
        auto res = s.insert(r);
        if (!res.second) {
            return *s.begin();
        }
        auto it = res.first;
        if (it == s.end()) {
            it = s.begin();
        }
        auto val = *it;
        s.erase(r);
        return val;
    }
};

#endif /* utils_h */
