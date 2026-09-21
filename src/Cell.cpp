#include "Cell.h"

Cell::Cell()
    : isMine(false), isRevealed(false), isFlagged(false), adjacentMines(0) {}

bool Cell::hasMine()     const { return isMine; }
bool Cell::isOpened()    const { return isRevealed; }
bool Cell::isMarked()    const { return isFlagged; }
int  Cell::getAdjacent() const { return adjacentMines; }

void Cell::setMine(bool val)     { isMine = val; }
void Cell::setRevealed(bool val) { isRevealed = val; }
void Cell::setFlagged(bool val)  { isFlagged = val; }
void Cell::setAdjacent(int val)  { adjacentMines = val; }

void Cell::reset() {
    isMine        = false;
    isRevealed    = false;
    isFlagged     = false;
    adjacentMines = 0;
}