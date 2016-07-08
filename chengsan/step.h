//
//  step.h
//  chengsan
//
//  Created by Garnel Mao on 7/7/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef step_h
#define step_h

class Step {
public:
    enum{PLACE, MOVE} type;
    size_t eatPos;
    int playerTag;

    union {
        struct {
            size_t moveFrom, moveTo;
        };
        size_t placePos;
    };

    static Step MakePlaceStep(int player, size_t placePos, size_t eatPos = -1) {
        return Step(player, placePos, eatPos);
    }

    static Step MakeMoveStep(int player, size_t moveFrom, size_t moveTo, size_t eatPos = -1) {
        return Step(player, moveFrom, moveTo, eatPos);
    }

private:
    Step(int player, size_t placePos, size_t eatPos)
    :playerTag(player), placePos(placePos), eatPos(eatPos), type(PLACE) {
    }

    Step(int player, size_t moveFrom, size_t moveTo, size_t eatPos)
    :playerTag(player), moveFrom(moveFrom), moveTo(moveTo), eatPos(eatPos), type(MOVE) {
    }
};

#endif /* step_h */
