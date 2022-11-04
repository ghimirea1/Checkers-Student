#include "StudentAI.h"
#include <random>
#include <cmath>
#include <string>
using namespace std;
const bool DEBUG = false;
const int DEPTH = 3;
//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
}

Move StudentAI::GetMove(Move move)
{
    if (move.seq.empty())
    {
        player = 1;
    } else{
        board.makeMove(move,player == 1?2:1);
    }

    // minimax move
    Board board_copy = board;
    Move res;
    // higher depth, better ai, longer run
    int max = minimax (board_copy, res, DEPTH, true);
    if (DEBUG) { cout << "minimax: " << max << " res: " << res.toString() << endl; }
    if (DEBUG) { cout << "making move (main): " << res.toString() << endl; }
    board.makeMove (res, player);
    return res;
}

int StudentAI::minimax (Board board, Move& res, int depth, bool max)
{
    string turn = "";
    if (max)
    {
        turn = "B";
    }
    else
    {
        turn = "W";
    }

    if ((depth == 0) || (board.isWin(turn) != 0))
    {
        // black is maximizing
        // white is minimizing
        if (DEBUG) { cout << "returned: " << board.blackCount - board.whiteCount << " at depth: " << depth << endl; }
        return (board.blackCount - board.whiteCount);
    }

    if (max)
    {

        int best_move;
        int best_i;
        int best_j;
        int max_evaluation_found = -99; // inf?
        vector<vector<Move> > moves = board.getAllPossibleMoves (1);
        for (int i = 0; i < moves.size(); i++) 
        {
            for (int j = 0; j < moves[i].size(); j++)
            {
                Board board_copy = board;
                if (DEBUG) { cout << "making move (max): " << moves[i][j].toString() << endl; }
                board_copy.makeMove (moves[i][j], 1);
                int current_evaluation = minimax (board_copy, res, depth - 1, false);
                if (DEBUG) { cout << "made move (max): " << moves[i][j].toString() << endl; cout << "current_eval (max): " << current_evaluation << endl; }
                max_evaluation_found = fmax (current_evaluation, max_evaluation_found);
                if (current_evaluation == max_evaluation_found)
                {
                    best_move = max_evaluation_found;
                    best_i = i;
                    best_j = j;
                    if (DEBUG) { cout << "found best move (max): " << best_move << " | " << res.toString() << " at depth: " << depth << endl; }

                }
            }
        }

        res = moves[best_i][best_j];
        return best_move;
    }
    else
    {
        int best_move;
        int best_i;
        int best_j;
        // int min_evaluation;
        int min_evaluation_found = 99; // inf?
        vector<vector<Move> > moves = board.getAllPossibleMoves (2);
        for (int i = 0; i < moves.size(); i++) 
        {
            for (int j = 0; j < moves[i].size(); j++)
            {
                Board board_copy = board;
                if (DEBUG) { cout << "making move (min): " << moves[i][j].toString() << endl; }
                board_copy.makeMove (moves[i][j], 2);
                int current_evaluation = minimax (board_copy, res, depth - 1, true);
                if (DEBUG) { cout << "made move (min): " << moves[i][j].toString() << endl; cout << "current_eval (min): " << current_evaluation << endl; }
                min_evaluation_found = fmin (current_evaluation, min_evaluation_found);
                if (current_evaluation == min_evaluation_found)
                {
                    best_move = min_evaluation_found;
                    best_i = i;
                    best_j = j;
                    if (DEBUG) { cout << "found best move (min): " << best_move << " | " << res.toString() << " at depth: " << depth << endl; }
                }
            }
        }

        res = moves[best_i][best_j];
        return best_move;
    }
}