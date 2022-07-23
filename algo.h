#include "players.h"
#include <random>

using namespace std;

auto rd = std::random_device{};
auto rng = std::default_random_engine{rd()}; // random engine for randomization

/*Minimax algorithm with alpha beta pruning to compute next move. Searches for a given depth and tries to maximize score of max_agent
Return type is <best_move_row, best_move_col, best_move_vertical, best_move_score>
*/
tuple<int, int, bool, int> Computer::minimax(Board &game, int depth, int alpha, int beta, bool maximize, char &max_agent, char &min_agent) const
{
	if (depth == 0 || game.finished()) // return evaluation of position to stop search
	{
		return {0, 0, 0, minimax_heuristic(game, max_agent, min_agent)};
	}
	if (maximize) // maximizing agent's move
	{
		tuple<int, int, bool, int> max_move = {0, 0, 0, -1000};
		for (const auto &[row, col, vertical] : allowed_moves(game))
		{
			int previous_score = game.get_score(max_agent);
			game.add_move(row, col, vertical, max_agent);
			tuple<int, int, bool, int> eval = {0, 0, 0, 0};
			if (previous_score < game.get_score(max_agent)) // check if move switched turns
			{
				eval = minimax(game, depth - 1, alpha, beta, true, max_agent, min_agent);
			}
			else
			{
				eval = minimax(game, depth - 1, alpha, beta, false, max_agent, min_agent);
			}
			if (get<3>(eval) > get<3>(max_move))
			{
				max_move = {row, col, vertical, get<3>(eval)};
			}
			game.remove_move(row, col, vertical);
			alpha = max(alpha, get<3>(eval)); // alpha beta pruning
			if (beta <= alpha)
			{
				break;
			}
		}
		return max_move;
	}
	else // almost same code as maximizing agent
	{
		tuple<int, int, bool, int> min_move = {0, 0, 0, 1000};
		for (const auto &[row, col, vertical] : allowed_moves(game))
		{
			int previous_score = game.get_score(min_agent);
			game.add_move(row, col, vertical, min_agent);
			tuple<int, int, bool, int> eval = {0, 0, 0, 0};
			if (previous_score < game.get_score(min_agent))
			{
				eval = minimax(game, depth - 1, alpha, beta, false, max_agent, min_agent);
			}
			else
			{
				eval = minimax(game, depth - 1, alpha, beta, true, max_agent, min_agent);
			}
			if (get<3>(eval) < get<3>(min_move))
			{
				min_move = {row, col, vertical, get<3>(eval)};
			}
			game.remove_move(row, col, vertical);
			beta = min(beta, get<3>(eval));
			if (beta <= alpha)
			{
				break;
			}
		}
		return min_move;
	}
}

/*heuristic function to evaluate the score of a particular position.
TODO: Improve it with some rule-based functions
*/
int Computer::minimax_heuristic(Board &game, char &max_agent, char &min_agent) const
{
	return game.get_score(max_agent) - game.get_score(min_agent);
}

vector<tuple<int, int, bool>> Computer::allowed_moves(Board &game) const
{
	vector<tuple<int, int, bool>> edges_3, edges_0_1, edges_2; // moves adjacent to squares with 3 edges, 0|1 edges, 2 edges
	for (int vertical = 0; vertical <= 1; vertical++)
	{
		for (int row = 0; row < game.size + 1 - vertical; row++)
		{
			for (int col = 0; col < game.size + vertical; col++)
			{
				if ((vertical && game.vertical_dashes[row][col]) || (!vertical && game.horizontal_dashes[row][col])) // check if edge is filled
				{
					continue;
				}
				bool edge_2 = false, edge_3 = false; // has square with 2 edges, 3 edges. Used to sort moves in a priority order
				for (auto square : get_neighbours(game, row, col, vertical))
				{
					if (filled_edges(game, square.first, square.second) == 2)
					{
						edges_2.push_back({row, col, vertical});
						edge_2 = true;
						break;
					}
					if (filled_edges(game, square.first, square.second) == 3)
					{
						edge_3 = true;
					}
				}
				if (!edge_2)
				{
					if (edge_3)
					{
						edges_3.push_back({row, col, vertical});
					}
					else
					{
						edges_0_1.push_back({row, col, vertical});
					}
				}
			}
		}
	}
	// shuffle to randomize
	shuffle(edges_3.begin(), edges_3.end(), rng);
	shuffle(edges_0_1.begin(), edges_0_1.end(), rng);
	shuffle(edges_2.begin(), edges_2.end(), rng);
	edges_3.insert(edges_3.end(), edges_0_1.begin(), edges_0_1.end());
	edges_3.insert(edges_3.end(), edges_2.begin(), edges_2.end());
	return edges_3;
}

/*Returns list of squares that contain edge with coordinate (row, col, vertical)*/
vector<pair<int, int>> Computer::get_neighbours(Board &game, const int &row, const int &col, const bool &vertical) const
{
	vector<pair<int, int>> neighbours;
	if (vertical)
	{
		if (col > 0)
		{
			neighbours.push_back({row, col - 1});
		}
		if (col < game.size)
		{
			neighbours.push_back({row, col});
		}
	}
	else
	{
		if (row > 0)
		{
			neighbours.push_back({row - 1, col});
		}
		if (row < game.size)
		{
			neighbours.push_back({row, col});
		}
	}
	return neighbours;
}

/*Returns number of filled_edges in square at position (row, col)*/
int Computer::filled_edges(Board &game, int &row, int &col) const
{
	int sum = 0;
	sum += game.horizontal_dashes[row][col];
	sum += game.vertical_dashes[row][col];
	sum += game.horizontal_dashes[row + 1][col];
	sum += game.vertical_dashes[row][col + 1];
	return sum;
}