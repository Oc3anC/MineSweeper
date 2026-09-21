#pragma once

class Cell {
private:
    bool isMine;
    bool isRevealed;
    bool isFlagged;
    int  adjacentMines;

public:
    Cell();

    bool hasMine()     const;
    bool isOpened()    const;
    bool isMarked()    const;
    int  getAdjacent() const;

    void setMine(bool val);
    void setRevealed(bool val);
    void setFlagged(bool val);
    void setAdjacent(int val);

    void reset();
};