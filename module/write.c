#include <stdio.h>
#include <curses.h>
//#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

void post(char *filename, char *nickname);
//void display(FILE *fp);

int main(int ac, char *av[])
{
	post(av[1], av[2]);
	return 0;
}

void post(char *filename, char *nickname)
{
	time_t current_time;
	FILE *fp;
	int ch;
	int row = 0, col = 0, a, b;
	int MAX = 30;
	int check;
	fp = fopen(filename, "w");
	
	initscr();
	clear();
	noecho();
	//raw();
//	cbreak();
	//nonl();
	//intrflush(stdscr, TRUE);
	keypad(stdscr, TRUE);

	for(row=0; row<30; row++)
	{
		for(col=0; col<70; col++)
		{
			if(row == 0 || row == MAX-1 || row == 2)
			{
				move(row, col);
				addch('*');
			}
		}
	}
	
	move(1, 1);
	refresh();
	row = 1;
	col = 1;
	keypad(stdscr, TRUE);
	while(1)
	{
		ch = getch();

		if(ch == 27)
		{
			a=row; b=col;
			move(30, 0);
			addstr("1.post 2.quit 3.cancel : ");
			while(1)
			{
				check = getch();
				if(check == '1')
				{
					for(a=1; a<=28; a++)
					{
						if(a == 2)
							continue;
						for(b=1; b<69; b++)
						{
							ch = mvinch(a, b);
							fprintf(fp, "%c", ch);
						}
					
						fprintf(fp, "\n");
						
					}
					time(&current_time);
					fprintf(fp, ctime(&current_time));
					fprintf(fp, nickname);
					fprintf(fp, "\n");
					clear();
					endwin();
					return;
				}
				else if(check == '2')
				{
					clear();
					endwin();
					fclose(fp);
					remove(filename);
					return;
				}
				else if(check == '3')
				{
					move(30, 0);
					clrtoeol();
					move(a, b);
					refresh();	
					break;
				}
			}
		}
		else if(ch == KEY_BACKSPACE)
		{
			if(row == 1 && col == 1)
			{
				continue;
			}
			else if(row ==3 && col == 1)
			{
				row = 1; col = 68;
			}
			else if(col == 1)
			{
				row--; col=68;
			}
			else
			{
				col--;
			}
			move(row, col);
			delch();
			refresh();
		}
		else if(ch == 10)
		{
			if(row == 1)
			{
				row = row+2;
				col = 1;
				move(row, col); refresh();
			}
			else if(row == 28)
				continue;
			else
			{
				row++;
				col=1;
				move(row, col); refresh();
			}
		}
		else if(ch == KEY_UP)
		{
			if(row == 1)
				continue;
			if(row == 3)
				row = row - 2;
			else
				row--;
			move(row, col); refresh();

		}
		else if(ch == KEY_DOWN)
		{
			if(row == 1)
				row = row + 2;
			else if(row == 28)
				continue;
			else
				row++;
			move(row, col); refresh();
		}
		else if(ch == KEY_LEFT)
		{
			if(col == 1)
				continue;
			col--;
			move(row, col); refresh();
		}
		else if(ch == KEY_RIGHT)
		{
			if(col == 68)
				continue;
			col++;
			move(row, col); refresh();
		}
		else
		{
			//if(row == 28 && col == 68)
			//	continue;
			mvaddch(row, col, ch);
			if(row == 1 && col == 68)
			{
				row = row + 2;
				col = 1;
				move(row, col);
			}
			else if(row == 28 && col == 68)
			{
				continue;
			}
			else if(col == 68)
			{
				row++;
				col = 1;
				move(row, col);
			}
			else
			{
				col++;
				move(row, col);
			}
			refresh();
		}
	}

	endwin();
	fclose(fp);
}
