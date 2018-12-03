#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
		printf("\n\t\t\t*\t2) 로 그 인\t\   *");
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

	char id[50];
	char pwd1[50];
	char pwd2[50];
	char name[50];	
	int sign_file;
	
		system("clear");		
		printf("\n\n\t\t\t\t 회 원 가 입");
		printf("\n\t\t\t****************************");
		printf("\n\n\t\t\t ID : ");
		scanf("%s", id);
		sign_file = open(id, O_WRONLY | O_APPEND | O_CREAT, 0644);
		printf("\n\t\t\t Password : ");
		scanf("%s", pwd1);
		printf("\n\t\t\t Password check : ");
		scanf("%s", pwd2);
		if(strcmp(pwd1, pwd2) == 0)
		{
			printf("\t\t\t\t(correct password)\n");
			write(sign_file, pwd1,strlen(pwd1));
		}
		else
		{
			printf("\t\t\t\t(password error)\n");	
		}
		printf("\n\t\t\t Nickname : ");
		scanf("%s", name);
		write(sign_file, name, strlen(name));
		
		return 0;
}

int display_sign_in(void){

	char id[50];
	char pwd[50];	
	FILE *fp;
	int Read;
	
		system("clear");		
		printf("\n\n\t\t\t\t   로 그 인");
		printf("\n\t\t\t****************************");
		printf("\n\n\t\t\t ID : ");
		scanf("%s", id);
		fp = fopen (id, "r");
		printf("\n\t\t\t Password : ");
		scanf("%s", pwd);
		Read = read(fp);
		if(strcmp(Read, pwd) == 0)
		{
			printf("ok");
		}
		else
		{
			printf("error");
		}

		return 0;
}

void sign_in(){
	
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
	
