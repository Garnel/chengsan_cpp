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
#include <cstdio>

class Game {
public:
    Game()
    :first(Board::FIRST_TAG, Board::SECOND_TAG),
    second(Board::SECOND_TAG, Board::FIRST_TAG), round(0) {
    }

    void Run() {
        int winner;
        while (true) {
            board.ApplyStep(first.MonteCarlo(board, round, second, true), round);
            PrintBoard();
            if (board.IsOver(winner, round)) {
                break;
            }
            
            board.ApplyStep(second.MonteCarlo(board, round, first, false), round);
//            board.ApplyStep(second.Human(board, round), round);
            PrintBoard();
            if (board.IsOver(winner, round)) {
                break;
            }

            round++;
//            cin.get();
        }

        cout << "Winner: Player " << winner << endl;
    }

    void PrintBoard() const {
        array<char, 4> tags({'.', '1', '2', '*'});
        printf("\nRound: %d\n\
%c-----------%c-----------%c\n\
|           |           |\n\
|   %c-------%c-------%c   |\n\
|   |       |       |   |\n\
|   |   %c---%c---%c   |   |\n\
|   |   |       |   |   |\n\
%c---%c---%c       %c---%c---%c\n\
|   |   |       |   |   |\n\
|   |   %c---%c---%c   |   |\n\
|   |       |       |   |\n\
|   %c-------%c-------%c   |\n\
|           |           |\n\
%c-----------%c-----------%c\n\
\n",
                round,
                tags[board.At(0)],
                tags[board.At(1)],
                tags[board.At(2)],
                tags[board.At(8)],
                tags[board.At(9)],
                tags[board.At(10)],
                tags[board.At(16)],
                tags[board.At(17)],
                tags[board.At(18)],
                tags[board.At(7)],
                tags[board.At(15)],
                tags[board.At(23)],
                tags[board.At(19)],
                tags[board.At(11)],
                tags[board.At(3)],
                tags[board.At(22)],
                tags[board.At(21)],
                tags[board.At(20)],
                tags[board.At(14)],
                tags[board.At(13)],
                tags[board.At(12)],
                tags[board.At(6)],
                tags[board.At(5)],
                tags[board.At(4)]);
        printf("1st: %ld on board | 2nd: %ld on board\n", board.FindFirst().size(), board.FindSecond().size());
    }
private:
    Board board;
    int round;
    Player first, second;
};

#endif /* game_h */
