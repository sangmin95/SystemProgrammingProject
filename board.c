#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#define LOCAL_ESCAPE_KEY 27 //enter key
#define TITLE_LEN 20
#define NAME_LEN 20
#define CONTEXT_LINE 27
#define DATE_LEN 30
#define WINDOW_WIDTH 70
#define WINDOW_HEIGHT 30
#define POST_NUM 3

void draw_boundary(int origin_x, int origin_y, int width, int height);
void load_posts(int n);
void print_posts();

typedef struct _POST
{
    char title[TITLE_LEN]; //post title
    char context[CONTEXT_LINE][WINDOW_WIDTH];
    char date[DATE_LEN];
    char name[NAME_LEN];   //writer
} POST;

int win_w, win_h; //size of windows
WINDOW *win;
POST *posts;

int main()
{
    //win = newwin(WINDOW_HEIGHT,WINDOW_WIDTH,origin_y,origin_x);
    //getmaxyx(win,win_h,win_w);

    //draw_boundary(3, 3, 30, 20);
    load_posts(POST_NUM);
    win = initscr();
    clear();
    draw_boundary(0,0,30,70);
    print_posts();
    refresh();
    getch();
    if (getch() == LOCAL_ESCAPE_KEY)
    {
        endwin();
    }
    printf("\n");
    endwin();
}

void print_posts()
{
    int i, x, y;
    move(3,0);
    for (i = 1; i <= POST_NUM; i++)
    {
        getyx(win, y, x);
        mvprintw(y+=3,0,"%d)", i);
        mvprintw(y,x+=4,"[title] : %s\n",posts[i].title);
        mvprintw(y,WINDOW_WIDTH-24,"<writer> : %s\n",posts[i].name);
        mvprintw(y+1,WINDOW_WIDTH-24,"%s\n",posts[i].date); //date길이는 항상 24
    }
}

void load_posts(int n) //n : the number of posts
{
    int i, j, k;
    posts = (POST *)malloc(sizeof(POST) * (n+1));
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
        fgets(posts[i].title,WINDOW_WIDTH,fp);
        for(j=0;j<WINDOW_WIDTH;j++)    if(posts[i].title[j]=='\n') posts[i].title[j] = '\0';
        
        //context 받아오기
        for (j = 1; j < CONTEXT_LINE; j++){
            fgets(posts[i].context[j],WINDOW_WIDTH,fp);
            posts[i].context[j][WINDOW_WIDTH-2] = '\0';
        }

        //date 받아오기
        fgets(posts[i].date,WINDOW_WIDTH,fp);
        for(j=0;j<DATE_LEN;j++)    if(posts[i].date[j]=='\n') posts[i].date[j] = '\0';

        //writer 받아오기
        fgets(posts[i].name,WINDOW_WIDTH,fp);
        for(j=0;j<NAME_LEN;j++)    if(posts[i].name[j]=='\n') posts[i].name[j] = '\0';
        fclose(fp);
    }
}

void draw_boundary(int origin_y, int origin_x, int width, int height)
{

    int i, j;

    move(origin_y, origin_x);
    for(origin_x=0; origin_x<30; origin_x++)
	{
		for(origin_y=0; origin_y<70; origin_y++)
		{
			if(origin_x == 0 || origin_x == 30-1)
			{
				move(origin_x, origin_y);
				addch('*');
			}
		}
	}
}
