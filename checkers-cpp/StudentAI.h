#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#pragma once
#include <random>
#include <ctime>
#include <cmath>
#include <string>
const bool DEBUG = false;

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);
	Move mcts (Board board, Move& res, int player);
	Move minimax_move (Board board, int player);
	int minimax (Board board, Move& res, int depth, bool max, int alpha, int beta);
	int count_kings (Board b, int player);
};

class Node
{
	public:
	Node* parent;
	vector <Node> children;
	Board board;
	int player;
	double wins;
	int simulations;
	double uct;
	Move move;

	Node ()
	{
		Node* parent = NULL;
		children = vector<Node>();
		board = Board();
		player = 0;
		wins = 0;
		simulations = 0;
		uct = 0;
		move = Move();
	}
	Node (Node* p, vector<Node> c, Board b, int pl, int w, int s, double u, Move m)
	{
		parent = p;
		children = c;
		board = b;
		player = pl;
		wins = w;
		simulations = s;
		uct = u;
		move = m;
	}

	void update_uct()
	{
		if (simulations)
		{
			uct = ((wins / simulations) + (2 * sqrt (log (parent->simulations) / simulations)));
		}
	}

	double get_uct()
	{
		update_uct();
		return uct;
	}

	int generate_children()
	{
		vector<vector<Move> > moves = board.getAllPossibleMoves (player);
		for (int i = 0; i < moves.size(); i++) 
        {
            for (int j = 0; j < moves[i].size(); j++)
			{
				children.push_back (Node (this, vector<Node>(), board, player, 0, 0, 0, moves[i][j]));
			}

		}

		for (int i = 0; i < children.size(); i++)
		{
			children[i].make_move();
		}

		return children.size();
	}

	void make_move()
	{
		board.makeMove (move, player);
	}

	void make_random_opponent_move()
	{
		int opponent;
		if (player == 1)
		{
			opponent = 2;
		}
		else
		{
			opponent = 1;
		}

		vector<vector<Move> > moves = board.getAllPossibleMoves (opponent);
		
		if (moves.size())
		{
			srand (time(NULL));
			int rand_i = rand() % moves.size();
			int rand_j = rand() % moves[rand_i].size();
			Move opponent_move = moves[rand_i][rand_j];
			board.makeMove (opponent_move, opponent);
		}
	}

};

class MCTS
{
	public:
	MCTS (Board b, int player)
	{		
		srand (time(NULL));
		root = Node();
		root.parent = NULL;
		root.board = b;
		root.player = player;
		root.generate_children();
	}

	Node root;
	Node selection (Node node)
	{
		double highest_utc_found = -999;
		int highest_utc_found_index = -999;
		for (int i = 0; i < node.children.size(); i++)
		{
			highest_utc_found = fmax (highest_utc_found, node.children[i].get_uct());
			if (node.children[i].get_uct() == highest_utc_found)
			{
				highest_utc_found_index = i;

			}
		}

		if (highest_utc_found_index != -999)
		{
			return selection (node.children[highest_utc_found_index]);
		}

		return node;
	}

	Node* selection (Node* node)
	{
		double highest_utc_found = -999;
		int highest_utc_found_index = -999;
		for (int i = 0; i < node->children.size(); i++)
		{
			highest_utc_found = fmax (highest_utc_found, node->children[i].get_uct());
			if (node->children[i].get_uct() == highest_utc_found)
			{
				highest_utc_found_index = i;

			}
		}

		if (highest_utc_found_index != -999)
		{
			return selection (&node->children[highest_utc_found_index]);
		}
		
		return node;
	}

	int simulation (Node node)
	{
		Node n = node;
		while (node.board.isWin (node.player) == 0)
		{
			node.generate_children();
			node.make_random_opponent_move();
			if (node.children.size() == 0) { break; }
			node = selection (node.children[rand() % node.children.size()]);
		}

		int score = node.board.blackCount - node.board.whiteCount;
		// player 1 is Black
		if ((node.player == 1) && (node.board.isWin (node.player) == 1))
		{
			return 1;
		}
		if ((node.player == 2) && (node.board.isWin (node.player) == 2))
		{
			return 1;
		}
		if (node.board.isWin (node.player) == -1)
		{
			return -1;
		}

		return 0;
	}

	void backpropagation (Node* node, int result)
	{
		while (node->parent != NULL)
		{	
			node->simulations++;
			
			if (result == 1)
			{
				node->wins++;
			}
			else if (result == -1)
			{
				node->wins += 0.5;
			}

			node->update_uct();
			node = node->parent;
		}
	}

	Move best_move ()
	{
		for (int i = 0; i < root.children.size(); i++)
		{
			root.simulations += 1;
			int result = simulation (root.children[i]);
			backpropagation (&root.children[i], result);
		}

		for (int i = 0; i < 1000; i++)
		{
			Node* n = selection (&root);
			if (n->children.size() == 0)
			{
				n->generate_children();
			}

			root.simulations += 1;
			if (n->children.size() == 0) { break; }
			Node* random_node = &n->children[rand() % n->children.size()];
			random_node->make_random_opponent_move();
			int result = simulation (*random_node);
			backpropagation (random_node, result);
		}

		if (DEBUG)
		{
		for (int i = 0; i < root.children.size(); i++)
			{
				int w = root.children[i].wins;
				int s = root.children[i].simulations;
				int p = root.children[i].parent->simulations;
				double uct = root.children[i].get_uct();
				cout << "[" << root.children[i].move.toString() << "]" << "[" << w << "]" << "[" << s << "]" << "[" << p << "]" << "[" << uct << "] | ";
			}
		}

		double highest_utc_found = -999;
		int highest_utc_found_index = -999;
		for (int i = 0; i < root.children.size(); i++)
		{
			highest_utc_found = fmax (highest_utc_found, root.children[i].get_uct());
			if (root.children[i].get_uct() == highest_utc_found)
			{
				highest_utc_found_index = i;

			}
		}

		return root.children[highest_utc_found_index].move;
	}
};

#endif //STUDENTAI_H
