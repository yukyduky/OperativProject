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
		std::vector<int> blockPosition;
		std::string contains;
	};

	struct Directory
	{
		std::string name;
		std::list<Directory> dirs;
		std::list<File> files;
		Directory* parent;
	};

	Directory* currentDir;
	std::string currentPath;
	Directory* rootDir;

public:
    FileSystem();
    ~FileSystem();

	void format();
	int removeFolder(std::string dirPath);

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

    /* This function creates a file in the filesystem */
    // createFile(...)

    /* Creates a folder in the filesystem */
    // createFolderi(...);
	int createDirectory(std::string name);

    /* Removes a file in the filesystem */
    // removeFile(...);

    /* Removes a folder in the filesystem */
    // removeFolder(...);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

    /* This function will get all the files and folders in the specified folder */
    // listDir(...);

    /* Add your own member-functions if needed */
};

#endif // FILESYSTEM_H
