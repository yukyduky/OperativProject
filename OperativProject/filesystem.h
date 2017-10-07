#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"
#include <list>

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
    // Here you can add your own data structures

	struct File
	{
		std::string name;
		std::vector<int> blockPositions;
		std::string contains;

		File(std::string name, std::vector<int> blockPositions, std::string contains) : name(name), blockPositions(blockPositions), contains(contains) {};
	};

	struct Directory
	{
		std::string name;
		std::list<Directory> dirs;
		std::list<File> files;
		Directory* parent;

		Directory(std::string name, Directory* parent) : name(name), parent(parent) {};
	};

	Directory* workingDir;
	std::string currentPath;
	Directory rootDir;

public:
    FileSystem();
    ~FileSystem();

	void format();
	

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

    /* This function creates a file in the filesystem */
    // createFile(...)

    /* Creates a folder in the filesystem */
	int createDirectory(std::string name);

    /* Removes a file in the filesystem */
    // removeFile(...);

    /* Removes a folder in the filesystem */
    // removeFolder(...);
	int removeFolder(std::string dirPath);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);
	int changeDirectory(std::string dirPath);

    /* This function will get all the files and folders in the specified folder */
    // listDir(...);
	std::string listDir();

	std::string printWorkingDir();
    /* Add your own member-functions if needed */

	std::string printFile(std::string dirPath);

};

#endif // FILESYSTEM_H
