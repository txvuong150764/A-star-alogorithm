#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <queue>
#include <algorithm>

using namespace std;

int x[] = {0, -1, 0, 1};
int y[] = {1, 0, -1, 0};

enum class State
{
  kStart,
  kFinish,
  kEmpty,
  kObstacle,
  kClose,
  kPath
};

vector<State> ParseLine(string line)
{
  istringstream sline(line);
  int n;
  char c;
  vector<State> row;
  while (sline >> n >> c && c == ',')
  {
    row.push_back(n == 0 ? State::kEmpty : State::kObstacle);
  }

  return row;
}

vector<vector<State>> ReadBoard(string path)
{
  ifstream myfile;
  string line;
  vector<vector<State>> board;
  myfile.open(path);
  if (myfile)
  {
    while (getline(myfile, line))
    {
      board.push_back(ParseLine(line));
    }
  }

  return board;
}

string CellString(State state)
{
  switch (state)
  {
  case State::kStart:
    return "\x53   ";
    break;
  case State::kFinish:
    return "\x45   ";
    break;
  case State::kObstacle:
    return "\xDB   ";
    break;
  case State::kClose:
    return "\6   ";
    break;
  case State::kPath:
    return "-   ";
    break;
  default:
    return "0   ";
    break;
  }
}

void PrintBoard(vector<vector<State>> board)
{
  for (int i = 0; i < board.size(); i++)
  {
    for (int j = 0; j < board[i].size(); j++)
    {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}

bool isValid(int tmpRow, int tmpCol, vector<vector<State>> &board)
{
  bool on_x = tmpRow < board.size() && tmpCol >= 0;
  bool on_y = tmpCol < board[0].size() && tmpRow >= 0;

  if (on_x && on_y)
    return board[tmpRow][tmpCol] == State::kEmpty;
  return false;
}

// BFS to fill Heuristic Table
// vector<vector<int>> HeuristicTable(int row, int column, int desRow, int desCol) {
//   int x[] = {0, -1, 0, 1};
//   int y[] = {1, 0, -1, 0};
//   vector<vector<int>> board(row, vector<int>(column, INT32_MAX));
//   vector<vector<int>> visited(row, vector<int>(column, false));
//   queue<pair<pair<int, int>, int>> q;
//   board[desRow][desCol] = 0;
//   q.push(make_pair(make_pair(desRow, desCol), 0));
//   visited[desRow][desCol] = true;

//   while(!q.empty()) {
//     pair<pair<int, int>, int> top = q.front();
//     q.pop();

//     for(int i = 0; i < 4; i++) {
//       int tmpRow = top.first.first;
//       int tmpCol = top.first.second;
//       if (isValid(row, column, tmpRow + y[i], tmpCol + x[i]) && !visited[tmpRow + y[i]][tmpCol + x[i]])
//       {
//         board[tmpRow + y[i]][tmpCol + x[i]] = min(board[tmpRow + y[i]][tmpCol + x[i]], top.second + 1);
//         q.push(make_pair(make_pair(tmpRow + y[i], tmpCol + x[i]), board[tmpRow + y[i]][tmpCol + x[i]]));
//         visited[tmpRow + y[i]][tmpCol + x[i]] = true;
//       }
//     }
//   }

//   return board;
// }

int Heuristic(int x1, int y1, int x2, int y2)
{
  return abs(x1 - x2) + abs(y1 - y2);
}

bool Compare(vector<int> a, vector<int> b)
{
  return (a[2] + a[3] - b[2] - b[3]) > 0;
}

void CellSort(vector<vector<int>> *v)
{
  sort(v->begin(), v->end(), Compare);
}

void AddToQueue(int x, int y, int g, int h, vector<vector<int>> &q, vector<vector<State>> &board)
{
  q.push_back({x, y, g, h});
  board[x][y] = State::kClose;
}

void ExpandNeighbor(vector<int> current, int end[2], vector<vector<int>> &q, vector<vector<State>> &board)
{
  for (int i = 0; i < 4; i++)
  {
    if (isValid(current[0] + x[i], current[1] + y[i], board))
    {
      AddToQueue(current[0] + x[i], current[1] + y[i], current[2] + 1, Heuristic(current[0] + x[i], current[1] + y[i], end[0], end[1]), q, board);
    }
  }
}

vector<vector<State>> Search(vector<vector<State>> board, int start[2], int end[2])
{
  vector<vector<int>> q = {};
  AddToQueue(start[0], start[1], 0, Heuristic(start[0], start[1], end[0], end[1]), q, board);

  while (q.size() > 0)
  {
    CellSort(&q);
    auto current = q.back();
    q.pop_back();
    board[current[0]][current[1]] = State::kPath;

    if (current[0] == end[0] && current[1] == end[1])
    {
      board[start[0]][start[1]] = State::kStart;
      board[end[0]][end[1]] = State::kFinish;
      return board;
    }

    ExpandNeighbor(current, end, q, board);
  }
}

int main()
{
  vector<vector<State>> board = ReadBoard("./1.board");
  int start[2] = {0, 0};
  int end[2] = {4, 5};
  auto res = Search(board, start, end);
  PrintBoard(res);
}