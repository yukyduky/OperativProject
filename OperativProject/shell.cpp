#include <iostream>
#include <sstream>
#include "filesystem.h"

const int MAXCOMMANDS = 8;
const int NUMAVAILABLECOMMANDS = 15;

std::string availableCommands[NUMAVAILABLECOMMANDS] = {
    "quit", "format","ls","create","cat","createImage","restoreImage",
    "rm","cp","append","mv","mkdir","cd","pwd","help"
};

/* Takes usercommand from input and returns number of commands, commands are stored in strArr[] */
int parseCommandString(const std::string &userCommand, std::string strArr[]);
int findCommand(std::string &command);
bool quit();
std::string help();

/* More functions ... */

int main(void) {

	std::string userCommand, commandArr[MAXCOMMANDS];
	std::string user = "user@DV1492";    // Change this if you want another user to be displayed
	std::string currentDir = "/";    // current directory, used for output
	FileSystem fileSys;
	int result = 0;

    bool bRun = true;

    do {
		for (int i = 0; i < MAXCOMMANDS; i++) {
			commandArr[i].clear();
		}

        std::cout << user << ":" << currentDir << "$ ";
        getline(std::cin, userCommand);

        int nrOfCommands = parseCommandString(userCommand, commandArr);
        if (nrOfCommands > 0) {

            int cIndex = findCommand(commandArr[0]);
            switch(cIndex) {

			case 0: //quit
				bRun = quit();                
                break;
            case 1: // format
                break;
            case 2: // ls
                std::cout << "Listing directory" << std::endl;
				std::cout << fileSys.list(commandArr[1]) << std::endl;
                break;
            case 3: // create
			{
				std::string content = "";
				std::cout << "Input content: ";
				std::cin >> content;
				result = fileSys.createFile(commandArr[1], content);

				switch (result)
				{
				case -1:
					std::cout << "Pathname missing, eg: \"test/test/test\" (relative) or \"/test/test/test\" (aboslute)" << std::endl;
					break;
				case 1:
					std::cout << "Content exceeds max length." << std::endl;
					break;
				case 2:
					std::cout << "Not enough memory left." << std::endl;
					break;
				case 3:
					std::cout << "Failed to write to memory." << std::endl;
				default:
					std::cout << "Successfully created a new file." << std::endl;
					break;
				}

				break;
			}
            case 4: // cat
                break;
            case 5: // createImage
                break;
            case 6: // restoreImage
                break;
            case 7: // rm
                break;
            case 8: // cp
                break;
            case 9: // append
                break;
            case 10: // mv
                break;
            case 11: // mkdir
				result = fileSys.createDirectory(commandArr[1]);
				
				switch (result)
				{
				case -1:
					std::cout << "Pathname missing, eg: \"test/test/test\" (relative) or \"/test/test/test\" (aboslute)" << std::endl;
					break;
				case 1:
					std::cout << "Missing final directory name." << std::endl;
					break;
				case 2:
					std::cout << "A folder with that name already exists." << std::endl;
					break;
				case 3:
					std::cout << "This shouldn't happen." << std::endl;
				default:
					std::cout << "Successfully created a new dir." << std::endl;
					break;
				}

                break;
            case 12: // cd
                break;
            case 13: // pwd
				std::cout << fileSys.printWorkingDir() << std::endl;
                break;
            case 14: // help
                std::cout << help() << std::endl;
                break;
            default:
                std::cout << "Unknown command: " << commandArr[0] << std::endl;
            }
        }
    } while (bRun == true);

    return 0;
}

int parseCommandString(const std::string &userCommand, std::string strArr[]) {
    std::stringstream ssin(userCommand);
    int counter = 0;
    while (ssin.good() && counter < MAXCOMMANDS) {
        ssin >> strArr[counter];
        counter++;
    }
    if (strArr[0] == "") {
        counter = 0;
    }
    return counter;
}
int findCommand(std::string &command) {
    int index = -1;
    for (int i = 0; i < NUMAVAILABLECOMMANDS && index == -1; ++i) {
        if (command == availableCommands[i]) {
            index = i;
        }
    }
    return index;
}

bool quit() {
	std::cout << "Exiting\n";
	return false;
}

std::string help() {
    std::string helpStr;
    helpStr += "OSD Disk Tool .oO Help Screen Oo.\n";
    helpStr += "-----------------------------------------------------------------------------------\n" ;
    helpStr += "* quit:                             Quit OSD Disk Tool\n";
    helpStr += "* format;                           Formats disk\n";
    helpStr += "* ls     <path>:                    Lists contents of <path>.\n";
    helpStr += "* create <path>:                    Creates a file and stores contents in <path>\n";
    helpStr += "* cat    <path>:                    Dumps contents of <file>.\n";
    helpStr += "* createImage  <real-file>:         Saves disk to <real-file>\n";
    helpStr += "* restoreImage <real-file>:         Reads <real-file> onto disk\n";
    helpStr += "* rm     <file>:                    Removes <file>\n";
    helpStr += "* cp     <source> <destination>:    Copy <source> to <destination>\n";
    helpStr += "* append <source> <destination>:    Appends contents of <source> to <destination>\n";
    helpStr += "* mv     <old-file> <new-file>:     Renames <old-file> to <new-file>\n";
    helpStr += "* mkdir  <directory>:               Creates a new directory called <directory>\n";
    helpStr += "* cd     <directory>:               Changes current working directory to <directory>\n";
    helpStr += "* pwd:                              Get current working directory\n";
    helpStr += "* help:                             Prints this help screen\n";
    return helpStr;
}

/* Insert code for your shell functions and call them from the switch-case */
