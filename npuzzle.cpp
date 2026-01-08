#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cmath>
#include <memory>
#include <cstdlib>
#include <ctime>

using namespace std;

// ============================================================================
// PUZZLE STATE CLASS
// ============================================================================
class PuzzleState {
public:
    vector<vector<int>> board;
    int size;
    pair<int, int> emptyPos;
    int g;  // Cost from start
    int h;  // Heuristic to goal
    string moves;
    shared_ptr<PuzzleState> parent;

    PuzzleState(const vector<vector<int>>& b, int s)
        : board(b), size(s), g(0), h(0), parent(nullptr) {
        findEmptyPosition();
    }

    void findEmptyPosition() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] == 0) {
                    emptyPos = {i, j};
                    return;
                }
            }
        }
    }

    int getFScore() const { return g + h; }

    string toString() const {
        string result;
        for (const auto& row : board) {
            for (int val : row) {
                result += to_string(val) + ",";
            }
        }
        return result;
    }

    bool isGoal(const vector<vector<int>>& goal) const {
        return board == goal;
    }

    void printBoard() const {
        for (const auto& row : board) {
            for (int val : row) {
                cout << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

// ============================================================================
// COMPARATOR FOR PRIORITY QUEUE
// ============================================================================
struct CompareState {
    bool operator()(const shared_ptr<PuzzleState>& a,
                   const shared_ptr<PuzzleState>& b) const {
        if (a->getFScore() == b->getFScore()) {
            return a->h > b->h;
        }
        return a->getFScore() > b->getFScore();
    }
};

// ============================================================================
// GOAL GENERATION
// ============================================================================
vector<vector<int>> generateGoal(int size) {
    vector<vector<int>> goal(size, vector<int>(size, 0));
    int num = 1;
    int top = 0, bottom = size - 1;
    int left = 0, right = size - 1;

    while (top <= bottom && left <= right) {
        for (int i = left; i <= right; i++) goal[top][i] = num++;
        top++;
        for (int i = top; i <= bottom; i++) goal[i][right] = num++;
        right--;
        if (top <= bottom) {
            for (int i = right; i >= left; i--) goal[bottom][i] = num++;
            bottom--;
        }
        if (left <= right) {
            for (int i = bottom; i >= top; i--) goal[i][left] = num++;
            left++;
        }
    }

    goal[size/2][size/2] = 0;
    return goal;
}

// ============================================================================
// GOAL POSITION MAP
// ============================================================================
unordered_map<int, pair<int, int>> buildGoalPositions(const vector<vector<int>>& goal) {
    unordered_map<int, pair<int, int>> goalPos;
    int size = goal.size();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (goal[i][j] != 0) {
                goalPos[goal[i][j]] = {i, j};
            }
        }
    }
    return goalPos;
}

// ============================================================================
// HEURISTIC 1: MANHATTAN DISTANCE
// ============================================================================
int manhattanDistance(const vector<vector<int>>& board,
                     const unordered_map<int, pair<int, int>>& goalPos) {
    int distance = 0;
    int size = board.size();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int tile = board[i][j];
            if (tile != 0) {
                auto it = goalPos.find(tile);
                if (it != goalPos.end()) {
                    auto [goalRow, goalCol] = it->second;
                    distance += abs(i - goalRow) + abs(j - goalCol);
                }
            }
        }
    }
    return distance;
}

// ============================================================================
// HEURISTIC 2: LINEAR CONFLICT
// ============================================================================
int linearConflict(const vector<vector<int>>& board,
                  const unordered_map<int, pair<int, int>>& goalPos) {
    int conflict = 0;
    int size = board.size();

    // Row conflicts
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1; j++) {
            int tile1 = board[i][j];
            if (tile1 == 0) continue;

            auto it1 = goalPos.find(tile1);
            if (it1 == goalPos.end()) continue;
            auto [goalRow1, goalCol1] = it1->second;

            if (goalRow1 == i) {
                for (int k = j + 1; k < size; k++) {
                    int tile2 = board[i][k];
                    if (tile2 == 0) continue;

                    auto it2 = goalPos.find(tile2);
                    if (it2 == goalPos.end()) continue;
                    auto [goalRow2, goalCol2] = it2->second;

                    if (goalRow2 == i && goalCol1 > goalCol2) {
                        conflict += 2;
                    }
                }
            }
        }
    }

    // Column conflicts
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size - 1; i++) {
            int tile1 = board[i][j];
            if (tile1 == 0) continue;

            auto it1 = goalPos.find(tile1);
            if (it1 == goalPos.end()) continue;
            auto [goalRow1, goalCol1] = it1->second;

            if (goalCol1 == j) {
                for (int k = i + 1; k < size; k++) {
                    int tile2 = board[k][j];
                    if (tile2 == 0) continue;

                    auto it2 = goalPos.find(tile2);
                    if (it2 == goalPos.end()) continue;
                    auto [goalRow2, goalCol2] = it2->second;

                    if (goalCol2 == j && goalRow1 > goalRow2) {
                        conflict += 2;
                    }
                }
            }
        }
    }

    return manhattanDistance(board, goalPos) + conflict;
}

// ============================================================================
// HEURISTIC 3: HAMMING DISTANCE
// ============================================================================
int hammingDistance(const vector<vector<int>>& board,
                   const vector<vector<int>>& goal) {
    int count = 0;
    int size = board.size();

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] != 0 && board[i][j] != goal[i][j]) {
                count++;
            }
        }
    }
    return count;
}

// ============================================================================
// SOLVABILITY CHECK
// ============================================================================
bool isSolvable(const vector<vector<int>>& board,
                const vector<vector<int>>& goal) {
    int size = board.size();
    vector<int> flatBoard, flatGoal;
    int emptyRowFromBottom = 0;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (board[i][j] == 0) {
                emptyRowFromBottom = size - i;
            } else {
                flatBoard.push_back(board[i][j]);
            }
            if (goal[i][j] != 0) {
                flatGoal.push_back(goal[i][j]);
            }
        }
    }

    unordered_map<int, int> goalIndex;
    for (size_t i = 0; i < flatGoal.size(); i++) {
        goalIndex[flatGoal[i]] = i;
    }

    vector<int> remapped;
    for (int tile : flatBoard) {
        remapped.push_back(goalIndex[tile]);
    }

    int inversions = 0;
    for (size_t i = 0; i < remapped.size(); i++) {
        for (size_t j = i + 1; j < remapped.size(); j++) {
            if (remapped[i] > remapped[j]) {
                inversions++;
            }
        }
    }

    if (size % 2 == 1) {
        return inversions % 2 == 0;
    } else {
        return (inversions + emptyRowFromBottom) % 2 == 1;
    }
}

// ============================================================================
// NEIGHBOR GENERATION
// ============================================================================
vector<shared_ptr<PuzzleState>> generateNeighbors(
    shared_ptr<PuzzleState> current,
    const unordered_map<int, pair<int, int>>& goalPos,
    const vector<vector<int>>& goal,
    int heuristicChoice) {

    vector<shared_ptr<PuzzleState>> neighbors;
    int row = current->emptyPos.first;
    int col = current->emptyPos.second;
    int size = current->size;

    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    vector<char> moveNames = {'U', 'D', 'L', 'R'};

    for (int i = 0; i < 4; i++) {
        int newRow = row + directions[i].first;
        int newCol = col + directions[i].second;

        if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size) {
            auto newState = make_shared<PuzzleState>(current->board, size);
            swap(newState->board[row][col], newState->board[newRow][newCol]);
            newState->emptyPos = {newRow, newCol};
            newState->g = current->g + 1;

            if (heuristicChoice == 1) {
                newState->h = manhattanDistance(newState->board, goalPos);
            } else if (heuristicChoice == 2) {
                newState->h = linearConflict(newState->board, goalPos);
            } else {
                newState->h = hammingDistance(newState->board, goal);
            }

            newState->moves = current->moves + moveNames[i];
            newState->parent = current;
            neighbors.push_back(newState);
        }
    }
    return neighbors;
}

// ============================================================================
// A* SEARCH
// ============================================================================
tuple<shared_ptr<PuzzleState>, int, int> aStarSearch(
    const vector<vector<int>>& start,
    const vector<vector<int>>& goal,
    int heuristicChoice) {

    int size = start.size();
    auto goalPos = buildGoalPositions(goal);
    auto startState = make_shared<PuzzleState>(start, size);

    if (heuristicChoice == 1) {
        startState->h = manhattanDistance(start, goalPos);
    } else if (heuristicChoice == 2) {
        startState->h = linearConflict(start, goalPos);
    } else {
        startState->h = hammingDistance(start, goal);
    }

    priority_queue<shared_ptr<PuzzleState>,
                   vector<shared_ptr<PuzzleState>>,
                   CompareState> openSet;
    unordered_set<string> closedSet;
    unordered_set<string> inOpenSet;

    int totalOpened = 0;
    int maxInMemory = 0;

    openSet.push(startState);
    inOpenSet.insert(startState->toString());

    while (!openSet.empty()) {
        int currentMemory = openSet.size() + closedSet.size();
        maxInMemory = max(maxInMemory, currentMemory);

        auto current = openSet.top();
        openSet.pop();

        string currentStr = current->toString();
        inOpenSet.erase(currentStr);
        totalOpened++;

        if (current->isGoal(goal)) {
            return {current, totalOpened, maxInMemory};
        }

        closedSet.insert(currentStr);

        auto neighbors = generateNeighbors(current, goalPos, goal, heuristicChoice);

        for (auto& neighbor : neighbors) {
            string neighborStr = neighbor->toString();

            if (closedSet.find(neighborStr) != closedSet.end()) continue;
            if (inOpenSet.find(neighborStr) != inOpenSet.end()) continue;

            openSet.push(neighbor);
            inOpenSet.insert(neighborStr);
        }
    }

    return {nullptr, totalOpened, maxInMemory};
}

// ============================================================================
// FILE PARSING
// ============================================================================
pair<int, vector<vector<int>>> parseInputFile(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }

    int size = 0;
    vector<vector<int>> board;
    string line;

    while (getline(file, line)) {
        size_t commentPos = line.find('#');
        if (commentPos != string::npos) {
            line = line.substr(0, commentPos);
        }

        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;

        istringstream iss(line);
        vector<int> row;
        int num;

        while (iss >> num) {
            row.push_back(num);
        }

        if (size == 0 && row.size() == 1) {
            size = row[0];
            board.reserve(size);
        } else if (row.size() > 0) {
            if (row.size() != static_cast<size_t>(size)) {
                cerr << "Error: Invalid row size" << endl;
                exit(1);
            }
            board.push_back(row);
        }
    }

    file.close();

    if (board.size() != static_cast<size_t>(size)) {
        cerr << "Error: Expected " << size << " rows, got " << board.size() << endl;
        exit(1);
    }

    return {size, board};
}

// ============================================================================
// RANDOM PUZZLE GENERATION
// ============================================================================
vector<vector<int>> generateRandomPuzzle(int size, int numMoves) {
    vector<vector<int>> puzzle = generateGoal(size);

    pair<int, int> emptyPos;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (puzzle[i][j] == 0) {
                emptyPos = {i, j};
                break;
            }
        }
    }

    vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    pair<int, int> lastMove = {0, 0};

    for (int move = 0; move < numMoves; move++) {
        vector<pair<int, int>> validMoves;

        for (const auto& dir : directions) {
            int newRow = emptyPos.first + dir.first;
            int newCol = emptyPos.second + dir.second;

            if (newRow >= 0 && newRow < size &&
                newCol >= 0 && newCol < size &&
                !(dir.first == -lastMove.first && dir.second == -lastMove.second)) {
                validMoves.push_back(dir);
            }
        }

        if (!validMoves.empty()) {
            int idx = rand() % validMoves.size();
            auto dir = validMoves[idx];

            int newRow = emptyPos.first + dir.first;
            int newCol = emptyPos.second + dir.second;

            swap(puzzle[emptyPos.first][emptyPos.second], puzzle[newRow][newCol]);
            emptyPos = {newRow, newCol};
            lastMove = dir;
        }
    }

    return puzzle;
}

// ============================================================================
// OUTPUT
// ============================================================================
void printSolutionPath(shared_ptr<PuzzleState> goalState) {
    vector<shared_ptr<PuzzleState>> path;
    auto current = goalState;

    while (current != nullptr) {
        path.push_back(current);
        current = current->parent;
    }

    reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); i++) {
        cout << "Step " << i << ":" << endl;
        path[i]->printBoard();
    }
}

void printResults(shared_ptr<PuzzleState> solution,
                 int totalOpened,
                 int maxInMemory,
                 const string& heuristicName) {
    cout << "========================================" << endl;
    cout << "           SOLUTION FOUND!" << endl;
    cout << "========================================" << endl;
    cout << "Heuristic used: " << heuristicName << endl;
    cout << "Total states opened (time complexity): " << totalOpened << endl;
    cout << "Max states in memory (space complexity): " << maxInMemory << endl;
    cout << "Solution length: " << solution->g << " moves" << endl;
    cout << "Move sequence: " << solution->moves << endl;
    cout << "========================================" << endl << endl;
}

void printUsage(const char* programName) {
    cout << "Usage: " << programName << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -f <file>     Solve puzzle from file" << endl;
    cout << "  -r <size>     Generate random puzzle" << endl;
    cout << "  -h <1|2|3>    Heuristic: 1=Manhattan, 2=Linear Conflict, 3=Hamming" << endl;
    cout << "  -v            Show solution path" << endl;
    cout << "  --help        Show this help" << endl;
}

// ============================================================================
// MAIN
// ============================================================================
int main(int argc, char* argv[]) {
    srand(time(nullptr));

    string filename = "";
    int randomSize = 0;
    int heuristicChoice = 1;
    bool verbose = false;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-f" && i + 1 < argc) {
            filename = argv[++i];
        } else if (arg == "-r" && i + 1 < argc) {
            randomSize = atoi(argv[++i]);
        } else if (arg == "-h" && i + 1 < argc) {
            heuristicChoice = atoi(argv[++i]);
            if (heuristicChoice < 1 || heuristicChoice > 3) {
                cerr << "Error: Heuristic must be 1, 2, or 3" << endl;
                return 1;
            }
        } else if (arg == "-v") {
            verbose = true;
        } else if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    if (filename.empty() && randomSize == 0) {
        printUsage(argv[0]);
        return 1;
    }

    string heuristicName;
    if (heuristicChoice == 1) heuristicName = "Manhattan Distance";
    else if (heuristicChoice == 2) heuristicName = "Linear Conflict";
    else heuristicName = "Hamming Distance";

    int size;
    vector<vector<int>> startBoard;

    if (!filename.empty()) {
        cout << "Loading puzzle from: " << filename << endl;
        auto parsed = parseInputFile(filename);
        size = parsed.first;
        startBoard = parsed.second;
    } else {
        cout << "Generating random " << randomSize << "x" << randomSize << " puzzle..." << endl;
        size = randomSize;
        startBoard = generateRandomPuzzle(size, size * size * 10);
    }

    cout << "\nStart state:" << endl;
    PuzzleState start(startBoard, size);
    start.printBoard();

    auto goal = generateGoal(size);
    cout << "Goal state:" << endl;
    PuzzleState goalDisplay(goal, size);
    goalDisplay.printBoard();

    cout << "Checking solvability..." << endl;
    if (!isSolvable(startBoard, goal)) {
        cout << "ERROR: This puzzle is UNSOLVABLE!" << endl;
        return 1;
    }
    cout << "Puzzle is solvable!\n" << endl;

    cout << "Solving with " << heuristicName << "..." << endl;

    auto [solution, totalOpened, maxInMemory] = aStarSearch(startBoard, goal, heuristicChoice);

    if (solution == nullptr) {
        cout << "ERROR: Could not find solution!" << endl;
        return 1;
    }

    printResults(solution, totalOpened, maxInMemory, heuristicName);

    if (verbose) {
        cout << "Solution path:" << endl;
        cout << "========================================" << endl;
        printSolutionPath(solution);
    }

    return 0;
}
