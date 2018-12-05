#include <stdio.h>
#include <curses.h>
#include <stdlib.h>
#include <time.h>

void re(char *filename, char* nickname);

int main(void)
{
	char filename[100];

	printf("filename : ");
	scanf("%s", filename);

	re(filename, "hwangsukyoung");
	return 0;
}

void re(char *filename, char *nickname)
{
	time_t current_time;
	int ch;
	int row = 0, col = 0, a, b, d, temp, i;
	int MAX = 31;
	int check;
	char buf[68];
	FILE *fp = fopen(filename, "r+");
	if (fp == NULL)
	{
		exit(1);
	}
	initscr();
	clear();
	noecho();
	keypad(stdscr, TRUE);
	for (row = 0; row<30; row++)
	{
		for (col = 0; col<70; col++)
		{
			if (row == 0 || row ==29 || row == 2)
			{
				move(row, col);
				addch('*');
			}
		}
	}
	for (a = 1; a <= 27; a++)
	{
		for (b = 1; b <= 69; b++)
		{
			
			if (a == 1)
			{
				ch = getc(fp);
				mvaddch(a, b, ch);
				
			}
			else
			{
				ch = getc(fp);
				mvaddch(a + 1, b, ch);
	
			}
		}
	}
	refresh();
	fgets(buf, 100, fp);
	mvaddstr(30, 1, buf);
	for (a = 0; a < 70; a++)
		mvaddch(31, a, '*');
	
	fgets(buf, 100, fp);
	mvaddstr(30, 45, "writer : ");
	mvaddstr(30, 54, buf);
	//mvaddstr(32, 1, "1.add comment 2.quit : ");
	refresh();
	a = 0;
	while (1)
	{
		fgets(buf, 100, fp);
		if (feof(fp))
			break;
		mvaddstr(a, 72, buf);
		fgets(buf, 100, fp);
		mvaddstr(a, 95, buf);
		fgets(buf, 100, fp);
		mvaddstr(a+1, 72, buf);
		mvaddstr(a + 2, 72, "----------------------------------------------------------------------");
		a = a + 3;
		
	}
	mvaddstr(32, 1, "1.add comment 2.quit : ");
	temp = a;
	while(1)
	{
		ch = getch();
		if (ch == '1') // comment
		{
			mvaddstr(32, 1, "                       ");
			
					
			for (d = 0; d <= 68; d++)
			{
				mvaddch(34, d, '*');
				mvaddch(36, d, '*');
			}
			//mvaddch(30, 130, '*');
			mvaddstr(37, 64, "Enter");
			mvaddstr(33, 1, "<comment>");
			mvaddch(35, 0, '*');
			mvaddstr(37, 0, "cancel : ESC");
			move(35, 1);
			refresh();
			row = 35;  col = 1;
			while (1)
			{
				ch = getch();
				if (ch == 27) // esc
				{
					for (a = 33; a <= 37; a++)
					{
						for (b = 0; b <= 74; b++)
							mvaddch(a, b, ' ');
						
					}refresh();
					mvaddstr(32, 1, "1.add comment 2.quit : ");
					break;
					refresh();
				}
				else if (ch == 10) // enter
				{
					fseek(fp, 0, SEEK_END);
					//fprintf(fp, "\n");
					fprintf(fp, nickname);
					fprintf(fp, "\n");
					time(&current_time);
					fprintf(fp, ctime(&current_time));

					for (i = 1; i <= 68; i++)
					{
						ch = mvinch(35, i);
						fprintf(fp, "%c", ch);
					}
					fprintf(fp, "\n");
					fclose(fp);
					clear();
					re(filename, nickname);
					clear();
					endwin();
					return;
				}
				else if (ch == KEY_BACKSPACE)
				{
					if (col == 1)
						continue;
					else
						col--;
					move(row, col);
					delch();
					refresh();
				}
				else if (ch == KEY_UP)
				{
				}
				else if (ch == KEY_DOWN)
				{
				}
				else if (ch == KEY_LEFT)
				{
					if (col != 1)
						col--;
					move(row, col);
					refresh();
				}
				else if (ch == KEY_RIGHT)
				{
					if (col != 68)
						col++;
					move(row, col);
					refresh();
				}
				else
				{
					mvaddch(row, col, ch);
					col++;
					refresh();
				}
			}


			//fclose(fp);
		}
		else if (ch == '2') // quit
		{
			clear();
			endwin();
			fclose(fp);
			return;
		}
		else if (ch == KEY_UP)
		{
			getyx(stdscr, row, col);
			if (row != 0)
				row--;
			move(row, col);
			refresh();
		}
		else if (ch == KEY_DOWN)
		{
			getyx(stdscr, row, col);
				row++;
			move(row, col);
			refresh();
		}
		else if (ch == KEY_LEFT)
		{
			getyx(stdscr, row, col);
			if (col != 0)
				col--;
			move(row, col);
			refresh();
		}
		else if (ch == KEY_RIGHT)
		{
			getyx(stdscr, row, col);
			col++;
			move(row, col);
			refresh();
		}
	}
	
	
	endwin();
	fclose(fp);
}