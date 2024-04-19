#pragma once

#include <iostream>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <filesystem>
#include <Windows.h>

#define FILE_SEP '\\'
#define FILE_SEP_ALT '/'

#define HOME_DIR_SYMBOL '~'
#define CURR_DIR_SYMBOL '.'

using fsPath = std::filesystem::path;

const char* HELP_TEXT =
	"<exit>  - Close the shell.\n"
	"<quit>  - Alias for \"exit\".\n"
	"<help>  - Print this message, displaying all commands. Note that commands are case sensitive.\n"
	"<ls>    - List all of the files within the current directory.\n"
	"<pwd>   - Print the path of the current working directory.\n"
	"<cd>    - Append the given text to the current working directory.\n"
	"<clear> - Clear all of the text that has been printed to the console\n"
	"<debug> - Check or set the programs debug output.\n";

const char* HELP_EXIT =
"<exit>\n\tClose the shell.\n"
"<quit>\n\tAlias for exit.\n"
"Handles all current child programs and exits the shell.\n"
"Will wait for child processes to terminate before actually terminating the shell.\n";
//"Parameters:\n"
//"\t-f - Force the shell to exit immedietly by immedietly terminating and reaping all child processes.\n";

const char* HELP_HELP =
"<help>\n\tPrints a list of commands and a short summary of their functions.\n"
"<help> <command name>\n\t Prints more detailed information about a specific command.\n";

const char* HELP_LS =
"<ls>\n\tPrint the files contained within the current working directory.\n"
"Flags:\n"
"\tl - Print the full path of each file.\n"
"\tv - Alias for the -l flag.\n"
"\ta - Print all files, including hidden files (file that start with a \'.\').\n";

const char* HELP_PWD =
"<pwd>\n\tPrint the full filepath of the current working directory.\n"
"";

const char* HELP_CD =
"<cd> <filename>\n\tChange the current working directory by appending the given filename to the path.\n"
"";

const char* HELP_CLEAR =
"<clear>\n\tClear all of the text that has been printed to the console.\n"
"";

const char* HELP_DEBUG =
"<debug>\n\tToggle the debug output of the program on or off.\n"
"<debug ?>\n\tQuerry the progrma for the current debug status.\n"
"<debug on>\n\tEnable debug messages.\n"
"<debug on>\n\tDisable debug messages.\n"
"";