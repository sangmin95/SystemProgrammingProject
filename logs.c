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

char set_star()
{
	int c;

	tty_mode(0);
	set_noecho();

	c = getchar();
	
	if(c >= 32)
		printf("*");

	tty_mode(1);

	return c;
}			

int display_menu(void)
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
			continue;
		}

		else
		{
			return menu;
		}
	}

	return 0;
}

int display_sign_up(void){

	char cwd[PATH_MAX], path[PATH_MAX], ch;
	char id[50],  *pwd1,  pwd2[50], name[50];
	char *newLine = "\n";
	int sign_file;
	int i, c;

	struct stat st = {0};

		pwd1 = (char*)malloc(sizeof(char) * 50);

		system("clear");		
		printf("\n\n\t\t\t\t 회 원 가 입");
		printf("\n\t\t\t****************************");
		printf("\n\n\t\t\t ID : ");

		scanf("%s", id);
		sign_file = open(id, O_WRONLY | O_APPEND | O_CREAT, 0644);
	
		printf("\n\t\t\t Password : ");

		getchar();
		i = 0;

		while(set_star() != '\n') {
			pwd1[i] = set_star();
			i++;
		}

		printf("\n\t\t\t Password check : ");
		scanf("%s", pwd2);

		if(getcwd(cwd, sizeof(cwd)) != NULL)
		{
			strcat(cwd, "/logfile");
			strcpy(path, cwd);

			if(stat(path, &st) == -1)
			{
				mkdir(path, 0777);
			}
		}

		if(strcmp(pwd1, pwd2) == 0)
		{
			printf("\n\t\t\t\t(correct password)\n");
			write(sign_file, pwd1, strlen(pwd1));
			write(sign_file, newLine, strlen(newLine));
		}

		else
		{
			printf("\t\t\t\t(password error)\n");	
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

	char id[50],  pwd[50], pwdC[50], buf[80];	
	char cwd[PATH_MAX], path[PATH_MAX], fname[PATH_MAX];
	FILE* fd;
	DIR *dir;

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

		while(direntp = readdir(dir))
		{
			if(strcmp(direntp->d_name, id))
			{
				strcpy(fname, path);
				strcat(fname, "/");
				strcat(fname, id);

				fd = fopen(fname, "r");
				
				printf("\n\t\t\t Password : ");
				scanf("%s", pwd);
			
				fscanf(fd, "%s", pwdC);

				if(strcmp(pwdC, pwd) == 0)
				{
					printf("Login success\n");

					return 0;
				}

				else
				{		
					display_sign_in();
				}
			}

			else
			{
				printf("ID does not exist. Please create account.\n");
				system("clear");
				display_menu();
			}
		}

		return 0;
}

int main(){

	int menu = 1;

	while(menu)
	{
		menu = display_menu();


		if(menu == 1)
		{
			display_sign_up();
		}
		
		else if(menu == 2)
		{	
			display_sign_in();
		}
	}

	return 0;
}
