//
//  board.h
//  chengsan
//
//  Created by Garnel Mao on 7/6/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef board_h
#define board_h

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "step.h"

using namespace std;

/*
 The game board
 
 0------------1-----------2
 |            |           |
 |   8--------9------10   |
 |   |        |       |   |
 |   |   16--17--18   |   |
 |   |   |        |   |   |
 7---15--23      19--11---3
 |   |   |        |   |   |
 |   |   22--21--20   |   |
 |   |        |       |   |
 |   14------13------12   |
 |            |           |
 6------------5-----------4
 */
class Board {
public:
    const unordered_map<size_t, vector<size_t>> nexts = {
        {0, {1, 7}},
        {1, {0, 2, 9}},
        {2, {1, 3}},
        {3, {2, 4, 11}},
        {4, {3, 5}},
        {5, {4, 6, 13}},
        {6, {5, 7}},
        {7, {0, 6, 15}},
        {8, {9, 15}},
        {9, {1, 8, 10, 17}},
        {10, {9, 11}},
        {11, {3, 10, 12, 19}},
        {12, {11, 13}},
        {13, {5, 12, 14, 21}},
        {14, {13, 15}},
        {15, {7, 8, 14, 23}},
        {16, {17, 23}},
        {17, {9, 16, 18}},
        {18, {17, 19}},
        {19, {11, 18, 20}},
        {20, {19, 21}},
        {21, {13, 20, 22}},
        {22, {21, 23}},
        {23, {15, 16, 22}},
    };

    const array<array<size_t, 3>, 16> sans = {{
        {0, 1, 2},
        {2, 3, 4},
        {4, 5, 6},
        {6, 7, 0},
        {8, 9, 10},
        {10, 11, 12},
        {12, 13, 14},
        {14, 15, 8},
        {16, 17, 18},
        {18, 19, 20},
        {20, 21, 22},
        {22, 23, 16},
        {1, 9, 17},
        {3, 11, 19},
        {5, 13, 21},
        {7, 15, 23}
    }};

    unordered_map<size_t, vector<array<size_t, 3>>> luPosSan;
    
    static const int EMPTY_TAG = 0,     // Mark the empty positions
                     FIRST_TAG = 1,     // Mark the 1st player's pieces' positions
                     SECOND_TAG = 2,    // Mark the 2nd player's pieces' positions
                     BLOCK_TAG = 3;     // Mark the blocked positions

    Board(): board({EMPTY_TAG}){
        unordered_map<size_t, unordered_set<size_t>> reverseSanPos;
        for (size_t i = 0; i < sans.size(); ++i) {
            reverseSanPos[sans[i][0]].insert(i);
            reverseSanPos[sans[i][1]].insert(i);
            reverseSanPos[sans[i][2]].insert(i);
        }

        for (auto pair: reverseSanPos) {
            for (auto pos: pair.second) {
                luPosSan[pair.first].push_back(sans[pos]);
            }
        }
    }

    int At(size_t pos) const {
        return board[pos];
    }

    vector<size_t> FindEmpty() const {
        return FindPos(EMPTY_TAG);
    }

    vector<size_t> FindFirst() const {
        return FindPos(FIRST_TAG);
    }

    vector<size_t> FindSecond() const {
        return FindPos(SECOND_TAG);
    }

    vector<Step> FindMoveSteps(int playerTag) const {
        vector<Step> res;
        for (size_t from: FindPos(playerTag)) {
            for (auto to: nexts.at(from)) {
                if (!EmptyAt(to)) {
                    continue;
                }
                res.push_back(Step::MakeMoveStep(playerTag, from, to));
            }
        }
        return res;
    }

    bool EmptyAt(size_t pos) const {
        return board[pos] == EMPTY_TAG;
    }

    bool BlockedAt(size_t pos) const {
        return board[pos] == BLOCK_TAG;
    }

    void ClearBlock() {
        for (size_t i = 0; i < 24; ++i) {
            if (board[i] == BLOCK_TAG) {
                board[i] = EMPTY_TAG;
            }
        }
    }

    vector<size_t> FindFreePiece(int playerTag) const {
        vector<size_t> res;
        auto positions = FindPos(playerTag);
        for_each(positions.begin(), positions.end(), [&](size_t pos) {
            if (IsFreePiece(playerTag, pos)) {
                res.push_back(pos);
            }
        });
        return res;
    }

    bool IsFreePiece(int player, size_t pos) const {
        auto& allSan = luPosSan.at(pos);
        return all_of(allSan.begin(), allSan.end(), [&](const array<size_t, 3>& san) {
            return any_of(san.begin(), san.end(), [&](size_t pos) {
                return board[pos] != player;
            });
        });
    }

    vector<size_t> FindToEat(int player) const {
        auto res = FindFreePiece(player);
        if (res.empty()) {
            return FindPos(player);
        }
        return res;
    }

    bool CanMakeSan(int player, size_t pos, size_t from = Step::NONE_POS) const {
        for (auto san: luPosSan.at(pos)) {
            if (all_of(san.begin(), san.end(), [=](size_t curPos) {
                if (from != Step::NONE_POS && curPos == from) {
                    return false;
                }
                return curPos == pos || board[curPos] == player;
            })) {
                return true;
            }
        }

        return false;
    }

    void ApplyStep(const Step& step) {
        switch (step.type) {
            case Step::PLACE:
                board[step.placePos] = step.playerTag;
                if (step.eatPos != Step::NONE_POS) {
                    board[step.eatPos] = BLOCK_TAG;
                }
                break;
            case Step::MOVE:
                board[step.moveTo] = board[step.moveFrom];
                board[step.moveFrom] = EMPTY_TAG;
                if (step.eatPos != Step::NONE_POS) {
                    board[step.eatPos] = EMPTY_TAG;
                }
            default:
                break;
        }
    }

    bool IsOver(int& winner, int round) const {
        // skip the first round
        if (round == 0) {
            return false;
        }
        
        if ((round > 9 && FindFirst().size() <= 2) || FindMoveSteps(FIRST_TAG).empty()) {
            winner = SECOND_TAG;
            return true;
        }

        if ((round > 9 && FindSecond().size() <= 2) || FindMoveSteps(SECOND_TAG).empty()) {
            winner = FIRST_TAG;
            return true;
        }

        return false;
    }
    
    

private:
    vector<size_t> FindPos(int status) const {
        vector<size_t> pos;
        for (size_t i = 0; i < 24; ++i) {
            if (board[i] == status) {
                pos.push_back(i);
            }
        }
        return pos;
    }

    array<int, 24> board;
};

#endif /* board_h */
