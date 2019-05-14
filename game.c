#include "game.h"

#define GameStartX 0
#define GameEndX 8
#define GameStartY 0
#define GameEndY 12

#define BlockSize 0.5
#define BlockMargin 0.05

// 0 - 23 rows
// 0 - 15 column
// coordinate stand for the bottom-left of the block
// 0 stands for not occupied
// different values in map stand for different colors

int map[24][16];

int Tetriminos[10][4][4] = {
	{{1,1,0,0},
	{1,0,0,0},
	{1,0,0,0},
	{0,0,0,0}},//Cis-L 1

	{{2,2,0,0},
	{0,2,0,0},
	{0,2,0,0},
	{0,0,0,0}},//Trans-L 2

	{{3,0,0,0},
	{3,3,0,0},
	{0,3,0,0},
	{0,0,0,0}},//Cis-S 3

	{{0,4,0,0},
	{4,4,0,0},
	{4,0,0,0},
	{0,0,0,0}},//Trans-S 4

	{{5,5,0,0},
	{5,5,0,0},
	{0,0,0,0},
	{0,0,0,0}},//O 5

	{{6,0,0,0},
	{6,0,0,0},
	{6,0,0,0},
	{6,0,0,0}},//I 6

	{{0,7,0,0},
	{7,7,0,0},
	{0,7,0,0},
	{0,0,0,0}}//E 7 

};

struct Dropping {
	// id stands for the kind of tetrimino of which the dropping one is
	// x, y stand for the top-left of the dropping tetrimino matrix
	int id, column, row;
	int mat[4][4];
} drop;

bool isDropping;

void showBlock() {
	int i, j;
	for (i = 0; i < 24; i++)
		for (j = 0; j < 16; j++)
			if (map[i][j] != 0) drawBlock(map[i][j], i, j);
	if (isDropping) {
		for (i = 0; i < 4; i++)
			for (j = 0; j < 4; j++) {
				if (drop.mat[i][j] != 0) {
					drawBlock(drop.id + 1, drop.row - i, drop.column + j);
				}
			}
	}
}

void drawBlock(int color, int row, int column) {
	switch (color) {
	case 1:
		SetPenColor("red");
		break;
	case 2:
		SetPenColor("blue");
		break;
	case 3:
		SetPenColor("magenta");
		break;
	case 4:
		SetPenColor("orange");
		break;
	case 5:
		SetPenColor("violet");
		break;
	case 6:
		SetPenColor("green");
		break;
	case 7:
		SetPenColor("white");
		break;
	}
	drawBlockOuterBorder(row, column);
	drawBlockInnerBorder(row, column);
}

void drawBlockOuterBorder(int row, int column) {
	SetPenSize(2);
	MovePen(column*BlockSize, row*BlockSize);
	DrawLine(BlockSize, 0);
	DrawLine(0, BlockSize);
	DrawLine(-BlockSize, 0);
	DrawLine(0, -BlockSize);
}

void drawBlockInnerBorder(int row, int column) {
	SetPenSize(1);
	// TODO: Figure out why there are minor adjustments.
	MovePen(column*BlockSize+BlockMargin-0.005, row*BlockSize+BlockMargin+0.005);
	DrawLine(BlockSize-2*BlockMargin, 0);
	DrawLine(0, BlockSize-2*BlockMargin);
	DrawLine(-BlockSize+2*BlockMargin, 0);
	DrawLine(0, -BlockSize+2*BlockMargin);
}

int speed = 1, count;

void refreshGame() {
	if (!isDropping) {
		count = 0;
		drop.id = (int)(rand() * 10) % 7;
		memcpy(drop.mat, Tetriminos[drop.id], sizeof(drop.mat));
		drop.column = 6;
		drop.row = 23;
		isDropping = 1;
		return;
	}
	if (count == speed) {
		count = 0;
		dropIt();
		return;
	}
	count++;
}

void dropIt() {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			if (drop.mat[i][j] != 0 && (map[drop.row - i - 1][drop.column + j] != 0 || drop.row - i == 0)) {
				fixIt();
				return;
			}
		}
	drop.row--;
}

void fixIt() {
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (drop.mat[i][j]) map[drop.row - i][drop.column + j] = drop.id + 1;
	isDropping = 0;
	while (checkForElimination());
}

void moveIt(int id) {
	int direction[5][2] = { {-1},{1} };
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (drop.mat[i][j] != 0 && (map[drop.row - i][drop.column + j + direction[id][0]] || drop.column + j + direction[id][0] < 0 || drop.column + j + direction[id][0] > 15)) return;
	drop.column += direction[id][0];
}

bool checkForElimination() {
	int i, j;
	bool didSomething = 0;
	for (i = 0; i < 24; i++) {
		bool elimAble = 1;
		for (j = 0; j < 16; j++) {
			if (map[i][j] == 0) {
				elimAble = 0;
				break;
			}
		}
		if (elimAble) {
			didSomething = 1;
			eliminate(i);
			//gravity();
		}
	}
	return didSomething;
}

void eliminate(int index) {
	int i, j;
	for (i = index; i < 24; i++) {
		bool isBlank = 1;
		for (j = 0; j < 16; j++) {
			if (map[i + 1][j] != 0) isBlank = 0;
			map[i][j] = map[i + 1][j];
		}
		if (isBlank) break;
	}
}


// Is this needed?
void gravity() {
	int i, j;
	for (j = 1; j < 24; j++) {
		for (i = 0; i < 16; i++) {
			if (map[i][j] == 0) continue;
			if (((i == 0) || (i - 1 >= 0 && map[i - 1][j - 1] == 0)) && (map[i][j - 1] == 0) && ((i == 23 || (i + 1 < 24 && map[i + 1][j - 1] == 0)))) {
				map[i][j - 1] = map[i][j];
				map[i][j] = 0;
			}
		}
	}
}

// TODO: The feasibility of rotation.
void rotateIt() {
	if (!isDropping || drop.id == 4) return;
	int tmp[4][4] = { 0 };
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) {
			tmp[i][j] = drop.mat[j][3 - i];
		}
	/*
	1 1 0 0  0 0 0 0
	1 0 0 0  0 0 0 0
	1 0 0 0  1 0 0 0
	0 0 0 0  1 1 1 0
	*/
	memcpy(drop.mat, tmp, sizeof(tmp));
}

void dropToBottom() {
	int i, j, k;
	for (k = drop.row - 1; k >= 0; k--) {
		bool flag = 0;
		for (i = 3; i >= 0; i--) {
			for (j = 0; j < 4; j++) {
				if (!drop.mat[i][j]) continue;
				if (k - i < 0) {
					drop.row = k + 1;
					fixIt();
					return;
				}
				if (map[k - i][drop.column + j]) {
					drop.row = k + 1;
					fixIt();
					return;
				}
			}
		}
	}
}