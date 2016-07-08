//
//  game.h
//  chengsan
//
//  Created by Garnel Mao on 7/6/16.
//  Copyright © 2016 maogm12@gmail.com. All rights reserved.
//

#ifndef game_h
#define game_h

#include "board.h"
#include "player.h"
#include <cstdlib>
#include <cstdio>
#include <string>

class Game {
public:
    Game()
    :first(true), second(false), round(0) {
    }

    void Run() {
        int winner;
        string test;
        while (true) {
            board.ApplyStep(first.Random(board, round));
            PrintBoard();
            if (board.IsOver(winner, round)) {
                break;
            }

            board.ApplyStep(second.Random(board, round));
            PrintBoard();
            if (board.IsOver(winner, round)) {
                break;
            }

            round++;
            cin >> test;
        }

        cout << "Winner: Player " << winner << endl;
    }

    void PrintBoard() const {
        printf("\n\
%d-----------%d-----------%d\n\
|           |           |\n\
|   %d-------%d-------%d   |\n\
|   |       |       |   |\n\
|   |   %d---%d---%d   |   |\n\
|   |   |       |   |   |\n\
%d---%d---%d       %d---%d---%d\n\
|   |   |       |   |   |\n\
|   |   %d---%d---%d   |   |\n\
|   |       |       |   |\n\
|   %d-------%d-------%d   |\n\
|           |           |\n\
%d-----------%d-----------%d\n\
\n",
                board.At(0),
                board.At(1),
                board.At(2),
                board.At(8),
                board.At(9),
                board.At(10),
                board.At(16),
                board.At(17),
                board.At(18),
                board.At(7),
                board.At(15),
                board.At(23),
                board.At(19),
                board.At(11),
                board.At(3),
                board.At(22),
                board.At(21),
                board.At(20),
                board.At(14),
                board.At(13),
                board.At(12),
                board.At(6),
                board.At(5),
                board.At(4));
    }
private:
    Board board;
    int round;
    Player first, second;
};

#endif /* game_h */
