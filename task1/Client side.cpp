#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>



void listen_to_server(FILE *server_file, FILE *client_file)
{

	printf("Listening to Server.\n");
	int len = 0;
	//waiting until the length becomes non zero
	while (1)
	{
		fseek(server_file, 2, SEEK_SET);
		fread(&len, sizeof(int), 1, server_file);
		if (len != 0)
			break;
	}
	//reading the string with given lenth
	char *str = (char*)malloc(sizeof(char)*(len + 1));
	fseek(server_file, 2 + sizeof(int), SEEK_SET);
	fread(str, sizeof(char), len, server_file);
	str[len] = '\0';
	printf("%s\n", str);
	free(str);

	//writing the acknowledgement
	char ack = '1';
	fseek(client_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, client_file);

	//performing another operation to confirm for client
	fseek(client_file, 1, SEEK_SET);
	fread(&ack, sizeof(char), 1, client_file);
}


void write_to_server(FILE *client_file, FILE *server_file)
{
	printf("Message to server:\n");
	char *string;
	string = (char*)malloc(sizeof(char)*(1024 - 6));
	int len = 0;
	char ack = '0';
	//write the ackwnoledgement as 0 initially
	fseek(client_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, client_file);

	//write length of the string as 0 initially
	fseek(client_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, client_file);
	
	do
	{
		gets(string);
		len = strlen(string);
	} while (len == 0);

	//write the string
	fseek(client_file, 2 + sizeof(int), SEEK_SET);
	fwrite(string, sizeof(char), len, client_file);

	//write the length of the string
	fseek(client_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, client_file);

	//performing another operation to confirm for server
	fseek(client_file, 2, SEEK_SET);
	fread(&len, sizeof(int), 1, client_file);

	//waiting until the acknowledgement becomes 1
	while (1)
	{
		fseek(server_file, 1, SEEK_SET);
		fread(&ack, sizeof(char), 1, server_file);
		if (ack == '1')
		{
			len = 0;
			//write the length of the string as 0
			fseek(client_file, 2, SEEK_SET);
			fwrite(&len, sizeof(int), 1, client_file);

			//performing another operation to confirm for server
			fseek(client_file, 2, SEEK_SET);
			fread(&len, sizeof(int), 1, client_file);
			break;
		}
	}
	free(string);
}


void is_server_started(FILE *server_file)
{
	char flag;
	fseek(server_file, 0, SEEK_SET);
	fread(&flag, sizeof(char), 1, server_file);
	if (flag != '1')
	{
		printf("No Server.\n");
		getchar();
		exit(0);
	}
}


void start_indication(FILE *client_file)
{
	//start indication first bit as 1 and acknowledgement second bit as 0
	fseek(client_file, 0, SEEK_SET);
	fwrite("10", sizeof(char), 2, client_file);

	//performing another operation to confirm for client 
	fseek(client_file, 0, SEEK_SET);
	char c;
	fread(&c, sizeof(char), 1, client_file);
}


void start_client(FILE *server_file, FILE *client_file)
{
	start_indication(client_file);
	while (1)
	{
		is_server_started(server_file);
		write_to_server(client_file,server_file);
		listen_to_server(server_file, client_file);
	}
}

int main()
{
	FILE *server_file, *client_file, *local_file;
	server_file = fopen("D:\\client read server write.bin", "rb+");
	client_file = fopen("D:\\client write server read.bin", "wb+");
	start_client(server_file, client_file);
}
