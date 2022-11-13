#include <fstream>
#include <array>
#include <vector>
#include <queue>
#include <iostream>
using namespace std;

#define MAXCOLS 1000
#define MAXROWS 1000

typedef struct Point 
{
	int x;
	int y;

	Point(int x = 0, int y = 0) :x(x), y(y) {} // defult ctor
} Point;

typedef struct Node {
	Point p;
	int distance;
} Node;

int N = 0;
int M = 0;

//--------------------------------------------------------------------------------------//
bool isValid(int row, int col);
void writeAnswerToFile(string fileName, bool foundPath, string path);
void solve(vector<vector <bool>>& boardAsGraph, vector<Point>& ghosts, Point& player, vector<Point>& destinations);
void BFS(vector<vector <bool>>& mat, Point src, Point dest, string& path);
vector<vector <char>> readFile(string fileName, Point& player, vector<Point>& ghosts, vector<Point>& destinations);
//--------------------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
	vector<Point> ghosts;
	Point player;
	vector<Point> destinations;
	vector<vector <char>> board = readFile(argv[1], player, ghosts, destinations);

	vector<vector <bool>> boardAsGraph(N);
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			if (board[i][j] != '#')
			{
				boardAsGraph[i].push_back(true);
			}
			else 
			{
				boardAsGraph[i].push_back(false);
			}
		}
	solve(boardAsGraph, ghosts, player, destinations);
}
//--------------------------------------------------------------------------------------//
bool isValid(int row, int col)
{ 
	return (row >= 0) && (col >= 0) && (row < MAXROWS) && (col < MAXCOLS); 
}
//--------------------------------------------------------------------------------------//
void solve(vector<vector <bool>>& boardAsGraph, vector<Point>& ghosts, Point& player, vector<Point>& destinations)
{
	string currentPath;
	int minPathSteps = INT32_MAX;


	// goes through destinations
	for (int i = 0; i < destinations.size(); i++)
	{
		// goes though ghosts
		for (int j = 0; j < ghosts.size(); j++)
		{
			BFS(boardAsGraph, ghosts[j], destinations[i], currentPath);

			if (currentPath.length() < minPathSteps)
			{
				minPathSteps = currentPath.length();
			}
		}

		// goes to the player
		BFS(boardAsGraph, player, destinations[i], currentPath);
		if (currentPath.length() < minPathSteps)
		{
			writeAnswerToFile("res.txt", true, currentPath);
			return;
		}
	}

	writeAnswerToFile("res.txt", false, "");
}
//--------------------------------------------------------------------------------------//
void BFS(vector<vector <bool>>& mat,Point src, Point dest, string& path)
{
	int dRow[] = { -1, 0, 0, 1 };
	int dCol[] = { 0, -1, 1, 0 };

	// Stores the distance for each
	// cell from the source cell
	int d[1000][1000];
	memset(d, -1, sizeof d);

	// Distance of start cell is 0
	d[src.y][src.x] = 0;

	// Initialize a visited array to track the positions we are going to explore
	bool visited[1000][1000];
	memset(visited, false, sizeof visited);

	// Mark source cell as visited
	visited[src.y][src.x] = true;

	// Create a queue for BFS algoritem
	queue<Node> q;
	// Distance of source cell is 0
	Node s = { src, 0 };
	// Enqueue source cell
	q.push(s);
	// Keeps track of whether pacman has been reached or not.
	bool missionAcomplished = false;
	// Iterate until queue is not empty
	while (!q.empty())
	{

		// Deque front of the queue
		Node curr = q.front();
		Point p = curr.p;

		// If the destination cell is reached, then find the path
		if (p.x == dest.x && p.y == dest.y)
		{
			int x = p.x, y = p.y;
			int dist = curr.distance;
			// Assign the distance of destination to the distance matrix
			d[p.y][p.x] = dist;
			// Iterate until source is reached
			while (x != src.x || y != src.y)
			{

				// Append DOWN
				if (y > 0 && d[y - 1][x] == dist - 1)
				{
					path += 'D';
					y--;
				}

				// Append UP
				if (y < N - 1 && d[y + 1][x] == dist - 1)
				{
					path += 'U';
					y++;
				}

				// Append RIGHT
				if (x > 0 && d[y][x - 1] == dist - 1) {
					path += 'R';
					x--;
				}

				// Append LEFT
				if (x < M - 1 && d[y][x + 1] == dist - 1)
				{
					path += 'L';
					x++;
				}
				dist--;
			}

			// Reverse the backtracked path
			reverse(path.begin(), path.end());
			missionAcomplished = true;
			break;
		}
		// Pop the start of queue
		q.pop();
		// Explore all adjacent directions
		for (int i = 0; i < 4; i++)
		{
			int row = p.y + dRow[i];
			int col = p.x + dCol[i];

			// If the current cell is valid
			// cell and can be traversed
			if (isValid(row, col) && (mat[row][col] == false) && !visited[row][col])
			{
				// Mark the adjacent cells as visited
				visited[row][col] = true;

				// Enqueue the adjacent cells
				Node adjCell = { { col, row }, curr.distance + 1 };
				q.push(adjCell);

				// Update the distance
				// of the adjacent cells
				d[row][col] = curr.distance + 1;
			}
		}
	}
	// If the destination is not reachable
	if (!missionAcomplished)
		return;
}
//--------------------------------------------------------------------------------------//
vector<vector <char>> readFile(string fileName, Point& player, vector<Point>& ghosts, vector<Point>& destinations) // opened the next screen file, read the data from it, and writes it to the 'board'
{
	int i, j;
	char currChar;

	ifstream file(fileName);

	if (!file.is_open()) // fails to open
	{
		cout << "cant open file" << endl;
	}

	// read N & M first
	file >> N >> M;
	vector<vector <char>> board(N);

	for (i = 0; i < N; i++) // fills the board, and converts the file's data to the borad's symbols
		for (j = 0; j < M; j++)
		{
			currChar = file.get();

			switch (currChar)
			{
			case 'A':
				player = Point(j, i);
				break;

			case 'M':
				ghosts.push_back(Point(j, i));
				break;

			case '.': // exit points == dest
				destinations.push_back(Point(j, i));
				break;

			default:
				cout << "bad file. could not read all characters";
				return;
			}
			board[i].push_back(currChar);
		}
}
//--------------------------------------------------------------------------------------//
void writeAnswerToFile(string fileName, bool foundPath, string path)
{
	ofstream file(fileName);
	if (foundPath) 
	{
		file << "YES" << endl;
		file << path;
	}
	else 
	{
		file << "NO" << endl;
	}
}