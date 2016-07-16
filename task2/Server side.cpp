#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<Windows.h>


void listen_to_client(FILE *comm_file)
{
	printf("Listening to Client.\n");
	int len = 0;
	//waiting until the length becomes non zero
	while (1)
	{
		fseek(comm_file, 1024 + 2, SEEK_SET);
		fread(&len, sizeof(int), 1, comm_file);
		if (len != 0)
			break;
	}
	//reading the string with given lenth
	char *str = (char*)malloc(sizeof(char)*(len + 1));
	fseek(comm_file, 1024 + 2 + sizeof(int), SEEK_SET);
	fread(str, sizeof(char), len, comm_file);
	str[len] = '\0';
	printf("%s\n", str);
	free(str);
	
	//writing the acknowledgement
	char ack = '1';
	fseek(comm_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, comm_file);

	//performing another operation to confirm for client
	fseek(comm_file, 1, SEEK_SET);
	fread(&ack, sizeof(char), 1, comm_file);
}


void write_to_client(FILE *comm_file)
{
	printf("Reply to Client:\n");
	char string[100];
	int len = 0;
	char ack = '0';
	//write the ackwnoledgement as 0 initially
	fseek(comm_file, 1, SEEK_SET);
	fwrite(&ack, sizeof(char), 1, comm_file);

	//write length of the string as 0 initially
	fseek(comm_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, comm_file);

	do
	{
		gets(string);
		len = strlen(string);
	} while (len == 0);

	//write the string
	fseek(comm_file, 2 + sizeof(int), SEEK_SET);
	fwrite(string, sizeof(char), len, comm_file);

	//write the length of the string
	fseek(comm_file, 2, SEEK_SET);
	fwrite(&len, sizeof(int), 1, comm_file);
	len = 0;

	//performing another operation to confirm for client
	fseek(comm_file, 2, SEEK_SET);
	fread(&len, sizeof(int), 1, comm_file);

	//waiting until the acknowledgement becomes 1
	while (1)
	{
		fseek(comm_file, 1024 + 1, SEEK_SET);
		fread(&ack, sizeof(char), 1, comm_file);
		if (ack == '1')
		{
			len = 0;
			//write length of the string as 0
			fseek(comm_file, 2, SEEK_SET);
			fwrite(&len, sizeof(int), 1, comm_file);

			//performing another operation to confirm for client
			fseek(comm_file, 2, SEEK_SET);
			fread(&len, sizeof(int), 1, comm_file);
			break;
		}
			
	}
}


int is_client_started(FILE *comm_file)
{
	char flag = '0';
	printf("Checking client is started or not.\n");
	while (1)
	{
		fseek(comm_file, 1024, SEEK_SET);
		fread(&flag, sizeof(char), 1, comm_file);
		if (flag == '1')
		{
			printf("Client has started.\n");
			return 1;
		}
	}
}


void start_indication(FILE *comm_file)
{
	//start indication first bit as 1 and acknowledgement as 0
	fseek(comm_file, 0, SEEK_SET);
	fwrite("10", sizeof(char), 2, comm_file);

	//performing another operation to confirm for client 
	char c;
	fseek(comm_file, 0, SEEK_SET);
	fread(&c, sizeof(char), 1, comm_file);
}


void start_server(FILE *comm_file)
{
	start_indication(comm_file);
	is_client_started(comm_file);
	while (1)
	{
		listen_to_client(comm_file);
		write_to_client(comm_file);
	}
}


int main()
{
	FILE *comm_file;
	comm_file = fopen("D:\\client and server.bin", "rb+");
	start_server(comm_file);
}