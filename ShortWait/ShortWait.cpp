#include <iostream>
#include <cstdlib>
#include <Windows.h>

/// <summary>
/// Small self contained program that will be used to test the shell program that I am creating.
/// Will print a message, wait one second, then print that message again.
/// If an arguement is given to the program, it will try and convert it into an integer and wait that number of seconds.
/// </summary>
int main(int argc, char** argv) {
	long int wait = 0;
	if (argc > 1) {
		wait = strtol(argv[1], NULL, 10);
	}

	if (wait < 1) {
		wait = 2;
	}

	printf("Sleeping for %i seconds... ", wait);

	Sleep(wait * 1000);

	printf("Done!\n");

	return 0;
}