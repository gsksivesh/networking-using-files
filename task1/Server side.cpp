#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>


void listen_to_client(FILE *client_file, FILE *server_file)
{
	printf("Listening to Client.\n");
	int len = 0;
	//waiting until the length becomes non zero
	while (1)
	{
		fseek(client_file, 2, SEEK_SET);
		fread(&len, sizeof(int), 1, client_file);
		if (len != 0)
			break;
	}
	//reading the string with given lenth
	char *str = (char*)malloc(sizeof(char)*(len + 1));
	fseek(client_file, 2 + sizeof(int), SEEK_SET);
	fread(str, sizeof(char), len, client_file);
	str[len] = '\0';
	printf("%s\n", str);
	free(str);
	
	//writing the acknowledgement
	char ack = '1';
	fseek(server_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, server_file);

	//performing another operation to confirm for client
	fseek(server_file, 1, SEEK_SET);
	fread(&ack, sizeof(char), 1, server_file);
}


void write_to_client(FILE *server_file, FILE *client_file)
{
	printf("Reply to Client:\n");
	char string[100];
	gets(string);
	int len = 0;
	char ack = '0';
	//write the ackwnoledgement as 0 initially
	fseek(server_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, server_file);

	//write length of the string as 0 initially
	fseek(server_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, server_file);

	len = strlen(string);

	//write the string
	fseek(server_file, 2 + sizeof(int), SEEK_SET);
	fwrite(string, sizeof(char), len, server_file);

	//write the length of the string
	fseek(server_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, server_file);
	len = 0;

	//performing another operation to confirm for client
	fseek(server_file, 1, SEEK_SET);
	fread(&len, sizeof(int), 1, server_file);

	//waiting until the acknowledgement becomes 1
	while (1)
	{
		fseek(client_file, 1, SEEK_SET);
		fread(&ack, sizeof(char), 1, client_file);
		if (ack == '1')
		{
			len = 0;
			//write length of the string as 0
			fseek(server_file, 2, SEEK_SET);
			fwrite(&len, sizeof(int), 1, server_file);

			//performing another operation to confirm for client
			fseek(server_file, 1, SEEK_SET);
			fread(&len, sizeof(int), 1, server_file);
			break;
		}
			
	}
}


int is_client_started(FILE *client_file)
{
	char flag = '0';
	printf("Checking client is started or not.\n");
	while (1)
	{
		fseek(client_file, 0, SEEK_SET);
		fread(&flag, sizeof(char), 1, client_file);
		if (flag == '1')
		{
			printf("Client has started.\n");
			return 1;
		}
	}
}


void start_indication(FILE *server_file)
{
	//start indication first bit as 1 and acknowledgement as 0
	fseek(server_file, 0, SEEK_SET);
	fwrite("10", sizeof(char), 2, server_file);

	//performing another operation to confirm for client 
	char c;
	fseek(server_file, 0, SEEK_SET);
	fread(&c, sizeof(char), 1, server_file);
}


void start_server(FILE *server_file, FILE *client_file)
{
	start_indication(server_file);
	is_client_started(client_file);
	while (1)
	{
		listen_to_client(client_file, server_file);
		write_to_client(server_file, client_file);
	}
}


int main()
{
	FILE *server_file, *client_file, *local_file;
	server_file = fopen("D:\\client read server write.bin", "wb+");
	client_file = fopen("D:\\client write server read.bin", "rb+");
	start_server(server_file, client_file);
}
