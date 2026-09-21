#include "Game.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

Game::Game()
    : board(nullptr), isGameOver(false), isWin(false) {
    srand((unsigned)time(nullptr));
}

Game::~Game() {
    delete board;   // Giải phóng bộ nhớ
}

void Game::selectDifficulty() {
    int level;
    cout << "Chon do kho:\n";
    cout << "1. De (8x8, 10 min)\n";
    cout << "2. Trung binh (16x16, 40 min)\n";
    cout << "3. Kho (30x16, 99 min)\n";
    cout << "Nhap lua chon (1-3): ";
    cin >> level;

    int r, c, m;
    if (level == 1)      { r = 8;  c = 8;  m = 10; }
    else if (level == 2) { r = 16; c = 16; m = 40; }
    else                 { r = 16; c = 30; m = 99; }

    board = new Board(r, c, m);
}

void Game::run() {
    selectDifficulty();

    cout << "\nHuong dan:\n";
    cout << "  r c      -> Mo o (Click trai)\n";
    cout << "  f r c    -> Cam/Bo co (Click phai)\n";
    cout << "  c r c    -> Chord (Mo cac o xung quanh)\n";
    cout << "  q        -> Thoat\n\n";
    cout << "Nhan Enter de bat dau...";
    cin.ignore(); cin.get();

    while (!isGameOver) {
        // Xóa màn hình (portable)
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        board->print();

        string cmd;
        cout << "Nhap lenh: ";
        cin >> cmd;

        if (cmd == "q") {
            cout << "Thoat game.\n";
            return;
        }

        // Lệnh cắm cờ / chord
        if (cmd == "f" || cmd == "c") {
            int r, c;
            if (!(cin >> r >> c)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << ">> Nhap sai toa do.\n";
                continue;
            }
            if (cmd == "f") board->toggleFlag(r, c);
            else            board->chord(r, c);
        }
        // Lệnh mở ô (dạng số)
        else {
            int r, c;
            try {
                r = stoi(cmd);
            } catch (...) {
                cout << ">> Lenh khong hop le.\n";
                continue;
            }
            if (!(cin >> c)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << ">> Thieu cot.\n";
                continue;
            }
            board->reveal(r, c);
        }

        // Kiểm tra kết thúc
        if (board->isLose()) {
            isGameOver = true;
            isWin = false;
        } else if (board->isWin()) {
            isGameOver = true;
            isWin = true;
        }
    }

    // Hiển thị kết quả cuối
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    board->print(true);

    if (isWin)
        cout << "\nCHUC MUNG! Ban da chien thang!\n";
    else
        cout << "\nBUM! Ban da mo trung min. Thua!\n";
}