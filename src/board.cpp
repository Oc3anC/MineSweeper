#include "Board.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <string>

using namespace std;

// ==================== HÀM HỖ TRỢ ====================

bool Board::isValid(int r, int c) const {
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

void Board::placeMines(int safeR, int safeC) {
    // Bước 1: Đánh dấu ô an toàn (ô đầu + 8 ô xung quanh)
    vector<vector<bool>> forbidden(rows, vector<bool>(cols, false));
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            int nr = safeR + dr, nc = safeC + dc;
            if (isValid(nr, nc)) forbidden[nr][nc] = true;
        }
    }

    // Bước 2: Gom các ô hợp lệ
    vector<pair<int, int>> candidates;
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (!forbidden[r][c]) candidates.push_back({r, c});

    // Bước 3: Xáo trộn Fisher-Yates
    for (int i = (int)candidates.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(candidates[i], candidates[j]);
    }

    // Bước 4: Đặt mìn
    int place = min(mineCount, (int)candidates.size());
    for (int i = 0; i < place; i++) {
        grid[candidates[i].first][candidates[i].second].setMine(true);
    }

    // Bước 5: Tính số mìn kề cho từng ô
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

int Board::countFlagsAround(int r, int c) const {
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

// ==================== CONSTRUCTOR ====================

Board::Board(int r, int c, int m)
    : rows(r), cols(c), mineCount(m),
      grid(r, vector<Cell>(c)),
      revealedCount(0), flagCount(0), firstClick(true) {}

// ==================== HÀNH ĐỘNG NGƯỜI CHƠI ====================

void Board::reveal(int r, int c) {
    if (!isValid(r, c) || grid[r][c].isOpened() || grid[r][c].isMarked()) return;

    // Lần click đầu tiên: sinh mìn an toàn
    if (firstClick) {
        firstClick = false;
        placeMines(r, c);
    }

    grid[r][c].setRevealed(true);
    revealedCount++;

    if (grid[r][c].hasMine()) return;   // Trúng mìn

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

void Board::toggleFlag(int r, int c) {
    if (!isValid(r, c) || grid[r][c].isOpened()) return;

    if (grid[r][c].isMarked()) {
        grid[r][c].setFlagged(false);
        flagCount--;
    } else {
        if (flagCount >= mineCount) return;   // Hết cờ
        grid[r][c].setFlagged(true);
        flagCount++;
    }
}

void Board::chord(int r, int c) {
    if (!isValid(r, c) || !grid[r][c].isOpened()
        || grid[r][c].getAdjacent() == 0 || grid[r][c].hasMine()) return;

    if (countFlagsAround(r, c) == grid[r][c].getAdjacent()) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                int nr = r + dr, nc = c + dc;
                if (isValid(nr, nc)
                    && !grid[nr][nc].isMarked()
                    && !grid[nr][nc].isOpened()) {
                    reveal(nr, nc);
                }
            }
        }
    }
}

// ==================== TRUY VẤN TRẠNG THÁI ====================

bool Board::isWin() const {
    return revealedCount == rows * cols - mineCount;
}

bool Board::isLose() const {
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (grid[r][c].hasMine() && grid[r][c].isOpened())
                return true;
    return false;
}

// ==================== HIỂN THỊ ====================

void Board::print(bool showAll) const {
    cout << "===== MINESWEEPER =====\n";
    cout << "Co da dung: " << flagCount << "/" << mineCount << "\n\n";

    // Header cột (hàng chục + hàng đơn vị)
    cout << "    ";
    for (int c = 0; c < cols; c++)
        cout << (c / 10 == 0 ? " " : to_string(c / 10)) << " ";
    cout << "\n    ";
    for (int c = 0; c < cols; c++) cout << (c % 10) << " ";

    // Đường viền trên
    cout << "\n   +";
    for (int c = 0; c < cols; c++) cout << "--";
    cout << "+\n";

    // Nội dung bàn cờ
    for (int r = 0; r < rows; r++) {
        cout << setw(2) << r << " |";
        for (int c = 0; c < cols; c++) {
            char ch = '#';
            if (grid[r][c].isOpened()) {
                if (grid[r][c].hasMine())              ch = '*';
                else if (grid[r][c].getAdjacent() == 0) ch = '.';
                else                                    ch = '0' + grid[r][c].getAdjacent();
            } else if (grid[r][c].isMarked()) {
                ch = 'F';
            } else if (showAll && grid[r][c].hasMine()) {
                ch = '*';
            }
            cout << ch << " ";
        }
        cout << "|\n";
    }

    // Đường viền dưới
    cout << "   +";
    for (int c = 0; c < cols; c++) cout << "--";
    cout << "+\n";
}