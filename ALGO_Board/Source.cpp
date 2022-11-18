#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include <iostream>
#include <chrono>
#include <map>
using namespace std;

typedef struct Point 
{
	int x, y;
	bool isExit;

	Point(int x = 0, int y = 0, bool isExit = false, bool isGhost = false) :x(x), y(y), isExit(isExit) {} // defult ctor
} Point;

typedef struct Node {
	Point p;
	int distance;
	bool isGhost;
} Node;

typedef struct Path {
	Point p;
	string path;
} Path;

int N = 0;
int M = 0;

//--------------------------------------------------------------------------------------//
bool isValid(int row, int col);
void writeAnswerToFile(string fileName, bool foundPath, string path);
void solve(vector<vector <int>>& board, vector<Point>& ghosts, Point& player, vector<Point>& destinations);
void BFS(vector<vector<int>>& d, vector<vector<bool>>& visited, vector<vector <bool>>& mat, Point src, Point dest, string& path);
vector<string> readFile(string fileName, Point& player, vector<Point>& ghosts, vector<Point>& destinations);
void getPath(string& path, Node curr, vector<vector<int>>& distances, Point src);
bool BFSMeshupar(vector<vector <int>>& board,	Point src, vector<Point>& ghosts, vector<Point>& dest, string& path);
void solveYashan(vector<vector <bool>>& boardAsGraph, vector<Point>& ghosts, Point& player, vector<Point>& destinations);

//--------------------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
	vector<Point> ghosts;
	Point player;
	vector<Point> destinations;
	vector<string> board = readFile(argv[1], player, ghosts, destinations);

	vector<vector <int>> walls(N);
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			if (board[i][j] == '#')
			{
				walls[i].push_back(0); // 0 == wall 
			}
			else if (board[i][j] == 'M')
			{
				walls[i].push_back(2); // 2 == ghost
			}
			else
			{
				if (j == 0 || i == 0 || j == M - 1 || i == N - 1)
				{
					walls[i].push_back(3); // 3 == exit point
				}
				else
				{
					walls[i].push_back(1); // 1 == empty cell
				}
			}
		}

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	auto t1 = high_resolution_clock::now();
	solve(walls, ghosts, player, destinations);
	auto t2 = high_resolution_clock::now();
	auto ms_int = duration_cast<milliseconds>(t2 - t1);
	duration<double, std::milli> time_taken = t2 - t1;	
	cout << "Solved successfully " << time_taken.count() / 1000 << " seconds" << endl;
}
//--------------------------------------------------------------------------------------//
bool isValid(int row, int col)
{ 
	return (row >= 0) && (col >= 0) && (row < N) && (col < M); 
}
//--------------------------------------------------------------------------------------//
void solve(vector<vector <int>>& board, vector<Point>& ghosts, Point& player, vector<Point>& destinations)
{
	string path = "";
	bool pathFound = BFSMeshupar(board, player, ghosts, destinations, path);
	if (pathFound) 
	{
		writeAnswerToFile("output.txt", true, path);
	}
	else 
	{
		writeAnswerToFile("output.txt", false, "");
	}
}
//--------------------------------------------------------------------------------------//
void BFS(vector<vector<int>>& distances, vector<vector<bool>>& visited, vector<vector <bool>>& mat,Point src, Point dest, string& path)
{
	int dRow[] = { -1, 0, 0, 1 };
	int dCol[] = { 0, -1, 1, 0 };
	bool missionAcomplished = false; 	// Keeps track of whether pacman has been reached or not.

	// init vectors
	for_each(distances.begin(), distances.end(),
		[](auto& sub) {
			std::fill(sub.begin(), sub.end(), -1);
		});
	for_each(visited.begin(), visited.end(),
		[](auto& sub) {
			std::fill(sub.begin(), sub.end(), false);
		});

	// Distance of start cell is 0
	distances[src.y][src.x] = 0;
	visited[src.y][src.x] = true;

	queue<Node> queue;
	Node source = { src, 0 }; 	// Distance of source cell is 0
	queue.push(source); 	// Enqueue source cell

	while (!queue.empty())
	{
		Node curr = queue.front();
		Point p = curr.p;

		// If the destination cell is reached, then find the path
		if (p.x == dest.x && p.y == dest.y)
		{
			getPath(path, curr, distances, src);

			// Reverse the backtracked path
			reverse(path.begin(), path.end());
			missionAcomplished = true;
			break;
		}
		// Pop the start of queue
		queue.pop();
		// Explore all adjacent directions
		for (int i = 0; i < 4; i++)
		{
			int row = p.y + dRow[i];
			int col = p.x + dCol[i];

			// If the current cell is valid cell and can be traversed
			if (isValid(row, col) && (mat[row][col]) && !visited[row][col])
			{
				// Mark the adjacent cells as visited
				visited[row][col] = true;

				// Enqueue the adjacent cells
				Node adjCell = { { col, row }, curr.distance + 1 };
				queue.push(adjCell);

				// Update the distance of the adjacent cells
				distances[row][col] = curr.distance + 1;
			}
		}
	}
	// If the destination is not reachable
	if (!missionAcomplished)
		return;
}
//--------------------------------------------------------------------------------------//
vector<string> readFile(string fileName, Point& player, vector<Point>& ghosts, vector<Point>& destinations) // opened the next screen file, read the data from it, and writes it to the 'board'
{
	char currChar;

	ifstream file(fileName);

	if (!file.is_open()) // fails to open
	{
		cout << "cant open file" << endl;
	}

	// read N & M first
	file >> N >> M;
	currChar = file.get(); // get the extra \n

	vector<string> board(N);

	for (int i = 0; i < N; i++) // fills the board, and converts the file's data to the borad's symbols
		for (int j = 0; j < M; j++)
		{
			currChar = file.get();
			if (currChar == '\n') 
			{
				currChar = file.get();
			}

			switch (currChar)
			{
			case 'A':
				player = Point(j, i);
				break;

			case 'M':
				ghosts.push_back(Point(j, i));
				break;

			case '.': // exit points == dest
				if (j == 0 || i == 0 || j == M - 1 || i == N - 1)
					destinations.push_back(Point(j, i, true));
				break;

			case '#':
				break;
			default:
				cout << "bad file. could not read all characters";
				return board;
			}
			board[i] += currChar;
		}
	file.close();
	return board;
}
//--------------------------------------------------------------------------------------//
void writeAnswerToFile(string fileName, bool foundPath, string path)
{
	ofstream file(fileName);
	if (foundPath) 
	{
		file << "YES" << endl;
		file << path.length() << endl;
		file << path;
	}
	else 
	{
		file << "NO" << endl;
	}
	file.close();
}
//--------------------------------------------------------------------------------------//
void getPath(string& path, Node curr, vector<vector<int>>& distances, Point src)
{
	Point p = curr.p;
	int x = p.x, y = p.y;
	int dist = curr.distance;
	// Assign the distance of destination to the distance matrix
	distances[p.y][p.x] = dist;
	// Iterate until source is reached
	while (x != src.x || y != src.y)
	{

		// Append DOWN
		if (y > 0 && distances[y - 1][x] == dist - 1)
		{
			path += 'D';
			y--;
		}

		// Append UP
		if (y < N - 1 && distances[y + 1][x] == dist - 1)
		{
			path += 'U';
			y++;
		}

		// Append RIGHT
		if (x > 0 && distances[y][x - 1] == dist - 1) {
			path += 'R';
			x--;
		}

		// Append LEFT
		if (x < M - 1 && distances[y][x + 1] == dist - 1)
		{
			path += 'L';
			x++;
		}
		dist--;
	}
}
//--------------------------------------------------------------------------------------//
bool BFSMeshupar(vector<vector <int>>& board, Point src, vector<Point>& ghosts, vector<Point>& dest,string& path)
{
	int dRow[] = { -1, 0, 0, 1 };
	int dCol[] = { 0, -1, 1, 0 };
	bool missionAcomplished = false;

	// init vectors
	vector<vector<int>> distances(N, vector<int>(M, -1));
	vector<vector<bool>> visited(N, vector<bool>(M, false));

	// Distance of start cell is 0
	distances[src.y][src.x] = 0;
	visited[src.y][src.x] = true;

	queue<Node> queue;
	Node source = { src, 0 }; 	// Distance of source cell is 0

	queue.push(source); 	// Enqueue source cell
	
	
	for (int i = 0; i < ghosts.size(); i++) // Enqueue ghosts
	{
		Node ghost = { ghosts[i], 0, true};
		queue.push(ghost); 
	}
	
	int destInd = 0;
	while (!queue.empty())
	{
		Node curr = queue.front();
		Point p = curr.p;
		
		// If the destination cell is reached, then find the path
		if (p.isExit && !curr.isGhost)
		{
			getPath(path, curr, distances, src);
			reverse(path.begin(), path.end()); // Reverse the backtracked path
			missionAcomplished = true;
			break;
		}
		// Pop the start of queue
		queue.pop();
		// Explore all adjacent directions
		for (int i = 0; i < 4; i++)
		{
			int row = p.y + dRow[i];
			int col = p.x + dCol[i];

			if (curr.isGhost) // ghost mode
			{
				// If the current cell is valid cell and can be traversed
				if (isValid(row, col) && (board[row][col] != 0) && !visited[row][col])
				{
					visited[row][col] = true; // block cell for the pacman

					bool isExit = board[row][col] == 3;

					// Enqueue the adjacent cells
					Node adjCell = { { col, row, isExit }, curr.distance + 1, true}; // make sure we enque node as ghost..
					queue.push(adjCell);

					// Update the distance of the adjacent cells
					distances[row][col] = curr.distance + 1;
				}
			}
			else // pacman mode
			{
				// If the current cell is valid cell and can be traversed
				if (isValid(row, col) && (board[row][col] != 0 && board[row][col] != 2) && !visited[row][col])
				{
					// Mark the adjacent cells as visited
					visited[row][col] = true;

					bool isExit = board[row][col] == 3;

					// Enqueue the adjacent cells
					Node adjCell = { { col, row, isExit }, curr.distance + 1 };
					queue.push(adjCell);

					// Update the distance of the adjacent cells
					distances[row][col] = curr.distance + 1;
				}
			}
		}
	}
	// If the destination is not reachable
	return missionAcomplished;
}
//--------------------------------------------------------------------------------------//
void solveYashan(vector<vector <bool>>& boardAsGraph, vector<Point>& ghosts, Point& player, vector<Point>& destinations)
{
	vector<vector<int>> d(N, vector<int>(M, -1));
	vector<vector<bool>> visited(N, vector<bool>(M, false));
	bool pacmanEscaped;

	// goes through destinations
	for (int i = 0; i < destinations.size(); i++)
	{
		pacmanEscaped = true;
		// goes to the player
		string pacmanPath = "";
		BFS(d, visited, boardAsGraph, player, destinations[i], pacmanPath);
		int minPathSteps = pacmanPath.length();

		// goes though ghosts
		for (int j = 0; j < ghosts.size(); j++)
		{
			string currentPath = "";
			BFS(d, visited, boardAsGraph, ghosts[j], destinations[i], currentPath);

			if (currentPath.length() < minPathSteps)
			{
				pacmanEscaped = false;
				break;
			}
		}
		if (pacmanEscaped)
		{
			writeAnswerToFile("output.txt", true, pacmanPath);
			return;
		}
	}

	writeAnswerToFile("output.txt", false, "");
}
//--------------------------------------------------------------------------------------//