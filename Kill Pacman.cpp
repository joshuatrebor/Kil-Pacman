#include<stdio.h>
#include<conio.h>
//#include<stdlib.h>
#include<windows.h>
#include<string.h>
#include<ctype.h>

#define WALL 0xDB
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define SPACE 32
#define PLAYER 0xE9
#define COIN 0xFA

//This is a structure of a cell. each movable position in the pacman map is a cell.
struct Cell{
	COORD coord;
	Cell *up;
	Cell *down;
	Cell *left;
	Cell *right;
	int num;
	bool visited;
	bool coined;
};
Cell* cells[500];
int cellCtr = 0;

struct{
	Cell* cell;
	int move[10];
	int moveTop;
	int moveBot;
	int dir;
	bool exist;
}playerA;

struct{
	Cell* cell;
	int move[10];
	int moveTop;
	int moveBot;
	int dir;
	bool exist;
}playerB;

struct{
	Cell* cell;
	char up = 'v';
	char down = '^';
	char left = '>';
	char right = '<';
	int dir;
}pacman;

struct Path{
	Cell* cells[400];
	int cellCount;
};

struct Visited{
	Cell* cells[500];
	int visitCount;
};

struct{
	COORD pacmanPos;
	COORD playerAPos;
	COORD playerBPos;
}mapInfo;


void gotoxy(int x, int y);
void drawMap();
void newMap();
void newCell(int x, int y, bool coined);
int mapToCell(int x, int y);
void loadMap(char file[], char pFile[]);
void drawCell(Cell *cell);
void startGame();
void initPlayers();
void delay(int k);
void goPacman(Cell* cell, Path path, Visited visited);
void refreshCells(Cell* exceptionCell);
void startPacman(Cell* pacCell);

int main(){
	printf("IF C pressed THEN create new map ELSE play game");
	if(getch() == 'c')
	newMap();
	loadMap("map.txt", "player.txt");
	int i;
	system("cls");
	system("color F0");
	initPlayers();
	drawMap();
	startGame();
}

Path tempPath;
bool tempPathEmpty;
void goPacman(Cell* cell, Path path, Visited visited){
	if(cell == pacman.cell || (path.cellCount > tempPath.cellCount && !tempPathEmpty))	//if search came back to pacman cell or the current search distance is larger than the current best, return
		return;
		
	int i;
	for(i = 0; i < visited.visitCount; i++){
		if(cell == visited.cells[i])
			return;
	}
	visited.cells[visited.visitCount++] = cell;
	//if(cell->visited)
	//	return;
	//cell->visited = true;
	path.cells[path.cellCount++] = cell;
	if(cell->coined){
		if(path.cellCount < tempPath.cellCount || tempPathEmpty){
			tempPath = path;
			tempPathEmpty = false;
		}
		return;
	}
	
	
	
	if(cell->up != NULL)
		goPacman(cell->up, path, visited);
	if(cell->down != NULL)
		goPacman(cell->down, path, visited);
	if(cell->left != NULL)
		goPacman(cell->left, path, visited);
	if(cell->right != NULL)
		goPacman(cell->right, path, visited);
	
}

void startPacman(Cell* pacCell){
	Path path;
	path.cellCount = 0;

	tempPathEmpty = true;
	Visited visited;
	visited.visitCount = 0;
		
	if(pacCell->up != NULL && pacCell->up != pacman.cell)
		goPacman(pacCell->up, path, visited);
	if(pacCell->down != NULL && pacCell->down != pacman.cell)
		goPacman(pacCell->down, path, visited);
	if(pacCell->left != NULL && pacCell->left != pacman.cell)
		goPacman(pacCell->left, path, visited);
	if(pacCell->right != NULL && pacCell->right != pacman.cell)
		goPacman(pacCell->right, path, visited);
}

void startGame(){
	char key;
	Cell *cellA, *cellB;
	bool pacmanOpen = true;
	while(true){
		delay(40);
		//Code to set current cells of players
		cellA = playerA.cell;
		cellB = playerB.cell;
		//Code to catch keyboard inputs
		if(kbhit() == 1){
			key = getch();
			if(key == UP || key == DOWN || key == LEFT || key == RIGHT && playerA.exist){
				playerA.move[playerA.moveTop++] = key;
				if(playerA.moveTop == 10) playerA.moveTop = 0;
			}
			else if(key == 'w' || key == 's' || key == 'a' || key == 'd' && playerB.exist){
				playerB.move[playerB.moveTop++] = key;
				if(playerB.moveTop == 10) playerB.moveTop = 0;
			}
		}
		//Code to catch turns per movement
		//For Player A
		if(playerA.moveBot != playerA.moveTop && playerA.exist){
			switch(playerA.move[playerA.moveBot]){
				case UP:
					if(cellA->up != NULL){
						playerA.dir = UP;
						playerA.moveBot++;
					}
					break;
				case DOWN:
					if(cellA->down != NULL){
						playerA.dir = DOWN;
						playerA.moveBot++;
					}
					break;
				case LEFT:
					if(cellA->left != NULL){
						playerA.dir = LEFT;
						playerA.moveBot++;
					}
					break;
				case RIGHT:
					if(cellA->right != NULL){
						playerA.dir = RIGHT;
						playerA.moveBot++;
					}
					break;
			}
			if(playerA.moveBot == 10)
				playerA.moveBot = 0;
		}
		//For player B
		if(playerB.moveBot != playerB.moveTop && playerB.exist){
			switch(playerB.move[playerB.moveBot]){
				case 'w':
					if(cellB->up != NULL){
						playerB.dir = 'w';
						playerB.moveBot++;
					}
					break;
				case 's':
					if(cellB->down != NULL){
						playerB.dir = 's';
						playerB.moveBot++;
					}
					break;
				case 'a':
					if(cellB->left != NULL){
						playerB.dir = 'a';
						playerB.moveBot++;
					}
					break;
				case 'd':
					if(cellB->right != NULL){
						playerB.dir = 'd';
						playerB.moveBot++;
					}
					break;
			}
			if(playerB.moveBot == 10)
				playerB.moveBot = 0;
		}
		//Code to move players
		//for player A
		if(playerA.exist){
			Cell* newCellA;
			switch(playerA.dir){
				case UP: newCellA = cellA->up; break;
				case DOWN: newCellA = cellA->down; break;
				case LEFT: newCellA = cellA->left; break;
				case RIGHT: newCellA = cellA->right; break;
			}
			cellA->coined = false;	//REMOVE THIS LINE
			if(newCellA != NULL){
				gotoxy(cellA->coord.X, cellA->coord.Y);
				printf("%c", (cellA->coined?COIN:' '));
				gotoxy(newCellA->coord.X, newCellA->coord.Y);
				printf("%c", PLAYER);
				playerA.cell = newCellA;
			}
			else if(playerA.moveBot != playerA.moveTop) playerA.moveBot++;
		}
		//for player B
		if(playerB.exist){
			Cell* newCellB;
			switch(playerB.dir){
				case 'w': newCellB = cellB->up; break;
				case 's': newCellB = cellB->down; break;
				case 'a': newCellB = cellB->left; break;
				case 'd': newCellB = cellB->right; break;
			}
			if(newCellB != NULL){
				gotoxy(cellB->coord.X, cellB->coord.Y);
				printf(" ");
				gotoxy(newCellB->coord.X, newCellB->coord.Y);
				printf("%c", PLAYER);
				playerB.cell = newCellB;
			}
			else if(playerB.moveBot != playerB.moveTop) playerB.moveBot++;
		}
		
		//Code for Pacman AI
		Cell* pacCell = pacman.cell;
		refreshCells(pacCell);
		
		startPacman(pacCell);
		
		Cell* nextCell = tempPath.cells[0];
		nextCell->coined = false;
		gotoxy(pacCell->coord.X, pacCell->coord.Y);
		printf(" ");
		gotoxy(nextCell->coord.X, nextCell->coord.Y);
		
		if(nextCell == pacCell->up)
			printf("%c", (pacmanOpen?pacman.up:'|'));
		else if(nextCell == pacCell->down)
			printf("%c", (pacmanOpen?pacman.down:'|'));
		else if(nextCell == pacCell->left)
			printf("%c", (pacmanOpen?pacman.left:'-'));
		else if(nextCell == pacCell->right)
			printf("%c", (pacmanOpen?pacman.right:'-'));
		pacman.cell = nextCell;
		
		pacmanOpen = !pacmanOpen;
	}
}

void refreshCells(Cell* exceptionCell){
	int i;
	for(i = 0; i < cellCtr; i++){
		cells[i]->visited = false;
		if(cells[i] == exceptionCell)
			cells[i]->visited = true;
	}
}

void initPlayers(){
	if(mapToCell(mapInfo.playerAPos.X,mapInfo.playerAPos.Y) != -1){
		playerA.cell = cells[mapToCell(mapInfo.playerAPos.X,mapInfo.playerAPos.Y)];
		playerA.exist = true;
	}
	else
		playerA.exist = false;
	playerA.moveTop = 0;
	playerA.moveBot = 0;
	playerA.dir = UP;
	
	if(mapToCell(mapInfo.playerBPos.X,mapInfo.playerBPos.Y) != -1){
		playerB.cell = cells[mapToCell(mapInfo.playerBPos.X,mapInfo.playerBPos.Y)];
		playerB.exist = true;
	}
	else
		playerB.exist = false;
	playerB.moveTop = 0;
	playerB.moveBot = 0;
	playerB.dir = 's';
	
	pacman.cell = cells[mapToCell(mapInfo.pacmanPos.X,mapInfo.pacmanPos.Y)];
}

void drawMap(){
	int x, y;
	for(y = 0; y < 29; y++){
		for(x = 0; x < 27; x++){
			printf("%c", WALL);
		}
		printf("\n");
	}
	
	int i;
	for(i = 0; i < cellCtr; i++)
		cells[i]->visited = false;
	drawCell(cells[0]);
	
	if(playerA.exist){
		Cell *cellA = playerA.cell;
		gotoxy(cellA->coord.X, cellA->coord.Y);
		printf("%c", PLAYER);
	}
	
	if(playerB.exist){
		Cell *cellB = playerB.cell;
		gotoxy(cellB->coord.X, cellB->coord.Y);
		printf("%c", PLAYER);
	}
	
	Cell *cellP = pacman.cell;
	gotoxy(cellP->coord.X, cellP->coord.Y);
	printf("%c", pacman.up);
	gotoxy(27, 0);
}

void drawCell(Cell* cell){
 	if(cell->visited) 
		return;
	
	cell->visited = true;
	delay(2);
	gotoxy(cell->coord.X, cell->coord.Y);
	printf("%c", (cell->coined?COIN:' '));
	if(cell->up != NULL)
		drawCell(cell->up);
	if(cell->right != NULL)	
		drawCell(cell->right);
	if(cell->down != NULL)
		drawCell(cell->down);
	if(cell->left != NULL)
		drawCell(cell->left);
}

void newMap(){
	system("cls");
	FILE *fp = fopen("map.txt", "w");
	bool coined = false;
	
	int x, y;
	for(y = 0; y < 29; y++){
		for(x = 0; x < 27; x++){
			printf("%c", WALL);
		}
		printf("\n");
	}
	
	x = 1;
	y = 1;
	gotoxy(27, 0);
	printf("X: %2d", x);
	gotoxy(27, 1);
	printf("Y: %2d", y);
	gotoxy(27,3);
	printf("Coined: %c", (coined?'Y':'N'));
	gotoxy(x, y);
	while(true){
		char key = getch();
		if(key == 'q') break;
		switch(key){
			case UP: y--; break;
			case DOWN: y++; break;
			case LEFT: x--; break;
			case RIGHT: x++; break;
			case SPACE:
				fprintf(fp, "%d\t%d\t%d\n", x, y, (coined?1:0));
				newCell(x, y, coined);
				printf("%c", (coined?COIN:' '));
				break;
			case 'p':
				if(mapToCell(x, y) != -1){
					mapInfo.pacmanPos.X = x;
					mapInfo.pacmanPos.Y = y;
					printf("P");
				}
				break;
			case 'a':
				if(mapToCell(x, y) != -1){
					mapInfo.playerAPos.X = x;
					mapInfo.playerAPos.Y = y;
					printf("A");
				}
				break;
			case 'b':
				if(mapToCell(x, y) != -1){
					mapInfo.playerBPos.X = x;
					mapInfo.playerBPos.Y = y;
					printf("B");
				}
				break;
			case 'c':
				coined = !coined;
				gotoxy(27,3);
				printf("Coined: %c", (coined?'Y':'N'));
				break;
		}
		if(x == 0) x = 1;
		if(x == 26) x = 25;
		if(y == 0) y = 1;
		if(y == 28) y = 27;
		gotoxy(27, 0);
		printf("X: %2d", x);
		gotoxy(27, 1);
		printf("Y: %2d", y);
		gotoxy(x, y);
		gotoxy(x, y);
	}
	FILE *fp2 = fopen("player.txt", "w");
	fprintf(fp2,"%d\t%d\n%d\t%d\n%d\t%d\n", mapInfo.pacmanPos.X, mapInfo.pacmanPos.Y, mapInfo.playerAPos.X, mapInfo.playerAPos.Y, mapInfo.playerBPos.X, mapInfo.playerBPos.Y);
	fclose(fp);
	fclose(fp2);
}

void newCell(int x, int y, bool coined){
	Cell* cell = (Cell*)malloc(sizeof(Cell));
	cell->coord.X = x;
	cell->coord.Y = y;
	cell->up = NULL;
	cell->down = NULL;
	cell->left = NULL;
	cell->right = NULL;
	cell->visited = false;
	cell->coined = coined;
	
	int i;
	for(i = 0; i < cellCtr; i++){
		if(cells[i]->coord.X == x && cells[i]->coord.Y == y)
			return;
	}
	
	cell->num = cellCtr;
	cells[cellCtr++] = cell;
}

int mapToCell(int x, int y){
	int i;
	for(i = 0; i < cellCtr; i++){
		if(cells[i]->coord.X == x && cells[i]->coord.Y == y)
			return cells[i]->num;
	}
	return -1;
}

void loadMap(char file[], char pFile[]){
	mapInfo.pacmanPos.X = -1;
	mapInfo.pacmanPos.Y = -1;
	mapInfo.playerAPos.X = -1;
	mapInfo.playerAPos.Y = -1;
	mapInfo.playerBPos.X = -1;
	mapInfo.playerBPos.Y = -1;
	
	FILE *fp = fopen(file, "r");
	int x, y, z;
	while(fscanf(fp, "%d\t%d\t%d", &x, &y, &z) != EOF)
		newCell(x, y, (z==1?true:false));
	fclose(fp);
	fp = fopen(pFile, "r");
	fscanf(fp, "%d\t%d", &mapInfo.pacmanPos.X, &mapInfo.pacmanPos.Y);
	fscanf(fp, "%d\t%d", &mapInfo.playerAPos.X, &mapInfo.playerAPos.Y);
	fscanf(fp, "%d\t%d", &mapInfo.playerBPos.X, &mapInfo.playerBPos.Y);
	fclose(fp);
	
	int i;
	for(i = 0; i < cellCtr; i++){
		Cell *cell = cells[i];
		x = cell->coord.X;
		y = cell->coord.Y;
		int cellNum;
		//CHECK CELL ON TOP
		cellNum = mapToCell(x, y-1);
		if(cellNum != -1)
			cell->up = cells[cellNum];
		//CHECK CELL ON BOTTOM
		cellNum = mapToCell(x, y+1);
		if(cellNum != -1)
			cell->down = cells[cellNum];
		//CHECK CELL ON LEFT
		cellNum = mapToCell(x-1, y);
		if(cellNum != -1)
			cell->left = cells[cellNum];
		//CHECK CELL ON RIGHT
		cellNum = mapToCell(x+1, y);
		if(cellNum != -1)
			cell->right = cells[cellNum];
	}
	
	//Wrap on edges
	//cells[cellCtr-2]->left = cells[cellCtr-1];
	//cells[cellCtr-1]->right = cells[cellCtr-2];
	
}

void gotoxy(int x, int y){
	COORD coord, d;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int k){
	int i, j;
	for(i = 0; i < 100*k; i++)
		for(j = 0; j < 10000; j++);
}
