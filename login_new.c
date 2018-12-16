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

int tty_mode(int how)
{
	static struct termios original_mode;

	if(how == 0) 
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

/*int display_menu(void)
{
	int menu = 0;

	while(1)
	{
		system("clear");
		printf("\n\n\t\t\t\t   게 시 판");
		printf("\n\t\t\t****************************");
		printf("\n\t\t\t\t    Menu\t\n");
		printf("\n\t\t\t****************************");
		printf("\n\t\t\t*\t             \t   *");
		printf("\n\t\t\t*\t1) 회 원 가 입\t   *");
		printf("\n\t\t\t*\t          \t   *");
		printf("\n\t\t\t*\t2) 로 그 인\t   *");
		printf("\n\t\t\t*\t             \t   *");
		printf("\n\t\t\t****************************");
		printf("\n\t\t\t\t\t Choose Menu : ");
		scanf("%d",&menu);

		if(menu < 1 || menu > 4)
		{
			display_menu;
		}

		else
		{
			return menu;
		}
	}

	return 0;
}*/

int display_sign_up(void){

	char cwd[PATH_MAX], path[PATH_MAX], ch;
	char id[50],  *pwd1,  pwd2[50], name[50], c;
	int sign_file;
	int i, len;
	
	FILE* fd;
	DIR *dir;

	struct stat st = {0};
	struct dirent *direntp;

		pwd1 = (char*)malloc(sizeof(char) * 50);
	
		for(i = 0; i < 50; i++)
		{
			pwd1[i] = '\0';
			pwd2[i] = '\0';
		}

		if(getcwd(cwd, sizeof(cwd)) != NULL)
		{
			strcpy(path, cwd);
			strcat(path, "/logfile");
		}

		if((dir = opendir(path)) == NULL)
			fprintf(stderr, "Cannot open %s\n", path);

		system("clear");		
		printf("\n\n\t\t\t\t 회 원 가 입");
		printf("\n\t\t\t****************************");
		printf("\n\n\t\t\t ID : ");

		scanf("%s", id);
		
		while(direntp = readdir(dir))
		{
			if(strcmp(direntp->d_name, id) == 0)
			{			
				printf("\t\t\tID already existing.");
				//display_menu();
				return 0;
			}
			

			else if (strcmp(direntp->d_name, id) != 0)
				sign_file = open(id, O_WRONLY | O_CREAT, 0644);
		}
	
		printf("\n\t\t\t Password : ");

		getchar();
		i = 0;

		tty_mode(0);
		set_noecho();

		while((c = getchar()) != '\n')
		{
			if(c >= 32 && c!= 127 && c != 8)
				printf("*");

			if((c == 127 || c == 8))
			{
				if(i > 0)
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

		while((c = getchar()) != '\n')
		{
			if(c >= 32 && c!= 127 && c != 8)
				printf("*");

			if((c == 127 || c == 8))
			{
				if(i > 0)
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

		if(getcwd(cwd, sizeof(cwd)) != NULL)
		{
			strcat(cwd, "/logfile");
			strcpy(path, cwd);

			if(stat(path, &st) == -1)
			{
				mkdir(path, 0777);
			}
		}

		if(strncmp(pwd1, pwd2, len) == 0)
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

		return 0;
}

int display_sign_in(void){

	char id[50],  pwd[50], pwdC[50], buf[80], c;	
	char cwd[PATH_MAX], path[PATH_MAX], fname[PATH_MAX];
	FILE* fd;
	DIR *dir;
	int i = 0, len, flag = 0;

	struct dirent *direntp;

		system("clear");		
		printf("\n\n\t\t\t\t   로 그 인");
		printf("\n\t\t\t****************************");

		printf("\n\n\t\t\t ID : ");
		scanf("%s", id);

		if(getcwd(cwd, sizeof(cwd)) != NULL)
		{
			strcpy(path, cwd);
			strcat(path, "/logfile");
		}

		if((dir = opendir(path)) == NULL)
			fprintf(stderr, "Cannot open %s\n", path);
		
		for(i = 0; i < 50; i++)
			pwd[i] = '\0';

		while(direntp = readdir(dir))
		{
			if(strcmp(direntp->d_name, id) == 0)
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

				while((c = getchar()) != '\n')
				{
					if(c >= 32 && c!= 127 && c != 8)
					printf("*");

					if((c == 127 || c == 8))
					{
						if(i > 0)
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
			
				if(strncmp(pwdC, pwd, len) == 0)
				{
					printf("\n\t\t\tLogin success\n"); //login 성공시
				}

				else
				{
					printf("\n\t\t\tLogin failed.");
				}
			}
		}

		if(flag == 0)
			printf("ID does not exist.\n");
}

int main(){

	int row = 0, col = 0;	
	int menu = 0;	
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	
	
	while(1)
	{
		clear();
		for (row = 0; row<50; row++)
		{
			for (col = 50; col<100; col++)
			{
				if (row == 10 || row == 16 || row == 28)
				{
					move(row, col);
					addch('*');
				}		
			}

			for (col = 50; col<100; col++)
			{
				if ((row >= 17 && row <= 27) && (col == 50 || col == 99))
				{
					move(row, col);
					addch('*');
				}
			}
		}

		mvaddstr(2,51,"*********   * *       *       *   *********  *  ");
		mvaddstr(3,51,"       *    * *       *       *     *   *    * ");
		mvaddstr(4,51,"      *     * *       *       *     *   *    *** ");
		mvaddstr(5,51,"     *   **** *      * *      *   *********  *  ");
		mvaddstr(6,51,"    *       * *     *   *     *   *          * ");
		mvaddstr(7,51,"   *        * *    *     *    *   *");
		mvaddstr(8,51,"  *         * *   *       *   *   ************");

		mvaddstr(20,68,"1) Sign Up");
		mvaddstr(24,68,"2) Sign In");	
		mvaddstr(13,68,"Choose Menu ");
		refresh();
		keypad(stdscr, TRUE);
	
			menu = getch();

			if(menu == '1')
			{
				endwin();
				display_sign_up();
			}

			else if(menu == '2')
			{	
				endwin();			
				display_sign_in();
			}
	}
	
	endwin();
	return 0;
}
