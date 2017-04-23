#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conio.h"
#define MAX 10

// modes
#define END_MODE 0
#define FLAG_MODE 1
#define CHECK_MODE 2
#define NEW_MODE 3
#define RESET_MODE 4
// define TRUE AND FALSE
#define FALSE 0
#define TRUE 1
// define direction
#define UP '8'
#define DOWN '2'
#define LEFT '4'
#define RIGHT '6'
// background color
#define BG_KNRM  "\x1B[0m"
#define BG_RED  "\x1B[41m"
#define BG_GRN  "\x1B[42m"
#define BG_YEL  "\x1B[43m"
#define BG_BLU  "\x1B[44m"
#define BG_MAG  "\x1B[45m"
#define BG_CYN  "\x1B[46m"
#define BG_WHT  "\x1B[47m"
// text color
#define TXT_RED  "\x1B[31m"
#define TXT_GRN  "\x1B[32m"
#define TXT_YEL  "\x1B[33m"
#define TXT_BLU  "\x1B[34m"
#define TXT_MAG  "\x1B[35m"
#define TXT_CYN  "\x1B[36m"
#define TXT_WHT  "\x1B[37m"


// global variables
// game table
unsigned char table_array[MAX][MAX];
// location of cursor
int x = 0, y = 0;
// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode = 0;

int is_bounded(int row, int column) {
	if ((row >= 0 && row < MAX) && (column >= 0 && column < MAX))
		return 1;
	else
		return 0;
}

/*This is a recursive function which uncovers blank cells while they are adjacent*/
int uncover_blank_cell(int row, int col) {
	int mine_value, rows[8], columns[8], i;

	if (table_array[row][col] != 0)
		return 0; // error

	table_array[row][col] += 10; // uncover current cell

								 // Get position of adjacent cells of current cell
	rows[0] = row - 1; // 0 = upperRight 
	columns[0] = col + 1;
	rows[1] = row; // 1 = right
	columns[1] = col + 1;
	rows[2] = row + 1; // 2 = lowerRight
	columns[2] = col + 1;
	rows[3] = row - 1; // 3 = up
	columns[3] = col;
	rows[4] = row + 1; // 4 = down
	columns[4] = col;
	rows[5] = row - 1; // 5 = upperLeft
	columns[5] = col - 1;
	rows[6] = row; // 6 = left
	columns[6] = col - 1;
	rows[7] = row + 1; // 7 = lowerLeft
	columns[7] = col - 1;

	for (i = 0; i < 8; i++) {
		mine_value = table_array[rows[i]][columns[i]];

		if (is_bounded(rows[i], columns[i])) {	// to prevent negative index and out of bounds
			if (mine_value > 0 && mine_value <= 8)
				table_array[rows[i]][columns[i]] += 10;										// it is a cell with 1-8 number so we need to uncover
			else if (mine_value == 0)
				uncover_blank_cell(rows[i], columns[i]);
		}

	}

	return 1; // success!
}

void view_table() {
	// clear screen
	system("clear");

	int i, j, mine_value;
	int is_uncoverd, is_clean_area, is_one, is_gt_than_one;

	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			if (x == j && y == i) {
				if (game_mode == FLAG_MODE) {
					printf("|%sF%s", BG_MAG, BG_KNRM);
					continue;
				}
				else if (game_mode == CHECK_MODE) {
					printf("|%sC%s", BG_MAG, BG_KNRM);
					continue;
				}

			}
			mine_value = table_array[i][j];

			is_uncoverd = (mine_value >= 0 && mine_value <= 8) || mine_value == 0 || mine_value == 99 ? TRUE : FALSE;
			is_clean_area = mine_value == 10 ? TRUE : FALSE;
			is_one = mine_value == 11 ? TRUE : FALSE;
			is_gt_than_one = mine_value > 11 && mine_value <= 18 ? TRUE : FALSE;

			if (is_uncoverd)
				printf("|X");
			else if (is_clean_area) // clean area
				printf("|%s%d%s", TXT_CYN, mine_value - 10, BG_KNRM);
			else if (is_one) // the number of near mine is 1
				printf("|%s%d%s", TXT_YEL, mine_value - 10, BG_KNRM);
			else if (is_gt_than_one) // the number of near mine is greater than 1
				printf("|%s%d%s", TXT_RED, mine_value - 10, BG_KNRM);
			else if ((mine_value >= 20 && mine_value <= 28) || mine_value == 100)
				printf("|%sF%s", TXT_GRN, BG_KNRM);
			else
				printf("ERROR"); // test purposes
		}
		printf("|\n");
	}

	printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n", TXT_CYN, BG_KNRM, TXT_GRN, BG_KNRM);
	if (game_mode == END_MODE) {
		printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
	}
	else if (game_mode == FLAG_MODE) {
		printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
	}
	else if (game_mode == CHECK_MODE) {
		printf("Enter (select to check cell), q (Exit selection): ");
	}


}


int main(int argc, char *argv[]) {

	char ch;
	int nMines; // the number of the remaining mines
	int i, j, newRow, newCol, mine_value, rows[8], columns[8];
	int mode_flag;

	//new_game:
	while (TRUE) {
		mode_flag = NEW_MODE;
		// the number of mines
		nMines = 10;
		if (argc == 2) {
			nMines = atoi(argv[1]);
		}
		srand(time(NULL));						// random seed
												// setting cursor
		x = 0;
		y = 0;
		// set all cells to 0
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				table_array[i][j] = 0;

		for (i = 0; i < nMines; i++) {
			/* initialize random seed: */

			newRow = rand() % 10;					// it generates a integer in the range 0 to 9
			newCol = rand() % 10;

			// put mines
			if (table_array[newRow][newCol] != 99) {
				table_array[newRow][newCol] = 99;

				// Get position of adjacent cells of current cell
				rows[0] = newRow - 1;
				columns[0] = newCol + 1;
				rows[1] = newRow;
				columns[1] = newCol + 1;
				rows[2] = newRow + 1;
				columns[2] = newCol + 1;
				rows[3] = newRow - 1;
				columns[3] = newCol;
				rows[4] = newRow + 1;
				columns[4] = newCol;
				rows[5] = newRow - 1;
				columns[5] = newCol - 1;
				rows[6] = newRow;
				columns[6] = newCol - 1;
				rows[7] = newRow + 1;
				columns[7] = newCol - 1;

				for (j = 0; j < 8; j++) {
					mine_value = table_array[rows[j]][columns[j]];
					if (is_bounded(rows[j], columns[j])) {	// to prevent negative index and out of bounds
						if (mine_value != 99)																// to prevent remove mines
							table_array[rows[j]][columns[j]] += 1;									// sums 1 to each adjacent cell
					}
				}

			}
			else {							// to make sure that there are the properly number of mines in table
				i--;
				continue;
			}
		}


		//
		view_table();
		while (nMines != 0 && mode_flag != RESET_MODE) {			// when nMines becomes 0 you will win the game

			if (mode_flag == END_MODE)
				break;

			else if (mode_flag == NEW_MODE)
				ch = getch();


			// cursor direction
			char direction;
			switch (ch) {

				// flag mode
			case 'f':
			case 'F':
				game_mode = FLAG_MODE;
				mode_flag = FLAG_MODE;
				do {
					view_table();
					direction = getch();
					// arrow direction
					if (direction == UP) {
						// up
						--y;
						y = (MAX + y) % MAX;
					}
					else if (direction == DOWN) {
						// down
						++y;
						y = y % MAX;
					}
					else if (direction == LEFT) {
						//left
						--x;
						x = (MAX + x) % MAX;
					}
					else if (direction == RIGHT) {
						//right
						++x;
						x = x % MAX;
					}
					else if (direction == 'c' || direction == 'C') {
						game_mode = CHECK_MODE;
						mode_flag = CHECK_MODE;
						break;
					}
					else if (direction == '\n') {
						mine_value = table_array[y][x];

						if (mine_value == 99) {				// mine case
							table_array[y][x] += 1;
							nMines -= 1;				// mine found
						}
						else if (mine_value >= 0 && mine_value <= 8) {	// number of mines case (the next cell is a mine)
							table_array[y][x] += 20;
						}
						else if (mine_value >= 20 && mine_value <= 28) {
							table_array[y][x] -= 20;
						}

						if (nMines == 0)
							break;
					}
				} while (direction != 'q' && direction != 'Q');

				if (mode_flag == CHECK_MODE) {
					ch = 'c';
					break;
				}

				mode_flag = END_MODE;
				game_mode = END_MODE;

				break;

				// check cell
			case 'c':
			case 'C':
				game_mode = CHECK_MODE;
				mode_flag = CHECK_MODE;
				do {
					view_table();
					direction = getch();

					// arrow direction
					if (direction == UP) {
						// up
						--y;
						y = (MAX + y) % MAX;
					}
					else if (direction == DOWN) {
						// down
						++y;
						y = y % MAX;
					}
					else if (direction == LEFT) {
						//left
						--x;
						x = (MAX + x) % MAX;
					}
					else if (direction == RIGHT) {
						//right
						++x;
						x = x % MAX;
					}
					else if (direction == 'f' || direction == 'F') {
						mode_flag = FLAG_MODE;
						game_mode = FLAG_MODE;
						break;
					}

					else if (direction == '\n') {
						mine_value = table_array[y][x];
						if (mine_value == 0)						// blank case
							uncover_blank_cell(y, x);
						else if (mine_value == 99) {				// mine case
							mode_flag = END_MODE;
							break;
						}
						else if (mine_value > 0 && mine_value <= 8)	// number case (the next cell is a mine)
							table_array[y][x] += 10;

						//	break;
					}
				} while (direction != 'q' && direction != 'Q');

				if (mode_flag == FLAG_MODE) {
					ch = 'f';
					break;
				}

				mode_flag = END_MODE;
				game_mode = END_MODE;

				break;

				// jump to a new game
			case 'n':
			case 'N':
				mode_flag = RESET_MODE;
				break;

				// exit
			case 'q':
			case 'Q':
				mode_flag = END_MODE;
				break;

			default:
				break;
			}
		}
		if (mode_flag == END_MODE) {
			game_mode = END_MODE;
			view_table();
			printf("\nGAME OVER\n");

			if (nMines == 0)
				printf("you won!!!!\n");

			else
				printf("BOOM! you LOOSE!\n");

			do {
				printf("Are you sure to exit? (y or n)? ");
				ch = getch();
				putchar('\n');
				if (ch == 'y' || ch == 'Y') {
					break;
				}
				else if (ch == 'n' || ch == 'N') {
					mode_flag = NEW_MODE;
					break;
				}
				printf("Please answer y or n\n");
			} while (1);
			if (mode_flag == NEW_MODE)
				continue;

			else
				printf("See you next time!\n");

			fflush(stdin);


			return 0;
		}
	}
}

