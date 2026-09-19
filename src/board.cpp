#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

class Cell {
private:
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int adjacentMines;

public:
    Cell() : isMine(false), isRevealed(false), isFlagged(false), adjacentMines(0) {}

    // Getters
    bool hasMine() const { return isMine; }
    bool isOpened() const { return isRevealed; }
    bool isMarked() const { return isFlagged; }
    int getAdjacent() const { return adjacentMines; }

    // Setters
    void setMine(bool val) { isMine = val; }
    void setRevealed(bool val) { isRevealed = val; }
    void setFlagged(bool val) { isFlagged = val; }
    void setAdjacent(int val) { adjacentMines = val; }

    // Reset ô
    void reset() {
        isMine = false;
        isRevealed = false;
        isFlagged = false;
        adjacentMines = 0;
    }
};

class Board {
private:
    int rows, cols, mineCount;
    vector<vector<Cell>> grid;
    int revealedCount;
    int flagCount;
    bool firstClick;

    // Hàm hỗ trợ nội bộ
    bool isValid(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    void placeMines(int safeR, int safeC) {
        // Bảo vệ ô đầu tiên và 8 ô xung quanh
        vector<vector<bool>> forbidden(rows, vector<bool>(cols, false));
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                int nr = safeR + dr, nc = safeC + dc;
                if (isValid(nr, nc)) forbidden[nr][nc] = true;
            }
        }

        vector<pair<int, int>> candidates;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                if (!forbidden[r][c]) candidates.push_back({r, c});

        // Xáo trộn Fisher-Yates
        for (int i = (int)candidates.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(candidates[i], candidates[j]);
        }

        int place = min(mineCount, (int)candidates.size());
        for (int i = 0; i < place; i++) {
            grid[candidates[i].first][candidates[i].second].setMine(true);
        }

        // Tính số mìn kề
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (grid[r][c].hasMine()) continue;
                int count = 0;
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr, nc = c + dc;
                        if (isValid(nr, nc) && grid[nr][nc].hasMine()) count++;
                    }
                }
                grid[r][c].setAdjacent(count);
            }
        }
    }

    int countFlagsAround(int r, int c) const {
        int count = 0;
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr, nc = c + dc;
                if (isValid(nr, nc) && grid[nr][nc].isMarked()) count++;
            }
        }
        return count;
    }

public:
    Board(int r, int c, int m) : rows(r), cols(c), mineCount(m) {
        grid.assign(rows, vector<Cell>(cols));
        revealedCount = 0;
        flagCount = 0;
        firstClick = true;
    }

    // Mở ô (Left Click)
    void reveal(int r, int c) {
        if (!isValid(r, c) || grid[r][c].isOpened() || grid[r][c].isMarked()) return;

        // Lần click đầu tiên -> sinh mìn
        if (firstClick) {
            firstClick = false;
            placeMines(r, c);
        }

        grid[r][c].setRevealed(true);
        revealedCount++;

        if (grid[r][c].hasMine()) return; // Trúng mìn (Game Over sẽ được check bên ngoài)

        // Lan truyền (Flood Fill) nếu ô trống
        if (grid[r][c].getAdjacent() == 0) {
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    reveal(r + dr, c + dc);
                }
            }
        }
    }

    // Cắm cờ (Right Click)
    void toggleFlag(int r, int c) {
        if (!isValid(r, c) || grid[r][c].isOpened()) return;

        if (grid[r][c].isMarked()) {
            grid[r][c].setFlagged(false);
            flagCount--;
        } else {
            if (flagCount >= mineCount) return; // Hết cờ
            grid[r][c].setFlagged(true);
            flagCount++;
        }
    }

    // Chording (Click cả 2 chuột)
    void chord(int r, int c) {
        if (!isValid(r, c) || !grid[r][c].isOpened() || grid[r][c].getAdjacent() == 0 || grid[r][c].hasMine()) return;

        if (countFlagsAround(r, c) == grid[r][c].getAdjacent()) {
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    int nr = r + dr, nc = c + dc;
                    if (isValid(nr, nc) && !grid[nr][nc].isMarked() && !grid[nr][nc].isOpened()) {
                        reveal(nr, nc);
                    }
                }
            }
        }
    }

    // Kiểm tra thắng
    bool isWin() const {
        return revealedCount == rows * cols - mineCount;
    }

    // Kiểm tra thua (có ô mìn nào bị mở không)
    bool isLose() const {
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++)
                if (grid[r][c].hasMine() && grid[r][c].isOpened()) return true;
        return false;
    }

    // Hiển thị bàn cờ
    void print(bool showAll = false) const {
        cout << "===== MINESWEEPER =====\n";
        cout << "Co da dung: " << flagCount << "/" << mineCount << "\n\n";

        cout << "    ";
        for (int c = 0; c < cols; c++) cout << (c / 10 == 0 ? " " : to_string(c / 10)) << " ";
        cout << "\n    ";
        for (int c = 0; c < cols; c++) cout << (c % 10) << " ";
        cout << "\n   +";
        for (int c = 0; c < cols; c++) cout << "--";
        cout << "+\n";

        for (int r = 0; r < rows; r++) {
            cout << setw(2) << r << " |";
            for (int c = 0; c < cols; c++) {
                char ch = '#';
                if (grid[r][c].isOpened()) {
                    if (grid[r][c].hasMine()) ch = '*';
                    else if (grid[r][c].getAdjacent() == 0) ch = '.';
                    else ch = '0' + grid[r][c].getAdjacent();
                } else if (grid[r][c].isMarked()) {
                    ch = 'F';
                } else if (showAll && grid[r][c].hasMine()) {
                    ch = '*';
                }
                cout << ch << " ";
            }
            cout << "|\n";
        }
        cout << "   +";
        for (int c = 0; c < cols; c++) cout << "--";
        cout << "+\n";
    }
};

class Game {
private:
    Board* board;
    bool isGameOver;
    bool isWin;

    void selectDifficulty() {
        int level;
        cout << "Chon do kho:\n";
        cout << "1. De (8x8, 10 min)\n";
        cout << "2. Trung binh (16x16, 40 min)\n";
        cout << "3. Kho (30x16, 99 min)\n";
        cout << "Nhap lua chon (1-3): ";
        cin >> level;

        int r, c, m;
        if (level == 1) { r = 8; c = 8; m = 10; }
        else if (level == 2) { r = 16; c = 16; m = 40; }
        else { r = 16; c = 30; m = 99; } // 30 cot, 16 hang

        board = new Board(r, c, m);
    }

public:
    Game() : board(nullptr), isGameOver(false), isWin(false) {
        srand((unsigned)time(nullptr));
    }

    ~Game() {
        delete board;
    }

    void run() {
        selectDifficulty();

        cout << "\nHuong dan:\n";
        cout << "  r c      -> Mo o (Click trai)\n";
        cout << "  f r c    -> Cam/Bo co (Click phai)\n";
        cout << "  c r c    -> Chord (Mo cac o xung quanh)\n";
        cout << "  q        -> Thoat\n\n";
        cout << "Nhan Enter de bat dau...";
        cin.ignore(); cin.get();

        while (!isGameOver) {
            system("cls || clear");
            board->print();

            string cmd;
            cout << "Nhap lenh: ";
            cin >> cmd;

            if (cmd == "q") {
                cout << "Thoat game.\n";
                return;
            }

            if (cmd == "f" || cmd == "c") {
                int r, c;
                if (!(cin >> r >> c)) {
                    cin.clear(); cin.ignore(10000, '\n');
                    cout << ">> Nhap sai toa do.\n"; continue;
                }
                if (cmd == "f") board->toggleFlag(r, c);
                else board->chord(r, c);
            } 
            else {
                int r, c;
                try { r = stoi(cmd); } 
                catch (...) { cout << ">> Lenh khong hop le.\n"; continue; }
                
                if (!(cin >> c)) {
                    cin.clear(); cin.ignore(10000, '\n');
                    cout << ">> Thieu cot.\n"; continue;
                }

                board->reveal(r, c);
            }

            // Kiểm tra trạng thái game
            if (board->isLose()) {
                isGameOver = true;
                isWin = false;
            } else if (board->isWin()) {
                isGameOver = true;
                isWin = true;
            }
        }

        // Kết thúc
        system("cls || clear");
        board->print(true);
        if (isWin) {
            cout << "\nCHUC MUNG! Ban da chien thang!\n";
        } else {
            cout << "\nBUM! Ban da mo trung min. Thua!\n";
        }
    }
};

int main() {
    char again = 'y';
    while (again == 'y' || again == 'Y') {
        Game game;
        game.run();
        cout << "\nChoi lai? (y/n): ";
        cin >> again;
    }
    cout << "Tam biet!\n";
    return 0;
}