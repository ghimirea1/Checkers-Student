#include "StudentAI.h"

using namespace std;
const bool DEBUG = false;
const int DEPTH = 5;
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

    // Minimax move
    Board board_copy = board;
    Move res;

    // If we are player 1 (Black) we are the maximizing player
    // Otherwise, if we are player 2 (White) we are the minimizing player
    bool max;
    if (player == 1)
    {
        max = true;
    }
    else
    {
        max = false;
    }

    res = mcts (board_copy, res, player);
    board.makeMove (res, player);
    return res;

    // The greater the depth of the initial call to minimax, the deeper the tree and the longer the algorithm runs
    // Now we call minimax to use the minimax algorithm and make the resulting move
    int best_move = minimax (board_copy, res, DEPTH, max);
    if (DEBUG) { cout << "minimax: " << best_move << " res: " << res.toString() << endl; }
    if (DEBUG) { cout << "making move (main): " << res.toString() << endl; }
    board.makeMove (res, player);
    return res;
}

Move StudentAI::mcts (Board board, Move& res, int player)
{
    vector<vector<Move> > moves = board.getAllPossibleMoves (player);
    res = moves[0][0];
    Node n;
    n.board = board;

    MCTS m = MCTS (board, player);
    // cout << "moves: ";
	for (int i = 0; i < moves.size(); i++) 
    {
        for (int j = 0; j < moves[i].size(); j++)
		{
            // cout << moves[i][j].toString() << " | ";
        }
    }
    // cout << endl;

    Move move = m.best_move();
    return move;
}

/**
 * @brief 
 * Minimax algorithm that returns the best move to make on the current state of the board
 * @param board the current state of the board
 * @param res the resulting best move found
 * @param depth the number of levels to perform the algorithm
 * @param max boolean to indicate if the player is maximizing or minimizing
 * @return int the resulting score from the best move found
 */
int StudentAI::minimax (Board board, Move& res, int depth, bool max)
{
    string turn;
    if (max)
    {
        turn = "B";
    }
    else
    {
        turn = "W";
    }

    // If the depth is 0 or if a player has won or tied
    // return the score which is the number of black pieces - the number of white pieces, since black is maximizing and white is minimizing
    if ((depth == 0) || (board.isWin(turn) != 0))
    {
        if (DEBUG) { cout << "returned: " << board.blackCount - board.whiteCount << " at depth: " << depth << endl; }
        return (board.blackCount - board.whiteCount);
    }
    
    // If we are the max player, we choose the best move which is the highest scoring move from the list of possible moves
    // Otherwise, if we are the min player, we choose the best move which is the lowest scoring move from the list of possible moves
    if (max)
    {
        int best_move;
        int best_i;
        int best_j;
        int max_evaluation_found = -99;

        // Get all the possible moves as player 1 (Black)
        // and go through all the moves
        vector<vector<Move> > moves = board.getAllPossibleMoves (1);
        for (int i = 0; i < moves.size(); i++) 
        {
            for (int j = 0; j < moves[i].size(); j++)
            {
                // Make a copy of the board and make the move on the copied board as player 1
                Board board_copy = board;
                if (DEBUG) { cout << "making move (max): " << moves[i][j].toString() << endl; }
                board_copy.makeMove (moves[i][j], 1);
                
                // Recursively call minimax with depth - 1 and max set to false
                int current_evaluation = minimax (board_copy, res, depth - 1, false);
                if (DEBUG) { cout << "made move (max): " << moves[i][j].toString() << endl; cout << "current_eval (max): " << current_evaluation << endl; }
                
                // Set the max evaluation found to be the greater one of the two
                max_evaluation_found = fmax (current_evaluation, max_evaluation_found);
                
                // If the current evaluation is the max evaluation save it as the best move with its corresponding i and j values and move on to the next possible move
                if (current_evaluation == max_evaluation_found)
                {
                    best_move = max_evaluation_found;
                    best_i = i;
                    best_j = j;
                    if (DEBUG) { cout << "found best move (max): " << best_move << " | " << res.toString() << " at depth: " << depth << endl; }
                }
            }
        }

        // After all the possible moves have been gone through, set the result as the best move found's i and j
        // and return the best move
        res = moves[best_i][best_j];
        return best_move;
    }
    else
    {
        int best_move;
        int best_i;
        int best_j;
        int min_evaluation_found = 99;

        // Get all the possible moves as player 2 (White)
        // and go through all the moves
        vector<vector<Move> > moves = board.getAllPossibleMoves (2);
        for (int i = 0; i < moves.size(); i++) 
        {
            for (int j = 0; j < moves[i].size(); j++)
            {
                // Make a copy of the board and make the move on the copied board as player 2
                Board board_copy = board;
                if (DEBUG) { cout << "making move (min): " << moves[i][j].toString() << endl; }
                board_copy.makeMove (moves[i][j], 2);

                // Recursively call minimax with depth - 1 and max set to true
                int current_evaluation = minimax (board_copy, res, depth - 1, true);
                if (DEBUG) { cout << "made move (min): " << moves[i][j].toString() << endl; cout << "current_eval (min): " << current_evaluation << endl; }

                // Set the min evaluation found to be the lowest one of the two
                min_evaluation_found = fmin (current_evaluation, min_evaluation_found);

                // If the current evaluation is the min evaluation save it as the best move with its corresponding i and j values and move on to the next possible move
                if (current_evaluation == min_evaluation_found)
                {
                    best_move = min_evaluation_found;
                    best_i = i;
                    best_j = j;
                    if (DEBUG) { cout << "found best move (min): " << best_move << " | " << res.toString() << " at depth: " << depth << endl; }
                }
            }
        }
        
        // After all the possible moves have been gone through, set the result as the best move found's i and j
        // and return the best move
        res = moves[best_i][best_j];
        return best_move;
    }
}