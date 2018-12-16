#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <termios.h>
#include <curses.h>
#include <time.h>

int firstscreen(void);
void login(void);
void post(char *filename, char *nickname);
void re(char *filename, char *nickname);
void join(void);
void board(void);
int filecheck();
int tty_mode(int how)
{
	static struct termios original_mode;

	if (how == 0)
		tcgetattr(0, &original_mode);

	else
		tcsetattr(0, TCSANOW, &original_mode);
}
void set_noecho()
{
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_cc[VMIN] = 1;
	ttystate.c_lflag &= ~ECHO;

	tcsetattr(0, TCSANOW, &ttystate);
}

int number = 0;
int check = 0;
char nickname[100];
char ID[50];
char path[100];

int main()
{
	FILE *fp;
	char buf[100];
	while (1)
	{
		check = firstscreen();
		if (check == 2)
		{
			login();
			strcpy(path, "./logfile/");
			strcat(path, ID);
			
			fp = fopen(path, "r");
			fscanf(fp, "%s", buf);
			fscanf(fp, "%s", nickname);
			printf("AA%sAA\n", nickname);
			break;
		}
		else if (check == 1)
		{
			join();
			continue;
		}
	}
	

	// login done.

	
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
	//cbreak();
	//nonl();
	//intrflush(stdscr, TRUE);
	keypad(stdscr, TRUE);

	for (row = 0; row<30; row++)
	{
		for (col = 0; col<70; col++)
		{
			if (row == 0 || row == MAX - 1 || row == 2)
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
	while (1)
	{
		ch = getch();

		if (ch == 27)
		{
			a = row; b = col;
			move(30, 0);
			addstr("1.post 2.quit 3.cancel : ");
			while (1)
			{
				check = getch();
				if (check == '1')
				{
					for (a = 1; a <= 28; a++)
					{
						if (a == 2)
							continue;
						for (b = 1; b<69; b++)
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
				else if (check == '2')
				{
					clear();
					endwin();
					fclose(fp);
					remove(filename);
					return;
				}
				else if (check == '3')
				{
					move(30, 0);
					clrtoeol();
					move(a, b);
					refresh();
					break;
				}
			}
		}
		else if (ch == KEY_BACKSPACE)
		{
			if (row == 1 && col == 1)
			{
				continue;
			}
			else if (row == 3 && col == 1)
			{
				row = 1; col = 68;
			}
			else if (col == 1)
			{
				row--; col = 68;
			}
			else
			{
				col--;
			}
			move(row, col);
			delch();
			refresh();
		}
		else if (ch == 10)
		{
			if (row == 1)
			{
				row = row + 2;
				col = 1;
				move(row, col); refresh();
			}
			else if (row == 28)
				continue;
			else
			{
				row++;
				col = 1;
				move(row, col); refresh();
			}
		}
		else if (ch == KEY_UP)
		{
			if (row == 1)
				continue;
			if (row == 3)
				row = row - 2;
			else
				row--;
			move(row, col); refresh();

		}
		else if (ch == KEY_DOWN)
		{
			if (row == 1)
				row = row + 2;
			else if (row == 28)
				continue;
			else
				row++;
			move(row, col); refresh();
		}
		else if (ch == KEY_LEFT)
		{
			if (col == 1)
				continue;
			col--;
			move(row, col); refresh();
		}
		else if (ch == KEY_RIGHT)
		{
			if (col == 68)
				continue;
			col++;
			move(row, col); refresh();
		}
		else
		{
			mvaddch(row, col, ch);
			if (row == 1 && col == 68)
			{
				row = row + 2;
				col = 1;
				move(row, col);
			}
			else if (row == 28 && col == 68)
			{
				continue;
			}
			else if (col == 68)
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
			if (row == 0 || row == 29 || row == 2)
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
		mvaddstr(a + 1, 72, buf);
		mvaddstr(a + 2, 72, "----------------------------------------------------------------------");
		a = a + 3;

	}
	mvaddstr(32, 1, "1.add comment 2.quit : ");
	temp = a;
	while (1)
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

int firstscreen(void)
{
	int row = 0, col = 0;
	int menu = 0;
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);


	while (1)
	{
		clear();
		for (row = 0; row < 50; row++)
		{
			for (col = 50; col < 100; col++)
			{
				if (row == 10 || row == 16 || row == 28)
				{
					move(row, col);
					addch('*');
				}
			}

			for (col = 50; col < 100; col++)
			{
				if ((row >= 17 && row <= 27) && (col == 50 || col == 99))
				{
					move(row, col);
					addch('*');
				}
			}
		}

		mvaddstr(2, 51, "*********   * *       *       *   *********  *  ");
		mvaddstr(3, 51, "       *    * *       *       *     *   *    * ");
		mvaddstr(4, 51, "      *     * *       *       *     *   *    *** ");
		mvaddstr(5, 51, "     *   **** *      * *      *   *********  *  ");
		mvaddstr(6, 51, "    *       * *     *   *     *   *          * ");
		mvaddstr(7, 51, "   *        * *    *     *    *   *");
		mvaddstr(8, 51, "  *         * *   *       *   *   ************");

		mvaddstr(20, 68, "1) Sign Up");
		mvaddstr(24, 68, "2) Sign In");
		mvaddstr(13, 68, "Choose Menu ");
		refresh();
		keypad(stdscr, TRUE);
		while (1)
		{
			menu = getch();

			if (menu == '1')
			{
				endwin();
				return 1;
			}

			else if (menu == '2')
			{
				endwin();
				return 2;
			}
		}
	}
	endwin();
}
void login(void)
{
	char id[50], pwd[50], pwdC[50], buf[80], c;
	char cwd[PATH_MAX], path[PATH_MAX], fname[PATH_MAX];
	FILE* fd;
	DIR *dir;
	int i = 0, len, flag = 0;

	struct dirent *direntp;

	system("clear");
	printf("\n\n\t\t\t\t   LOG IN");
	printf("\n\t\t\t****************************");

	printf("\n\n\t\t\t ID : ");
	scanf("%s", id);
	strcpy(ID, id);
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcpy(path, cwd);
		strcat(path, "/logfile");
	}

	if ((dir = opendir(path)) == NULL)
		fprintf(stderr, "Cannot open %s\n", path);

	for (i = 0; i < 50; i++)
		pwd[i] = '\0';

	while (direntp = readdir(dir))
	{
		if (strcmp(direntp->d_name, id) == 0)
		{
			flag = 1;

			strcpy(fname, path);
			strcat(fname, "/");
			strcat(fname, id);

			fd = fopen(fname, "r");
			fscanf(fd, "%s", pwdC);

			printf("\n\t\t\t Password : ");

			getchar();

			tty_mode(0);
			set_noecho();

			i = 0;

			while ((c = getchar()) != '\n')
			{
				if (c >= 32 && c != 127 && c != 8)
					printf("*");

				if ((c == 127 || c == 8))
				{
					if (i > 0)
					{
						printf("\b");
						printf(" ");
						printf("\b");
						i--;
					}

					else
						i = 0;
				}

				else
				{
					pwd[i] = c;
					i++;
				}
			}

			tty_mode(1);

			len = strlen(pwd);

			if (strncmp(pwdC, pwd, len) == 0)
			{
				printf("\n\t\t\tLogin success\n"); //login ¼º°ø½Ã
			}

			else
			{
				printf("\n\t\t\tLogin failed.");
			}
		}
	}

	if (flag == 0)
		printf("ID does not exist.\n");
}
void join(void)
{
	char cwd[PATH_MAX], path[PATH_MAX], ch;
	char id[50], *pwd1, pwd2[50], name[50], c;
	int sign_file;
	int i, len;

	FILE* fd;
	DIR *dir;

	struct stat st = { 0 };
	struct dirent *direntp;

	pwd1 = (char*)malloc(sizeof(char) * 50);

	for (i = 0; i < 50; i++)
	{
		pwd1[i] = '\0';
		pwd2[i] = '\0';
	}

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcpy(path, cwd);
		strcat(path, "/logfile");
	}

	if ((dir = opendir(path)) == NULL)
		fprintf(stderr, "Cannot open %s\n", path);

	system("clear");
	printf("\n\n\t\t\t\t   JOIN");
	printf("\n\t\t\t****************************");
	printf("\n\n\t\t\t ID : ");

	scanf("%s", id);

	while (direntp = readdir(dir))
	{
		if (strcmp(direntp->d_name, id) == 0)
		{
			printf("\t\t\tID already existing.");
			//display_menu();
			sleep(1);
			return ;
		}


		else if (strcmp(direntp->d_name, id) != 0)
			sign_file = open(id, O_WRONLY | O_CREAT, 0644);
	}

	printf("\n\t\t\t Password : ");

	getchar();
	i = 0;

	tty_mode(0);
	set_noecho();

	while ((c = getchar()) != '\n')
	{
		if (c >= 32 && c != 127 && c != 8)
			printf("*");

		if ((c == 127 || c == 8))
		{
			if (i > 0)
			{
				printf("\b");
				printf(" ");
				printf("\b");
				i--;
			}

			else
				i = 0;
		}

		else
		{
			pwd1[i] = c;
			i++;
		}
	}

	len = strlen(pwd1);

	printf("\n\t\t\t Password check : ");

	i = 0;

	while ((c = getchar()) != '\n')
	{
		if (c >= 32 && c != 127 && c != 8)
			printf("*");

		if ((c == 127 || c == 8))
		{
			if (i > 0)
			{
				printf("\b");
				printf(" ");
				printf("\b");
				i--;
			}

			else
				i = 0;
		}

		else
		{
			pwd2[i] = c;
			i++;
		}
	}

	tty_mode(1);

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		strcat(cwd, "/logfile");
		strcpy(path, cwd);

		if (stat(path, &st) == -1)
		{
			mkdir(path, 0777);
		}
	}

	if (strncmp(pwd1, pwd2, len) == 0)
	{
		printf("\n\t\t\t\t(correct password)\n");
		strcat(pwd1, "\n");
		write(sign_file, pwd1, strlen(pwd1));
	}

	else
	{
		printf("\t\t\t\t(password error)\n");
		//display_menu();
		return 0;
	}

	printf("\n\t\t\t Nickname : ");
	scanf("%s", name);
	write(sign_file, name, strlen(name));

	strcat(path, "/");
	strcat(path, id);
	rename(id, path);
}
void board(void)
{

}
int filecheck()
{
	int n = 1;
	char filename[20];
	while (1)
	{
		sprintf(filename, "%d", n);
		strcat(filename, ".txt");
		if (access(filename, F_OK) == 0)
		{
			n++;
			continue;
		}
		else
		{
			n--;
			break;
		}
		return n;
	}
	
	
}