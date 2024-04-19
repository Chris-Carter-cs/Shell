#include "Shell.h"

#pragma region Forwards

void WriteDebug(std::string _log);
void WriteDebug(std::string _log, std::string _context);

int SplitString(std::string _line, char _deliminter, std::vector<std::string>* _outLines);

bool BuiltIn(std::vector<std::string>* _lines);
void ProcessCommand(std::vector<std::string>* _lines, std::string _line);

#pragma endregion

bool debug;

fsPath currentPath;
fsPath defaultPath;

int main(int argc, char** argv)
{
    debug = false;
    defaultPath = currentPath = getenv("HOMEPATH");

    currentPath = currentPath.lexically_normal();

    //Loop to process arguements.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = true;
        }
    }

    WriteDebug("Debug Enabled");

    WriteDebug("Home directory found to be", currentPath.u8string().c_str());

    WriteDebug("Entering main loop");

    std::string line = "";
    std::vector<std::string> linesV;
    std::string curr = "";

    while (true) {

        
        //print prompt and get user input.
        std::cout << "> ";
        std::getline(std::cin, line);

        //Flush cout
        std::flush(std::cout);

        //Split the string into individual words.
        SplitString(line, ' ', &linesV);

        //Handle built in commands.
        if (!BuiltIn(&linesV)) {

            //This is only reached if the given line is NOT a command.
            //Interpret the line as a "run" command.
            ProcessCommand(&linesV, line);
        }

    }
}

#pragma region Utilities

int SplitString(std::string _line, char _deliminter, std::vector<std::string>* _outLines) {

    _outLines->clear();
    std::istringstream iss(_line);
    int i = 0;
    while (!iss.eof()) {
        std::string word = "";
        std::getline(iss, word, _deliminter);
        _outLines->emplace_back(word);
        i++;
    }
    return i;
}



#pragma endregion

#pragma region Debugging

void WriteDebug(std::string _log) {
    if (debug) printf("\t<%s>\n", _log.c_str());
}

void WriteDebug(std::string _log, std::string _context) {
    std::string combined = _log;
    combined.append(_context);
    WriteDebug(combined);
}

#pragma endregion

#pragma region BuiltIns

void help(std::vector<std::string>* _lines);
void ls(std::vector<std::string>* _lines);
void cd(std::vector<std::string>* _lines);

bool BuiltIn(std::vector<std::string>* _lines) {
    WriteDebug("Checking builtins");

    std::string command = _lines->at(0);
    
    if (std::strcmp(command.c_str(), "exit") == 0
        || std::strcmp(command.c_str(), "quit") == 0) {
        printf("Exiting program due to exit command.\n");
        exit(0);
    }
    
    if (std::strcmp(command.c_str(), "help") == 0) {
        help(_lines);
        return true;
    }
    
    if (std::strcmp(command.c_str(), "pwd") == 0) {
        printf("%s\n", currentPath.u8string().c_str());
        return true;
    }
    
    if (std::strcmp(command.c_str(), "ls") == 0) {
        ls(_lines);
        return true;
    }

    if (std::strcmp(command.c_str(), "cd") == 0) {
        cd(_lines);
        return true;
    }

    return false;
}


void help(std::vector<std::string>* _lines) {
    //First, check to see if there are no args.
    if (_lines->size() == 1) {
        //If so, just print a list of commands and return.
        printf(HELP_TEXT);
        return;
    }
    
    //Loop to process arguements.
    for (int i = 1; i < _lines->size(); i++) {
        std::string arg = _lines->at(i);

        if (strcmp(arg.c_str(), "exit") == 0 ||
            strcmp(arg.c_str(), "quit") == 0) {
            printf(HELP_EXIT);
        } else if (strcmp(arg.c_str(), "help") == 0) {
            printf(HELP_HELP);
        } else if (strcmp(arg.c_str(), "ls") == 0) {
            printf(HELP_LS);
        } else if (strcmp(arg.c_str(), "pwd") == 0) {
            printf(HELP_PWD);
        } else if (strcmp(arg.c_str(), "cd") == 0) {
            printf(HELP_CD);
        }

    }
}


/// <summary>
/// Built in command to list all of the files within a given directory.
/// </summary>
void ls(std::vector<std::string>* _lines) {
    bool verbose = false;
    bool all = false;
    fsPath target = currentPath;

    //Loop to process arguements.
    for (int i = 1; i < _lines->size(); i++) {
        std::string arg = _lines->at(i);
        if (strcmp(arg.c_str(), "-v") == 0 ||
            strcmp(arg.c_str(), "-l") == 0) {
            //Verbose / Long format
            verbose = true;
        } else if (strcmp(arg.c_str(), "-a") == 0) {
            //All files
            all = true;
        }

        //Check if the arguement is a filename instead of a flag
        if (arg.at(0) != '-') {
            if (arg.at(0) == '.' && (arg.size() == 1 || arg.at(1) != '.')) {
                //Case 1: First character is a dot and the second is not.
                target = defaultPath;
            }
            else if (arg.at(0) == FILE_SEP ||
                arg.at(0) == FILE_SEP_ALT) {
                //Case 2: First character is a seperator
                target = "";
            }
            else {
                //Case 3: Normal appending
                target = currentPath;
            }

            target /= _lines->at(1);
        }
    }

    if (!std::filesystem::is_directory(target)) {
        printf("ls command failed due to nonexistant directory at: %s\n", target.u8string().c_str());
        return;
    }

    std::string full;
    std::string relative;
    std::filesystem::directory_iterator itt = std::filesystem::directory_iterator(target);
    while (!itt._At_end()) {
        //Find whole string.
        full = std::string(itt->path().u8string());
        //Find relative name.
        relative = full.substr(full.find_last_of(FILE_SEP) + 1);
        
        //Skip over "hidden" files if the "All" tag is not set.
        if (!all && relative.at(0) == '.') {
            itt++;
        }
        else {

            if (verbose) {
                std::cout << full;
            }
            else {
                std::cout << relative;
            }

            std::cout << "\n";
            itt++;
        }
    }
}

/// <summary>
/// Built in command used to change the current working directory.
/// </summary>
void cd(std::vector<std::string>* _lines) {
    //This command should only have one parameter.
    if (_lines->size() != 2) {
        //TODO: Make sure that the shell can take parameters with spaces in them,
        //  assuming they're wrapped in a pair of quotation marks.
        printf("Usage: <cd> <filename>\n");
        return;
    }

    //Three cases for changing the current directory:
    //  1: the first character of the first arguement is a dot, representing the default home directory.
    //      Unless the second character is also a dot, which will signify the parent directory instead.
    //  2: the first character of the first arguement is a seperator, representing the root directory.
    //  3: the first character of the first arguement does not begin with a dot or a seperator.
    //      In this case, the arguement should be appended to the current path and made lexically normal.


    fsPath target;
    std::string arg = _lines->at(1);
    if (arg.at(0) == '.' && (arg.size() == 1 || arg.at(1) != '.')) {
        //Case 1: First character is a dot and the second is not.
        target = defaultPath;
    }
    else if (arg.at(0) == FILE_SEP ||
        arg.at(0) == FILE_SEP_ALT) {
        //Case 2: First character is a seperator
        target = "";
    }
    else {
        //Case 3: Normal appending
        target = currentPath;
    }
    
    target /= _lines->at(1);


    //Make sure the target path is lexically normal.
    target = target.lexically_normal();

    //Check to make sure the given directory exists.
    if (!std::filesystem::is_directory(target)) {
        printf("cd command failed due to nonexistant directory at: %s\n", target.u8string().c_str());
    }
    else {
        currentPath = target.u8string();
    }
}

#pragma endregion

#pragma region External

/// <summary>
/// Take a program name as an input and then run that program as a new process, forwarding the appropriate parameters to it.
/// </summary>
void ProcessCommand(std::vector<std::string>* _lines, std::string _line) {
    WriteDebug("Running program at ", _lines->at(0).c_str());

    //Make sure there is at least one arguement given.
    if (_lines->size() < 1) {
        printf("Error in running program: Requires at least one arguement\n"
             "\tIf this error is displayed, then ProcessCommand was somehow called with zero arguements.");
    }

    //Create and zero information structures for the process and its startup.
    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION procInfo;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&procInfo, sizeof(procInfo));

    //Check to make sure given file actually exists and can be run.
    fsPath target = currentPath;
    target /= _lines->at(0).c_str();

    if (!std::filesystem::exists(target)) {
        printf("Specified file does not exist.\n");
        return;
    }

    if (strcmp(target.extension().u8string().c_str(), ".exe") != 0) {
        printf("Specified file is not a valid type (.exe)\n");
        return;
    }

    WriteDebug("Running file at: ", target.u8string().c_str());

    LPSTR lineLPSTR = const_cast<char*>(_line.c_str());

    BOOL result = CreateProcessA(
        target.u8string().c_str(),
        lineLPSTR,
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to TRUE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &startupInfo,       // Pointer to STARTUPINFO structure
        &procInfo           // Pointer to PROCESS_INFORMATION structure
        );

    if (result == 0) {
        printf("CreateProcessA function falled to run.\n");
        return;
    }

    WaitForSingleObject(procInfo.hProcess, INFINITE);

    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);

    WriteDebug("Foreground process terminated.");
}

#pragma endregion