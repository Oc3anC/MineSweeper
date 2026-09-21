#include "Game.h"
#include <iostream>

using namespace std;

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