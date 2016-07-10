//
//  player.h
//  chengsan
//
//  Created by Garnel Mao on 7/6/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef player_h
#define player_h

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "utils.h"
#include "step.h"

class Player {
public:
    Player(int selfTag, int oTag)
    :tag(selfTag), opponentTag(oTag), ai(false) {
        srand((unsigned)time(NULL));
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
            auto pos = Utils::random(positions);
            if (board.CanMakeSan(tag, pos)) {
                // random eat a piece
                auto eatPositions = board.FindToEat(opponentTag);
                return Step::MakePlaceStep(tag, pos, Utils::random(eatPositions));
            } else {
                return Step::MakePlaceStep(tag, pos);
            }
        } else {
            // move
            auto steps = board.FindMoveSteps(tag);
            auto chosen = Utils::random(steps);
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
            
            Board backupBoard = fakeBoard;
            int backupRound = fakeRound;
            for (int k = 0; k < 100; ++k) {
                bool over = false;
                for (int j = 0; j < 100; ++j) {
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
                    if (fakeBoard.PieceNumber(tag) > fakeBoard.PieceNumber(opponentTag)) {
                        score[i]++;
                    }
                }
                fakeBoard = backupBoard;
                fakeRound = backupRound;
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
    
    Step Human(const Board& board, int round) const {
        if (round < 9) {
            int circle, pos, boardPos;
            while (true) {
                cout << "Please input circle, pos: ";
                cin >> circle >> pos;
                boardPos = circle * 8 + pos;
                if (board.EmptyAt(boardPos)) {
                    break;
                }
            }

            auto step = Step::MakePlaceStep(tag, boardPos);
            if (board.CanMakeSan(tag, boardPos)) {
                while (true) {
                    cout << "Please input circle, pos of which piece you want to eat: ";
                    cin >> circle >> pos;
                    boardPos = circle * 8 + pos;
                    if (board.At(boardPos) == opponentTag) {
                        step.eatPos = boardPos;
                        break;
                    }
                }
            }
            return step;
        }
        
        int circle, pos, fromPos, toPos;
        while (true) {
            cout << "Please input circle, pos of which you want to move: ";
            cin >> circle >> pos;
            fromPos = circle * 8 + pos;
            if (board.At(fromPos) == tag) {
                break;
            }
        }
        while (true) {
            cout << "Please input circle, pos of which you want to move to: ";
            cin >> circle >> pos;
            toPos = circle * 8 + pos;
            if (board.EmptyAt(toPos)) {
                break;
            }
        }
        auto step = Step::MakeMoveStep(tag, fromPos, toPos);
        if (board.CanMakeSan(tag, toPos, fromPos)) {
            while (true) {
                cout << "Please input circle, pos of which piece you want to eat: ";
                cin >> circle >> pos;
                int boardPos = circle * 8 + pos;
                if (board.At(boardPos) == opponentTag) {
                    step.eatPos = boardPos;
                    break;
                }
            }
        }
        return step;
    }
    
private:
    bool ai;
    int tag;
    int opponentTag;
};

#endif /* player_h */
