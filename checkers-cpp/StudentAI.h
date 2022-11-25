#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#pragma once
#include <random>
#include <cmath>
#include <string>

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);
	Move mcts (Board board, Move& res, int player);
	int minimax (Board board, Move& res, int depth, bool max);
};

class Node
{
	public:
	Node* parent;
	vector <Node> children;
	Board board;
	int player;
	int wins;
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
		uct = ((wins / simulations) + sqrt (log (parent->simulations) / simulations));
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
		int rand_i = rand() % moves.size();
		if (moves.size() == 0) { break; }
		int rand_j = rand() % moves[rand_i].size();
		if (moves[rand_i].size() == 0) { break; }
		Move opponent_move = moves[rand_i][rand_j];
		board.makeMove (opponent_move, opponent);
	}

};

class MCTS
{
	public:
	MCTS (Board b, int player)
	{
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
			highest_utc_found = fmax (highest_utc_found, node.children[i].uct);
			if (node.children[i].uct == highest_utc_found)
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
			highest_utc_found = fmax (highest_utc_found, node->children[i].uct);
			if (node->children[i].uct == highest_utc_found)
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
			node.make_random_opponent_move();
			vector<vector<Move> > moves = node.board.getAllPossibleMoves (node.player);
			if (moves.size() == 0) { break; }
			int rand_i = rand() % moves.size();
			if (moves[rand_i].size() == 0) { break; }
			int rand_j = rand() % moves[rand_i].size();
			Move move = moves[rand_i][rand_j];
			node.move = move;
			node.make_move();
			node.generate_children();
			node = selection (node);
		}

		int score = node.board.blackCount - node.board.whiteCount;
		// player 1 is Black
		if ((node.player == 1) && (score > 0))
		{
			return 1;
		}
		else if ((node.player == 2) && (score < 0))
		{
			return 1;
		}

		return 0;
	}

	void backpropagation (Node* node, int result)
	{
		while (node->parent != NULL)
		{	
			node->simulations++;
			
			if (result)
			{
				node->wins++;
			}

			node->update_uct();
			node = node->parent;
		}
	}

	Move best_move ()
	{
		Move m;
		for (int i = 0; i < root.children.size(); i++)
		{
			Node n = root.children[i];
			m = n.move;
			root.children[i].make_move();
			root.simulations += 1;
			int result = simulation (n);
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
			
			Node* random_node = &n->children[rand() % n->children.size()];
			random_node->make_move();
			int result = simulation (*random_node);
			backpropagation (random_node, result);		
			
		}

		double highest_utc_found = -999;
		int highest_utc_found_index = -999;
		for (int i = 0; i < root.children.size(); i++)
		{
			highest_utc_found = fmax (highest_utc_found, root.children[i].uct);
			if (root.children[i].uct == highest_utc_found)
			{
				highest_utc_found_index = i;

			}
		}

		return root.children[highest_utc_found_index].move;
	}
};

#endif //STUDENTAI_H
