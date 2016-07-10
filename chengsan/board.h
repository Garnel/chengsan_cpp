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
    static const unordered_map<size_t, vector<size_t>> nexts;
    static const array<array<size_t, 3>, 16> sans;

    static const unordered_map<size_t, array<array<size_t, 3>, 2>> luPosSan;
    
    static const int EMPTY_TAG = 0,     // Mark the empty positions
                     FIRST_TAG = 1,     // Mark the 1st player's pieces' positions
                     SECOND_TAG = 2,    // Mark the 2nd player's pieces' positions
                     BLOCK_TAG = 3;     // Mark the blocked positions

    Board()
    : board({EMPTY_TAG}), emptyPos({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23}) {
    }

    const unordered_set<size_t>& FindEmpty() const {
        return emptyPos;
    }

    const unordered_set<size_t>& FindFirst() const {
        return firstPos;
    }

    const unordered_set<size_t>& FindSecond() const {
        return secondPos;
    }
    
    const unordered_set<size_t>& FindPlayer(int playerTag) const {
        return playerTag == 1 ? FindFirst() : FindSecond();
    }
    
    int PieceNumber(int playerTag) {
        switch (playerTag) {
            case FIRST_TAG:
                return FirstPieceNumber();
                break;
            case SECOND_TAG:
                return SecondPieceNumber();
            default:
                return 0;
        }
    }
    
    int FirstPieceNumber() const {
        return (int)firstPos.size();
    }
    
    int SecondPieceNumber() const {
        return (int)secondPos.size();
    }

    vector<Step> FindMoveSteps(int playerTag) const {
        vector<Step> res;
        for (size_t from: FindPlayer(playerTag)) {
            for (auto to: nexts.at(from)) {
                if (!EmptyAt(to)) {
                    continue;
                }
                res.push_back(Step::MakeMoveStep(playerTag, from, to));
            }
        }
        return res;
    }
    
    bool HasMovableSteps(int playerTag) const {
        for (size_t from: FindPlayer(playerTag)) {
            for (auto to: nexts.at(from)) {
                if (EmptyAt(to)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    int At(size_t pos) const {
        return board[pos];
    }
    
    bool EmptyAt(size_t pos) const {
        return board[pos] == EMPTY_TAG;
    }
    
    void SetEmpty(size_t pos) {
        if (FirstAt(pos)) {
            firstPos.erase(pos);
        } else if (SecondAt(pos)) {
            secondPos.erase(pos);
        } else if (BlockedAt(pos)) {
            blockPos.erase(pos);
        }
        board[pos] = EMPTY_TAG;
        emptyPos.insert(pos);
    }

    bool FirstAt(size_t pos) const {
        return board[pos] == FIRST_TAG;
    }
    
    void SetFirst(size_t pos) {
        board[pos] = FIRST_TAG;
        firstPos.insert(pos);
        emptyPos.erase(pos);
    }

    bool SecondAt(size_t pos) const {
        return board[pos] == SECOND_TAG;
    }
    
    void SetSecond(size_t pos) {
        board[pos] = SECOND_TAG;
        secondPos.insert(pos);
        emptyPos.erase(pos);
    }

    bool BlockedAt(size_t pos) const {
        return board[pos] == BLOCK_TAG;
    }
    
    void SetBlock(size_t pos) {
        if (FirstAt(pos)) {
            firstPos.erase(pos);
        } else if (SecondAt(pos)) {
            secondPos.erase(pos);
        }
        board[pos] = BLOCK_TAG;
        blockPos.insert(pos);
    }
    
    void ClearBlock(size_t pos) {
        SetEmpty(pos);
    }
    
    void ClearBlocks() {
        for (size_t i = 0; i < 24; ++i) {
            if (board[i] == BLOCK_TAG) {
                ClearBlock(i);
            }
        }
    }
    
    void Move(size_t from, size_t to) {
        if (EmptyAt(from) || !EmptyAt(to)) {
            return;
        }
        
        if (At(from) == FIRST_TAG) {
            SetFirst(to);
        } else if (At(from) == SECOND_TAG) {
            SetSecond(to);
        }
        SetEmpty(from);
    }

    unordered_set<size_t> FindFreePiece(int playerTag) const {
        unordered_set<size_t> res;
        auto positions = FindPlayer(playerTag);
        for_each(positions.begin(), positions.end(), [&](size_t pos) {
            if (IsFreePiece(playerTag, pos)) {
                res.insert(pos);
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

    unordered_set<size_t> FindToEat(int player) const {
        auto res = FindFreePiece(player);
        if (res.empty()) {
            return FindPlayer(player);
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

    void ApplyStep(const Step& step, int round) {
        if (round == 9) {
            ClearBlocks();
        }
        
        switch (step.type) {
            case Step::PLACE:
                if (step.playerTag == FIRST_TAG) {
                    SetFirst(step.placePos);
                } else if (step.playerTag == SECOND_TAG) {
                    SetSecond(step.placePos);
                }
                
                if (step.eatPos != Step::NONE_POS) {
                    SetBlock(step.eatPos);
                }
                break;
            case Step::MOVE:
                Move(step.moveFrom, step.moveTo);
                if (step.eatPos != Step::NONE_POS) {
                    SetEmpty(step.eatPos);
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
        
        if ((round > 9 && FindFirst().size() <= 2) || !HasMovableSteps(FIRST_TAG)) {
            winner = SECOND_TAG;
            return true;
        }

        if ((round > 9 && FindSecond().size() <= 2) || !HasMovableSteps(SECOND_TAG)) {
            winner = FIRST_TAG;
            return true;
        }

        return false;
    }
private:
    array<int, 24> board;
    unordered_set<size_t> emptyPos, firstPos, secondPos, blockPos;
};

const unordered_map<size_t, vector<size_t>> Board::nexts = {
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

const array<array<size_t, 3>, 16> Board::sans = {{
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

const unordered_map<size_t, array<array<size_t, 3>, 2>> Board::luPosSan = {
    {0,  {{0,1,2,      6,7,0}}},
    {1,  {{0,1,2,      1,9,17}}},
    {2,  {{0,1,2,      2,3,4}}},
    {3,  {{2,3,4,      3,11,19}}},
    {4,  {{2,3,4,      4,5,6}}},
    {5,  {{4,5,6,      5,13,21}}},
    {6,  {{4,5,6,      6,7,0}}},
    {7,  {{6,7,0,      7,15,23}}},
    {8,  {{8,9,10,     14,15,8}}},
    {9,  {{8,9,10,     1,9,17}}},
    {10, {{8,9,10,     10,11,12}}},
    {11, {{10,11,12,   3,11,19}}},
    {12, {{10,11,12,   12,13,14}}},
    {13, {{12,13,14,   5,13,21}}},
    {14, {{12,13,14,   14,15,8}}},
    {15, {{14,15,8,    7,15,23}}},
    {16, {{16,17,18,   22,23,16}}},
    {17, {{16,17,18,   1,9,17}}},
    {18, {{16,17,18,   18,19,20}}},
    {19, {{18,19,20,   3,11,19}}},
    {20, {{18,19,20,   20,21,22}}},
    {21, {{20,21,22,   5,13,21}}},
    {22, {{20,21,22,   22,23,16}}},
    {23, {{22,23,16,   7,15,23}}}
};

#endif /* board_h */
