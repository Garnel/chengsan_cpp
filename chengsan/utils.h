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

using namespace std;

struct Utils {
    template<typename T>
    static T random(const T low, const T high) {
        random_device r;
        default_random_engine e1(r());
        uniform_int_distribution<T> uniformDist(low, high);
        return uniformDist(e1);
    }
    
    template<typename T>
    static T random(const vector<T>& vec) {
        return vec[random<size_t>(0, vec.size() - 1)];
    }
};

#endif /* utils_h */
