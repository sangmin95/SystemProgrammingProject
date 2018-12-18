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

#define BUFSIZE 100
#define LOCAL_ESCAPE_KEY 27 //ESC key
#define TITLE_LEN 20
#define NAME_LEN 20
#define CONTEXT_LINE 27
#define DATE_LEN 30
#define WINDOW_WIDTH 70
#define WINDOW_HEIGHT 30
#define KEY_N 78
#define UP 8
#define DOWN 2
#define ENTER 10

int firstscreen(void);
void login(void);
void post(char *filename, char *nickname);
void read_post(char *filename, char *nickname);
void join(void);
void board(int );
int filecheck();
void process_login();
void draw_boundary(int origin_x, int origin_y, int width, int height);
void load_posts(int n);
void print_posts();
int keyboard_stream();
void move_cursor(int direction);
void make_cursor(int y, int x);
int tty_mode(int how);
void set_noecho();
void itofilename(char* filename,int i);

typedef struct _POST
{
	char title[TITLE_LEN]; //post title
	char context[CONTEXT_LINE][WINDOW_WIDTH];
	char date[DATE_LEN];
	char name[NAME_LEN]; //writer
} POST;

char session_id[BUFSIZE], session_nick[BUFSIZE], path[BUFSIZE];
int post_num, chosen_post = 1, print_start_y = 3; //size of board windows
int page = 1;
int total_page;
WINDOW *win;
POST *posts; //array of posts

int main()
{
	process_login(); //sign_in or sign_up
	post_num = filecheck();
	
	board(page);

	return 0;
}

void process_login()
//start login
{
	int check;
	FILE *fp;
	char buf[100];

	while (1)
	{
		check = firstscreen();
		if (check == 2)
		//Sign In is chosen
		{
			login();
			strcpy(path, "./logfile/");
			strcat(path, session_id);

			if ((fp = fopen(path, "r")) == NULL)
			{
				perror("fopen error");
				exit(2);
			}
			fscanf(fp, "%s", buf);
			fscanf(fp, "%s", session_nick);
			//printf("AA%sAA\n", session_nick);
			fclose(fp);
			return;
		}
		else if (check == 1)
		//sign_Up is chosen
		{
			join();
			continue;
		}
	}
	fclose(fp);
}

void post(char *filename, char *nickname)
{
	time_t current_time;
	FILE *fp;
	int ch, row = 0, col = 0, a, b, MAX = 30, check;
	fp = fopen(filename, "w");

	initscr();
	clear();
	noecho();
	//raw();
	//cbreak();
	//nonl();
	//intrflush(stdscr, TRUE);
	keypad(stdscr, TRUE);

	for (row = 0; row < 30; row++)
	{
		for (col = 0; col < 70; col++)
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
			a = row;
			b = col;
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
						for (b = 1; b < 69; b++)
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
					fclose(fp);
					board(page);
					
				}
				else if (check == '2')
				{
					clear();
					endwin();
					fclose(fp);
					remove(filename);
					post_num--;
					board(page);
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
				row = 1;
				col = 68;
			}
			else if (col == 1)
			{
				row--;
				col = 68;
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
				move(row, col);
				refresh();
			}
			else if (row == 28)
				continue;
			else
			{
				row++;
				col = 1;
				move(row, col);
				refresh();
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
			move(row, col);
			refresh();
		}
		else if (ch == KEY_DOWN)
		{
			if (row == 1)
				row = row + 2;
			else if (row == 28)
				continue;
			else
				row++;
			move(row, col);
			refresh();
		}
		else if (ch == KEY_LEFT)
		{
			if (col == 1)
				continue;
			col--;
			move(row, col);
			refresh();
		}
		else if (ch == KEY_RIGHT)
		{
			if (col == 68)
				continue;
			col++;
			move(row, col);
			refresh();
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

void read_post(char *filename, char *nickname)
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
	for (row = 0; row < 30; row++)
	{
		for (col = 0; col < 70; col++)
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
			row = 35;
			col = 1;
			while (1)
			{
				ch = getch();
				if (ch == 27) // esc
				{
					for (a = 33; a <= 37; a++)
					{
						for (b = 0; b <= 74; b++)
							mvaddch(a, b, ' ');
					}
					refresh();
					mvaddstr(32, 1, "1.add comment 2.quit : ");
					break;
					refresh();
				}
				else if (ch == ENTER) // enter
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
					read_post(filename, nickname);
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

	char id[BUFSIZE], pwd[BUFSIZE], pwdC[BUFSIZE], buf[BUFSIZE], c, cwd[BUFSIZE], path[BUFSIZE], fname[BUFSIZE];
	FILE *fd;
	DIR *dir;
	int i = 0, len, flag = 0;

	struct dirent *direntp;

	system("clear");
	printf("\n\n\t\t\t\t   LOG IN");
	printf("\n\t\t\t****************************");

	printf("\n\n\t\t\t ID : ");
	scanf("%s", id);

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

			//len = strlen(pwdC);

			if (strcmp(pwdC, pwd) == 0)
			{
				printf("\n\t\t\tLogin success\n"); //login ������
			}

			else
			{
				printf("\n\t\t\tLogin failed.");
			}
		}
	}

	if (flag == 0)
		printf("ID does not exist.\n");

	strcpy(session_id, id);
}

void join()
{

	struct stat st = {0};
	struct dirent *direntp;
	char cwd[BUFSIZE], path[BUFSIZE], ch;
	char id[BUFSIZE], pwd1[BUFSIZE], pwd2[BUFSIZE], name[BUFSIZE], c;
	int sign_file;
	int i, len;

	FILE *fd;
	DIR *dir;

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
			return;
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
		return;
	}

	printf("\n\t\t\t Nickname : ");
	scanf("%s", name);
	write(sign_file, name, strlen(name));

	strcat(path, "/");
	strcat(path, id);
	rename(id, path);
}

int filecheck()
{
	int n = 1;
	char filename[1000];
	while (1)
	{
		itofilename(filename,n);
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
	}
	
	return n;
}

void itofilename(char* filename,int i)
{
	sprintf(filename, "%d", i);
	strcat(filename, ".txt");
}

void board(int p)
{
	int flag;
	int n = 1;
	char buf[50];
	char c, filename[20];
	
	post_num = filecheck();

	if (post_num % 5 == 0)
	{
		total_page = post_num / 5;
	}
	else
	{
		total_page = post_num / 5;
		total_page++;
	}
	
	refresh();
	load_posts(post_num);


	win = initscr();
	clear();
	draw_boundary(0, 0, WINDOW_HEIGHT, WINDOW_WIDTH);
	mvaddch(28, 34, page + 48);
	mvaddch(28, 35, '/');
	mvaddch(28, 36, total_page + 48);
	
	print_posts(p);
	refresh();
	make_cursor(print_start_y, 1); // >> : cursor

	while (1)
	{
		flag = keyboard_stream(); //방향키

		if (flag == 0) {
			printf("ESC키가 입력되어 프로그램을 종료합니다.\n");
			endwin();
			break;
		}
		if (flag == 1) {
			itofilename(filename, chosen_post);
			read_post(filename, session_nick);
			board(page);
		}
		if (flag == 2) {
			//go to write mode
			post_num++;
			sprintf(buf, "%d", post_num);
			strcat(buf, ".txt");
			post(buf, session_nick);
		}
		if (flag == 3) // <
		{
			if (page > 1)
			{
				if (page != 1) { page--; }
				clear();
				endwin();
				board(page);
			}

		}
		if (flag == 4) // >
		{
			if (page < total_page)
			{
				if (page != total_page) { page++; }
				clear();
				endwin();
				board(page);
			}
		}
	}
	
}

void make_cursor(int y, int x)
{
	mvprintw(y, x, "%s", ">>");
	move(y, x--);
	refresh();
}

void move_cursor(int direction)
{

	int x, y;
	getyx(win, y, x);

	if (direction == UP)
	{
		if (y <= print_start_y)
			return;

		mvprintw(y, x, "   ");   //  >>을 지우고
		make_cursor(y -= 5, 1); //  4줄 위에 >> 그리기
		chosen_post--;
	}

	if (direction == DOWN)
	{
		if (page == total_page)
		{
			if (post_num % 5 == 0)
			{
				if (y > print_start_y - 1 + 5 * (5 - 1))
					return;
			}
			else
			{
				if (y > print_start_y - 1 + 5 * (post_num % 5 - 1))
					return;
			}
			
		}
		else {
			if (y > print_start_y - 1 + 5 * (5 - 1))
				return;
		}
		

		mvprintw(y, 1, "   ");   // >>을 지우고
		make_cursor(y += 5, 1); //  4줄 아래 >> 그리기
		chosen_post++;
	}
}

int keyboard_stream()
{
	//flag : ESC 0, ENTER 1, N 2, ERROR -1
	int c, flag = -1;

	chosen_post = 1; //cursor always located on first post
	//keypad(stdscr, TRUE);

	while (c = getch())
	{

		if (c == KEY_UP)			move_cursor(UP);
		if (c == KEY_DOWN)			move_cursor(DOWN);

		if (c == LOCAL_ESCAPE_KEY){ 
			//ESC
			flag = 0;	break;
		}

		if (c == ENTER){
			//read post
			flag = 1; break;
		}

		if (c == KEY_N){
			//write new post
			flag = 2;	break;
		}

		if (c == KEY_LEFT)
		{
			flag = 3; break;
		}
		if (c == KEY_RIGHT)
		{
			flag = 4; break;
		}

	}

	return flag;
}

void print_posts(int p)
{
	int i, x, y;
	move(0, 0);
	if (page < total_page)
	{
		for (i = 5 * (p - 1) + 1; i <= p * 5; i++)
		{
			getyx(win, y, x);
			mvprintw(y += 3, 5, "%d)", i); //the number of lines btw posts = 3
			mvprintw(y, x += 4, "[title] : %s\n", posts[i].title);
			mvprintw(y, WINDOW_WIDTH - 24, "<writer> : %s\n", posts[i].name);
			mvprintw(y + 1, WINDOW_WIDTH - 24, "%s\n", posts[i].date); // the length of date is 24byte
		}
	}
	else
	{
		if (post_num % 5 == 0)
		{
			for (i = 5 * (p - 1) + 1; i <= 5 * (p - 1) + 5; i++)
			{
				getyx(win, y, x);
				mvprintw(y += 3, 5, "%d)", i); //the number of lines btw posts = 3
				mvprintw(y, x += 4, "[title] : %s\n", posts[i].title);
				mvprintw(y, WINDOW_WIDTH - 24, "<writer> : %s\n", posts[i].name);
				mvprintw(y + 1, WINDOW_WIDTH - 24, "%s\n", posts[i].date); // the length of date is 24byte
			}
		}
		else
		{
			for (i = 5 * (p - 1) + 1; i <= 5 * (p - 1) + post_num % 5; i++)
			{
				getyx(win, y, x);
				mvprintw(y += 3, 5, "%d)", i); //the number of lines btw posts = 3
				mvprintw(y, x += 4, "[title] : %s\n", posts[i].title);
				mvprintw(y, WINDOW_WIDTH - 24, "<writer> : %s\n", posts[i].name);
				mvprintw(y + 1, WINDOW_WIDTH - 24, "%s\n", posts[i].date); // the length of date is 24byte
			}
		}
		
	}
}

void load_posts(int n) //n : the number of posts
{
	int i, j, k;
	posts = (POST *)malloc(sizeof(POST) * (n + 1));
	char **filename;
	filename = (char **)malloc(sizeof(char *) * n);
	for (i = 1; i <= n; i++)
	{
		filename[i] = (char *)malloc(sizeof(char) * 20);
		sprintf(filename[i], "%d", i);
		strcat(filename[i], ".txt");
	}

	for (i = 1; i <= n; i++)
	{
		//[1..n].txt 까지 열기
		FILE *fp = fopen(filename[i], "r");

		//title 받아오기
		fgets(posts[i].title, WINDOW_WIDTH, fp);
		for (j = 0; j < WINDOW_WIDTH; j++)
			if (posts[i].title[j] == '\n')
				posts[i].title[j] = '\0';

		//context 받아오기
		for (j = 1; j < CONTEXT_LINE; j++)
		{
			fgets(posts[i].context[j], WINDOW_WIDTH, fp);
			posts[i].context[j][WINDOW_WIDTH - 2] = '\0';
		}

		//date 받아오기
		fgets(posts[i].date, WINDOW_WIDTH, fp);
		for (j = 0; j < DATE_LEN; j++)
			if (posts[i].date[j] == '\n')
				posts[i].date[j] = '\0';

		//writer 받아오기
		fgets(posts[i].name, WINDOW_WIDTH, fp);
		for (j = 0; j < NAME_LEN; j++)
			if (posts[i].name[j] == '\n')
				posts[i].name[j] = '\0';
		fclose(fp);
	}
}

void draw_boundary(int origin_y, int origin_x, int width, int height)
{

	int i, j;

	move(origin_y, origin_x);
	for (origin_x = 0; origin_x < 30; origin_x++)
	{
		for (origin_y = 0; origin_y < 70; origin_y++)
		{
			if (origin_x == 0 || origin_x == 30 - 1)
			{
				move(origin_x, origin_y);
				addch('*');
			}
		}
	}
}

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