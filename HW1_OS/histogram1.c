#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


void printRes(int* res, int size);
int checkIndex(int num);
int* countHistogram(int* arr, int size);
void pipe_rw(int* fd1, int* arr, int size);


int main(int argc, char** argv)
{
	FILE* fp;
	pid_t pid1, pid2;
	int res[10] = { 0 };
	if (argc != 2 || strcmp(argv[1], "--help") == 0)
	{
		perror("no fole given");
	}
	fp = fopen(argv[1], "r");
	if (!fp)
		exit(EXIT_FAILURE);
	int size;
	if (!fread(&size, 1, sizeof(int), fp))
		perror("can't read from file");
	int* arr = (int*)malloc(size * sizeof(int));
	if (!arr)
		perror("no memory avaliable");
	if (!fread(arr, size, sizeof(int), fp))
		perror("no elements in file");

	int fd1[2];
	int fd2[2];
	int fd3[2];
	int fd4[2];

	if (pipe(fd1) == -1)
	{
		perror("PIPE 1");
		exit(1);
	}
	if (pipe(fd2) == -1)
	{
		perror("PIPE 2");
		exit(1);
	}
	if (pipe(fd3) == -1)
	{
		perror("PIPE 3");
		exit(1);
	}
	if (pipe(fd4) == -1)
	{
		perror("PIPE 4");
		exit(1);
	}

	pipe_rw(fd1, arr, size / 2);
	/*====================================================*/
	pid1 = fork();
	if (pid1 < 0)
	{
		perror("child 1");
		exit(1);
	}
	/*======================CHILD1==============================*/
	if (pid1 == 0)
	{

		if (close(fd1[0]) == -1)
			perror("reading pipe1 end");
		if (write(fd1[1], countHistogram(&fd1[0], sizeof(fd1[0]) / sizeof(int)), sizeof(int) * 10))
			perror("failed to write to pipe1");
		if (close(fd1[1]) == -1)
			perror("writing pipe1 end");

	}


	if (size % 2 != 0)
	{
		arr = arr + size / 2 + 1;
		pipe_rw(fd2, arr, size / 2 + 1);
	}
	else
	{
		arr = arr + size / 2;
		pipe_rw(fd2, arr, size / 2);
	}

	pid2 = fork();
	if (pid2 < 0)
		perror("child 2");
	if (pid2 == 0)
	{
		if (close(fd2[0]) == -1)
			perror("reading pipe1 end");
		if (write(fd2[1], countHistogram(&fd2[0], sizeof(fd2[0]) / sizeof(int)), sizeof(int) * 10))
			perror("failed to write to pipe2");
		if (close(fd2[1]) == -1)
			perror("writing pipe1 end");
	}

	while (wait(NULL) != -1 || errno != ECHILD);
	int i;
	int res1[10];
	int res2[10];
	read(fd1[0], res1, sizeof(int) * 10);
	read(fd2[0], res2, sizeof(int) * 10);
	for (i = 0; i < 10;i++)
	{
		res[i] = res1[i] + res2[i];
	}

	printRes(res, sizeof(res) / sizeof(res[0]));
}


void pipe_rw(int* fd1, int* arr, int size)
{
	int resArr[10] = { 0 };
	if (close(fd1[0]) == -1) //parent read close reading pipe1
		perror("fd1 reading end");
	if (write(fd1[1], arr, sizeof(int) * size / 2) == -1) // parrent write to pipe1
		perror("Error writing to pipe");
	if (close(fd1[1]) == -1) // parrent finish writing to pipe1
		perror("fd1 writing end");
	wait(NULL);
	if (read(fd1[0], resArr, sizeof(resArr)) == -1) //parrent reading from pipe2
		perror("Error reading from pipe");
	if (close(fd1[0]) == -1) //parrent closing pipe2
		perror("error reading end");
}

int* countHistogram(int* arr, int size)
{
	int* res;
	res = (int*)realloc(res, sizeof(int) * 10);
	int i;
	for (i = 0; i < size; i++)
	{
		if (arr[i] < 1 && arr[i] > 100)
		{
			perror("invalid number");
			exit(1);
		}
		res[checkIndex(arr[i])]++;
	}
	return res;
}

int checkIndex(int num)
{
	if (1 <= num && num <= 10)
		return 0;
	if (11 <= num && num <= 20)
		return 1;
	if (21 <= num && num <= 30)
		return 2;
	if (31 <= num && num <= 40)
		return 3;
	if (41 <= num && num <= 50)
		return 4;
	if (51 <= num && num <= 60)
		return 5;
	if (61 <= num && num <= 70)
		return 6;
	if (71 <= num && num <= 80)
		return 7;
	if (81 <= num && num <= 90)
		return 8;
	if (91 <= num && num <= 100)
		return 9;
}



void printRes(int* res, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (res[i] == 0)
			continue;
		printf("%d - %d: %d\n", (i * 10) + 1, (i + 1) * 10, res[i]);
	}
}