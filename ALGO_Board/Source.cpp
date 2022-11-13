#include <fstream>
#include <array>
#include <vector>

using namespace std;

typedef struct Point 
{
	int x;
	int y;
} Point;

int N = 0;
int M = 0;



int main(int argc, char* argv[])
{

	vector<Point> ghosts;
	Point player;
	vector<Point> destinations;

	char** board = readScreenFile(argv[1] );

	vector<vector <bool>> boardAsGraph(N);



	// converts screen array to bool array
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
		{
			boardAsGraph[i].push_back(false);
		}

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			if (board[i][j] != '#')
			{
				boardAsGraph[i][j] = true;
			}
		}
	}



}

void solve(vector<vector <bool>>& boardAsGraph, vector<Point>& ghosts, Point& player, vector<Point>& destinations)
{
	string currentPath;
	int minPathSteps = INT16_MAX;


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
			// func return path + YES
			return;
		}


	}

	// func return NO
}



void BFS(vector<vector <bool>>& mat,Point src, Point dest, string& path)
{
	string path = "";

		int dRow[] = { -1, 0, 0, 1 };
	int dCol[] = { 0, -1, 1, 0 };

	// Stores the distance for each
	// cell from the source cell
	int d[N][M];
	memset(d, -1, sizeof d);

	// Distance of start cell is 0
	d[src.GetY()][src.GetX()] = 0;

	// Initialize a visited array to track the positions we are going to explore
	bool visited[N][M];
	memset(visited, false, sizeof visited);

	// Mark source cell as visited
	visited[src.GetY()][src.GetX()] = true;

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
		Point p = curr.pt;

		// If the destination cell is reached, then find the path
		if (p.GetX() == dest.GetX() && p.GetY() == dest.GetY())
		{
			int x = p.GetX(), y = p.GetY();
			int dist = curr.dist;
			// Assign the distance of destination to the distance matrix
			d[p.GetY()][p.GetX()] = dist;
			// Iterate until source is reached
			while (x != src.GetX() || y != src.GetY())
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
			int row = p.GetY() + dRow[i];
			int col = p.GetX() + dCol[i];

			// If the current cell is valid
			// cell and can be traversed
			if (isValid(row, col) && (mat[row][col] == false) && !visited[row][col])
			{
				// Mark the adjacent cells as visited
				visited[row][col] = true;

				// Enqueue the adjacent cells
				Node adjCell = { { col, row }, curr.dist + 1 };
				q.push(adjCell);

				// Update the distance
				// of the adjacent cells
				d[row][col] = curr.dist + 1;
			}
		}
	}
	// If the destination is not reachable
	if (!missionAcomplished)
		return;





	return path;
}




char** readScreenFile(string fileName) // opened the next screen file, read the data from it, and writes it to the 'board'
{
	int i, j;
	int ghostCount = 0;
	int countBread = 0;
	char currChar;
	bool isLegendInLine;
	bool isHavePacman = false;
	bool isHaveLegend = false;
	char prevChar;
	int placeOfLegend;

	ifstream screenFile(fileName);

	if (!screenFile.is_open()) // fails to open
	{
		isOpened = false;
		//throw screenName[curScreen];
	}

	for (i = 0; i < MAX_ROWS; i++) // fills the board, and converts the file's data to the borad's symbols
	{
		j = 0;
		isLegendInLine = false;
		currChar = screenFile.get();

		while (j < MAX_COLS)
		{
			prevChar = currChar;

			switch (currChar)
			{
			case (char)File_Symbols::PACMAN_FILE:
				pacman.setStartPos(Coord(j, i));
				currChar = (char)Board_Symbols::SPACE;
				isHavePacman = true;
				break;

			case (char)File_Symbols::GHOST_FILE:
				if (ghostCount < MAX_GHOSTS)
				{
					(*ghosts[ghostCount]).setStartPos(Coord(j, i));
					ghostCount++;
				}
				currChar = (char)Board_Symbols::SPACE;
				break;

			case (char)File_Symbols::LEGEND_FILE:
				board.setLegendPos(Coord(j, i));
				currChar = (char)Board_Symbols::SPACE;
				placeOfLegend = j;
				isLegendInLine = true;
				isHaveLegend = true;
				break;

			case (char)File_Symbols::WALL_FILE:
				currChar = (char)Board_Symbols::WALL;
				break;

			case (char)Board_Symbols::SPACE:
				if (j >= board.getWidth())
				{
					currChar = (char)File_Symbols::LIMITS;
				}
				else
				{
					currChar = (char)Board_Symbols::BREAD;
					countBread++;
				}
				break;

			case (char)File_Symbols::LIMITS://%
				break;

			default:
				currChar = (char)File_Symbols::LIMITS;
			}

			board.setCell(i, j, currChar);
			j++;

			currChar = screenFile.get();

			if (currChar == END_LINE)
			{
				if (i == 0)
				{
					if (prevChar == (char)File_Symbols::LEGEND_FILE)
					{
						if (j + 19 > MAX_COLS)
						{
							board.setWidth(MAX_COLS);
						}
						else
						{
							board.setWidth(j + 19);
						}
					}
					else
					{
						board.setWidth(j);
					}
				}

				j = MAX_COLS;
			}

			if (screenFile.eof())
			{
				if (isLegendInLine)
				{
					if (i + 2 >= MAX_ROWS)
					{
						i = MAX_ROWS - 1;
					}
					else
					{
						i += 2;
					}
				}

				board.setHight(i + 1);
				j = MAX_COLS;
				i = MAX_ROWS;
			}
		}

		while (currChar != END_LINE && !screenFile.eof())
		{
			currChar = screenFile.get();
		}
	}

	if (pacman.getCurPos().getX() >= board.getWidth()) // checks the file vaildation
	{
		isHavePacman = false;
	}
	if (board.getLegendPos().getX() >= board.getWidth())
	{
		isHaveLegend = false;
	}

	if (isHavePacman && isHaveLegend)
	{
		isOpened = true;

		board.setCountBread(countBread);

		board.fixLegendArea();

		numOfGhosts = ghostCount;
	}
	else
	{
		isOpened = false;
		throw screenName[curScreen];
	}

}
//--------------------------------------------------------------------------------------//