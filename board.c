#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#define ENTER 10
#define LOCAL_ESCAPE_KEY 27 //enter key
#define TITLE_LEN 20
#define NAME_LEN 20
#define CONTEXT_LINE 27
#define DATE_LEN 30
#define WINDOW_WIDTH 70
#define WINDOW_HEIGHT 30
#define UP 8
#define DOWN 2



void draw_boundary(int origin_x, int origin_y, int width, int height);
void load_posts(int n);
void print_posts();
int keyboard_stream();
void move_cursor(int direction);
void make_cursor(int y,int x);


typedef struct _POST
{
    char title[TITLE_LEN]; //post title
    char context[CONTEXT_LINE][WINDOW_WIDTH];
    char date[DATE_LEN];
    char name[NAME_LEN];   //writer
} POST;

int print_start_y=3,post_num; //size of windows
WINDOW *win;
POST *posts;

int main()
{
   int flag;
	char c, filename[20];

	load_posts(post_num);

	win = initscr();
	clear();
	draw_boundary(0, 0, WINDOW_HEIGHT, WINDOW_WIDTH);

	print_posts();
	refresh();
	make_cursor(print_start_y, 1); // >> : cursor

	flag = keyboard_stream(); //방향키
	
	if(flag == 0){
		printf("ESC키가 입력되어 프로그램을 종료합니다.\n");
		endwin();
	}
	if(flag == 1){
		itofilename(filename,chosen_post);
		read_post(filename, session_nick);
	}
	if(flag == 2){
		//go to write mode

	}
}


void itofilename(char* filename,int i)
{
	sprintf(filename, "%d", i);
	strcat(filename, ".txt");
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
		if (y > print_start_y - 1 + 5 * (post_num - 1))
			return;

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
	}

	return flag;
}

void print_posts()
{
	int i, x, y;
	move(0, 0);
	for (i = 1; i <= post_num; i++)
	{
		getyx(win, y, x);
		mvprintw(y += 3, 5, "%d)", i); //the number of lines btw posts = 3
		mvprintw(y, x += 4, "[title] : %s\n", posts[i].title);
		mvprintw(y, WINDOW_WIDTH - 24, "<writer> : %s\n", posts[i].name);
		mvprintw(y + 1, WINDOW_WIDTH - 24, "%s\n", posts[i].date); // the length of date is 24byte
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