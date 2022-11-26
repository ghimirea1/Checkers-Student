#include "StudentAI.h"

using namespace std;
const int DEPTH = 10;
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

    vector<vector<Move> > moves = board.getAllPossibleMoves (player);
    
    // Minimax move
    Move res = moves[0][0];
    res = minimax_move (board, player);
    board.makeMove (res, player);
    return res;

    // Mcts move
    // res = mcts (board_copy, res, player);
    // board.makeMove (res, player);
    // return res;
}

Move StudentAI::mcts (Board board, Move& res, int player)
{
    vector<vector<Move> > moves = board.getAllPossibleMoves (player);
    res = moves[0][0];

    MCTS m = MCTS (board, player);
    Move move = m.best_move();
    return move;
}

Move StudentAI::minimax_move (Board board, int player)
{
    vector<vector<Move> > moves = board.getAllPossibleMoves (player);
    Board board_copy = board;
    Move res = moves[0][0];

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

    // The greater the depth of the initial call to minimax, the deeper the tree and the longer the algorithm runs
    // Now we call minimax to use the minimax algorithm and make the resulting move
    int best_move = minimax (board_copy, res, DEPTH, max, -99, 99);
    if (DEBUG) { cout << "minimax: " << best_move << " res: " << res.toString() << endl; }
    if (DEBUG) { cout << "making move (main): " << res.toString() << endl; }
    return res;
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
int StudentAI::minimax (Board board, Move& res, int depth, bool max, int alpha, int beta)
{
    int player;
    if (max)
    {
        player = 1;
    }
    else
    {
        player = 2;
    }

    // If the depth is 0 or if a player has won or tied
    // return the score which is the number of black pieces - the number of white pieces, since black is maximizing and white is minimizing
    if ((depth == 0) || (board.isWin (player) != 0))
    {
        if (DEBUG) { cout << "returned: " << board.blackCount - board.whiteCount << " at depth: " << depth << endl; }
        return ((board.blackCount + count_kings (board, 1)) - (board.whiteCount) + count_kings (board, 2));
    }
    
    // If we are the max player, we choose the best move which is the highest scoring move from the list of possible moves
    // Otherwise, if we are the min player, we choose the best move which is the lowest scoring move from the list of possible moves
    if (max)
    {
        int best_move = 0;
        int best_i = 0;
        int best_j = 0;
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
                int current_evaluation = minimax (board_copy, res, depth - 1, false, alpha, beta);
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
                
                // alpha beta pruning
                alpha = fmax (alpha, best_move);
                if (beta <= alpha)
                {
                    break;
                }
            }
            
            // alpha beta pruning
            if (beta <= alpha)
            {
                break;
            }
        }

        // After all the possible moves have been gone through, set the result as the best move found's i and j
        // and return the best move
        res = moves[best_i][best_j];
        return best_move;
    }
    else
    {
        int best_move = 0;
        int best_i = 0;
        int best_j = 0;
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
                int current_evaluation = minimax (board_copy, res, depth - 1, true, alpha, beta);
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

                // alpha beta pruning
                beta = fmin (beta, best_move);
                if (beta <= alpha)
                {
                    break;
                }
            }
            // alpha beta pruning
            if (beta <= alpha)
            {
                break;
            }
        }
        
        // After all the possible moves have been gone through, set the result as the best move found's i and j
        // and return the best move
        res = moves[best_i][best_j];
        return best_move;
    }
}

int StudentAI::count_kings (Board board, int player)
{
    int kings = 0;

    for (int i = 0; i < board.row; i++)
    {
        for (int j = 0; j < board.col; j++)
        {
            Checker checker = board.board[i][j];
            if (checker.color == "B" && checker.isKing && player == 1)
            {
                kings++;
            }
            
            if (checker.color == "W" && checker.isKing && player == 2)
            {
                kings++;
            }
        }
    }

    return kings;
}