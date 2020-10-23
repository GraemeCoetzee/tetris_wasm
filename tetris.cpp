#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include "emscripten.h"

using namespace std;

string piece[7];
vector<int> fullLines;
int maxWidth = 12;
int maxHeight = 22;
char *tetrisField;
int currentXPos;
int currentYPos;
int rotationDegree;
int currentPiece;
bool gameDone;
int score;
char* tempGameState;

void resetPiece() {
    currentXPos = maxWidth / 2;
    currentYPos = 0;
    rotationDegree = 0;
    currentPiece = rand() % 7;
}

int rotatePiece(int newRotationDegree, int x, int y) {
    if(newRotationDegree % 4 == 0) {
        return y * 4 + x;
    } else if(newRotationDegree % 4 == 1) {
        return 12 + y - (x * 4);
    } else if(newRotationDegree % 4 == 2) {
        return 15 - (y * 4) - x;
    } else if(newRotationDegree % 4 == 3) {
        return 3 - y + (x * 4);
    } else {
        return -1;
    }
}

bool testValidMove(int newRotationDegree, int newXCoord, int newYCoord)
{
	for (int x = 0; x < 4; x++)
		for (int y = 0; y < 4; y++)
		{
			int index = rotatePiece(newRotationDegree, x, y);
			int tetrisFieldIndex = (newYCoord + y) * maxWidth + (newXCoord + x);

			if (newXCoord + x >= 0 && newXCoord + x < maxWidth)
			{
				if (newYCoord + y >= 0 && newYCoord + y < maxHeight)
				{
					if (piece[currentPiece][index] != '-' && tetrisField[tetrisFieldIndex] != 0)
						return false;
				}
			}
		}

	return true;
}

void performMove(int keyPressed) {
    if(keyPressed % 4 == 0) {
        if(testValidMove(rotationDegree, currentXPos - 1, currentYPos)) {
            currentXPos--;
        }
    } else if(keyPressed % 4 == 1) {
        if(testValidMove(rotationDegree, currentXPos + 1, currentYPos)) {
            currentXPos++;
        }
    } else if(keyPressed % 4 == 2) {
        if(testValidMove(rotationDegree, currentXPos, currentYPos + 1)) {
            currentYPos++;
        }
    } else if(keyPressed % 4 == 3) {
        if(testValidMove(rotationDegree + 1, currentXPos, currentYPos)) {
            rotationDegree++;
        }
    }
}

int main() {
    srand(time(0));
    return 0;
}


#ifdef __cplusplus
extern "C" {
#endif

inline const char* cstr(const std::string& message) {
    char * cstr = new char [message.length()+1];
    strcpy (cstr, message.c_str());
    return cstr;
}

void EMSCRIPTEN_KEEPALIVE initializeGame() {
    resetPiece();
    gameDone = false;
    score = 0;

    piece[0] = ("--R---R---R---R-"); // Straight Line
    piece[1] = "--O--OO---O-----"; // T Shape
    piece[2] = "-YY--YY---------"; // Square shape
    piece[3] = "-GG---G---G-----"; // LShape
    piece[4] = "-BB--B---B------"; // Inverted L Shape
    piece[5] = "--P--PP--P------"; // Z Shape 
    piece[6] = "-M---MM---M-----"; // Inverted Z Shape

	tetrisField = new char[maxWidth * maxHeight];

	for (int x = 0; x < maxWidth; x++)
		for (int y = 0; y < maxHeight; y++)
			tetrisField[y * maxWidth + x] = (x == 0 || x == maxWidth - 1 || y == maxHeight - 1) ? 9 : 0;

    tempGameState = new char[maxWidth * maxHeight];
    for (int i = 0; i < maxWidth * maxHeight; i++) {
        tempGameState[i] = ' ';
    }
}

double EMSCRIPTEN_KEEPALIVE getScore() {
    return score;
}

const char* EMSCRIPTEN_KEEPALIVE getGameState() {
    for (int x = 0; x < maxWidth; x++)
        for (int y = 0; y < maxHeight; y++)
            tempGameState[(y) * maxWidth + (x)] = " ROYGBPM=W"[tetrisField[y * maxWidth + x]];

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (piece[currentPiece][rotatePiece(rotationDegree, x, y)] != '-') {
                tempGameState[(currentYPos + y) * maxWidth + (currentXPos + x)] = "ROYGBPM"[currentPiece]; 
            }
        }
    }

    string gameState = "";
    for(int i = 0; i < maxWidth * maxHeight; i++) {
        gameState += tempGameState[i];
    }

    return cstr(gameState);
}

void fullLinesUpdateScore() {
    for (int y = 0; y < 4; y++) {
        if(currentYPos + y < maxHeight - 1)
        {
            bool lineFlag = true;
            for (int x = 1; x < maxWidth - 1; x++) {
                lineFlag &= (tetrisField[(currentYPos + y) * maxWidth + x]) != 0;
            }

            if (lineFlag)
            {
                fullLines.push_back(currentYPos + y);
            }						
        }
    }

    score += 50;

    if(!fullLines.empty())	score += fullLines.size() * 100;

    for (int i = 0; i < fullLines.size(); i++) {
        int line = fullLines[i];
        for (int x = 1; x < maxWidth - 1; x++)
        {
            for (int y = line; y > 0; y--)
                tetrisField[y * maxWidth + x] = tetrisField[(y - 1) * maxWidth + x];
            tetrisField[x] = 0;
        }
    }

    fullLines.clear();
}

void setPieceInPlace() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (piece[currentPiece][rotatePiece(rotationDegree, x, y)] != '-') {
                tetrisField[(currentYPos + y) * maxWidth + (currentXPos + x)] = currentPiece + 1;
            }
        }
    }
}

bool EMSCRIPTEN_KEEPALIVE movePiece(bool movePieceDown, int key) {
    if(!movePieceDown) {
        performMove(key);
    } else {        
        if (testValidMove(rotationDegree, currentXPos, currentYPos + 1)) {
            currentYPos++; 
        }
        else
        {
            setPieceInPlace();
            fullLinesUpdateScore();
            resetPiece();
            gameDone = !testValidMove(rotationDegree, currentXPos, currentYPos);
        }
    }

    return gameDone;
}

#ifdef __cplusplus
}
#endif