#include "Shell.h"


void WriteDebug(std::string _log);
void WriteDebug(std::string _log, std::string _context);

int SplitString(std::string _line, char _deliminter, std::vector<std::string>* _outLines);

bool BuiltIn(std::vector<std::string>* _lines);
void ProcessCommand(std::vector<std::string>* _lines);

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
            ProcessCommand(&linesV);
        }

    }
}

void WriteDebug(std::string _log) {
    if (debug) printf("\t<%s>\n", _log.c_str());
}

void WriteDebug(std::string _log, std::string _context) {
    std::string combined = _log;
    combined.append(_context);
    WriteDebug(combined);
}

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

void ls(std::vector<std::string>* _lines) {
    bool verbose = false;
    bool all = false;
    
    //Loop to process arguements.
    for (int i = 1; i < _lines->size(); i++) {
        std::string arg = _lines->at(i);

        if (strcmp(arg.c_str(), "-v") == 0 ||
            strcmp(arg.c_str(), "-l") == 0) {
            verbose = true;
        }

        if (strcmp(arg.c_str(), "-a") == 0) {
            all = true;
        }
    }

    std::string full;
    std::string relative;
    std::filesystem::directory_iterator itt = std::filesystem::directory_iterator(currentPath);
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
    //  1: the first character of the first arguement does not begin with a dot or a seperator.
    //      In this case, the arguement should be appended to the current path and made lexically normal.
    //  2: the first character of the first arguement is a dot, representing the default home directory.
    //  3: the first character of the first arguement is a seperator, representing the root directory.

    fsPath target;
    std::string arg = _lines->at(1);
    if (arg.at(0) == '.' && (arg.size() == 1 || arg.at(1) != '.')) {
        //Case 2: First character is a dot and the second is not.
        target = defaultPath;
    }
    else if (arg.at(0) == FILE_SEP ||
        arg.at(0) == FILE_SEP_ALT) {
        //Case 3: First character is a seperator
        target = "";
    }
    else {
        //Case 1: Normal appending
        target = currentPath;
    }
    
    target /= _lines->at(1);


    //Make sure the target path is lexically normal.
    target = target.lexically_normal();

    //Check to make sure the given directory exists.
    if (!std::filesystem::is_directory(target)) {
        printf("cd command failed due to nonexistant directory at:\n%s\n", target.u8string().c_str());
    }
    else {
        currentPath = target.u8string();
    }
}


void ProcessCommand(std::vector<std::string>* _lines) {

}