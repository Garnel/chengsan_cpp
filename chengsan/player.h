//
//  player.h
//  chengsan
//
//  Created by Garnel Mao on 7/6/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef player_h
#define player_h

#include <random>
#include "utils.h"
#include "step.h"

class Player {
public:
    Player(int selfTag, int oTag)
    :tag(selfTag), opponentTag(oTag), ai(false) {
    }

    bool IsAI() const {
        return ai;
    }

    void SetAI() {
        ai = true;
    }

    int Tag() const {
        return tag;
    }
    
    int OpponentTag() const {
        return opponentTag;
    }

    Step Random(const Board& board, int round) const {
        if (round < 9) {
            // place
            auto positions = board.FindEmpty();
            auto pos = Utils::random<size_t>(positions);
            if (board.CanMakeSan(tag, pos)) {
                // random eat a piece
                auto eatPositions = board.FindToEat(opponentTag);
                return Step::MakePlaceStep(tag, pos, Utils::random<size_t>(eatPositions));
            } else {
                return Step::MakePlaceStep(tag, pos);
            }
        } else {
            // move
            auto steps = board.FindMoveSteps(tag);
            auto chosen = Utils::random<Step>(steps);
            if (board.CanMakeSan(tag, chosen.moveTo, chosen.moveFrom)) {
                // random eat a piece
                auto eatPositions = board.FindToEat(opponentTag);
                chosen.eatPos = Utils::random<size_t>(0, eatPositions.size() - 1);
            }
            return chosen;
        }
    }
    
    Step MonteCarlo(const Board& board, int round, const Player& oppenent, bool isFirst) const {
        vector<Step> possibleSteps;
        if (round < 9) {
            for (auto pos: board.FindEmpty()) {
                if (!board.CanMakeSan(tag, pos)) {
                    possibleSteps.push_back(Step::MakePlaceStep(tag, pos));
                    continue;
                }
                
                for (auto eatPos: board.FindToEat(opponentTag)) {
                    possibleSteps.push_back(Step::MakePlaceStep(tag, pos, eatPos));
                }
            }
        } else {
            for (auto step: board.FindMoveSteps(tag)) {
                if (!board.CanMakeSan(tag, step.moveTo, step.moveFrom)) {
                    possibleSteps.push_back(step);
                    continue;
                }
                
                for (auto eatPos: board.FindToEat(opponentTag)) {
                    Step s = step; s.eatPos = eatPos;
                    possibleSteps.push_back(s);
                }
            }
        }
        
        vector<int> score(possibleSteps.size(), 0);
        int winner;
        for (int i = 0; i < possibleSteps.size(); ++i) {
            Board fakeBoard = board;
            int fakeRound = round;
            fakeBoard.ApplyStep(possibleSteps[i], fakeRound);
            if (fakeBoard.IsOver(winner, fakeRound)) {
                if (winner == tag) {
                    return possibleSteps[i];
                } else {
                    continue;
                }
            }
            if (!isFirst) fakeRound++;
            
            bool over = false;
            for (int j = 0; j < 100000; ++j) {
                fakeBoard.ApplyStep(oppenent.Random(fakeBoard, fakeRound), fakeRound);
                if (fakeBoard.IsOver(winner, fakeRound)) {
                    if (winner == tag) {
                        score[i]++;
                    }
                    over = true;
                    break;
                }
                
                if (isFirst) fakeRound++;
                fakeBoard.ApplyStep(Random(fakeBoard, fakeRound), fakeRound);
                if (fakeBoard.IsOver(winner, fakeRound)) {
                    if (winner == tag) {
                        score[i]++;
                    }
                    over = true;
                    break;
                }
            }
            if (!over) {
                if (fakeBoard.FindPiecesPos(tag).size() > fakeBoard.FindPiecesPos(opponentTag).size()) {
                    score[i]++;
                }
            }
        }
        
        int maxScore = -1, maxPos = -1;
        for (int i = 0; i < score.size(); ++i) {
            if (score[i] > maxScore) {
                maxScore = score[i];
                maxPos = i;
            }
        }
        return possibleSteps[maxPos];
    }
private:
    bool ai;
    int tag;
    int opponentTag;
};

#endif /* player_h */
