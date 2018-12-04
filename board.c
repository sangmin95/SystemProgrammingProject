#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ncurses.h>

#define LOCAL_ESCAPE_KEY 27 //enter key
#define TITLE_LEN 20
#define NAME_LEN 10
#define CONTEXT_LINE 27
#define CONTEXT_LEN 1000
#define DATE_LEN 11
#define WINDOW_WIDTH 50
#define WINDOW_HEIGHT 30
#define POST_NUM 1

void draw_boundary(int origin_x, int origin_y, int width, int height);
void load_posts(int n);
void print_posts();

typedef struct _POST
{
    char title[TITLE_LEN]; //post title
    char name[NAME_LEN];   //writer
    char context[CONTEXT_LINE][CONTEXT_LEN];
    char date[DATE_LEN];
} POST;


int win_w, win_h; //size of windows
WINDOW *win;
POST *posts;

int main()
{
    win = initscr();
    clear();
    //win = newwin(WINDOW_HEIGHT,WINDOW_WIDTH,origin_y,origin_x);
    //getmaxyx(win,win_h,win_w);

    //draw_boundary(3, 3, 30, 20);
    load_posts(POST_NUM);
    print_posts();
    refresh();
    getch();
    if (getch() == LOCAL_ESCAPE_KEY)
    {
        endwin();
    }
    printf("\n");
}

void print_posts()
{
    int i, x, y;
    for (i = 0; i < POST_NUM; i++)
    {
        getyx(win, y, x);
        move(y++, x);
        printw("%d)\t%s",i+1,posts[i].title);
    }
}

void load_posts(int n) //n : the number of posts
{
    int i, j;
    posts = (POST *)malloc(sizeof(POST) * n);
    FILE *fp = fopen("post/post.txt", "r");

    for (i = 0; i < n; i++)
    {
        fscanf(fp, "%[^\n]s", &posts[i].title);
        for (j = 0; j < CONTEXT_LINE; j++)  fscanf(fp, "%[^\n]s", &posts[i].context[j]);
        fscanf(fp, "%[^\n]s", posts[i].date);
        fscanf(fp, "%[^\n]s", posts[i].name);
    }
    fclose(fp);
}

void draw_boundary(int origin_y, int origin_x, int width, int height)
{

    int i, j;

    move(origin_y, origin_x);
    for (i = origin_x; i < WINDOW_WIDTH; i++)
    {
        addstr("-");
    }
    move(origin_y + height, origin_x);
    for (i = origin_x; i < WINDOW_WIDTH; i++)
    {
        addstr("-");
    }
}