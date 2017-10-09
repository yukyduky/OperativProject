#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "memblockdevice.h"
#include <list>
#include <fstream>

class FileSystem
{
private:
    MemBlockDevice mMemblockDevice;
    // Here you can add your own data structures

	struct File
	{
		std::string name;
		std::vector<int> blockPositions;

		File(std::string name, std::vector<int> blockPositions) : name(name), blockPositions(blockPositions) {};
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

	/* Add your own member-functions if needed */
	int insertIntoString(std::string& moddedString, int moddedStartPos, std::string content);
	std::string listDir(Directory* dir);
	int parsePathAndDir(std::string& path, Directory** pathDir);
	int getNextDir(std::string dirName, Directory** currentDir);
	int getPrevDir(std::string dirName, Directory** currentDir);
	int getDirFromPath(std::string path, Directory** pathDir, bool createDirs);
	int getFileNameFromPath(std::string& path, std::string& fileName);
	int getNextDirNameFromPath(std::string& path, std::string& dirName);

public:
    FileSystem();
    ~FileSystem();	

    /* These API functions need to be implemented
	   You are free to specify parameter lists and return values
    */

    /* This function creates a file in the filesystem */
	int createFile(std::string dirPath, std::string content);

	int appendFileToFile(std::string dirPath1, std::string dirPath2);
	int appendContentToFile(std::string dirPath, std::string content);

    /* Creates a folder in the filesystem */
	int createDirectory(std::string dirPath);

    /* Removes a file in the filesystem */
	int removeFile(std::string dirPath);

    /* Removes a folder in the filesystem */
	int removeFolder(std::string dirPath);

    /* Function will move the current location to a specified location in the filesystem */
    // goToFolder(...);

	int changeDirectory(std::string dirPath);

    /* This function will get all the files and folders in the specified folder */
	std::string list(std::string dirPath);

	std::string printWorkingDir();
    /* Add your own member-functions if needed */

	std::string printFile(std::string dirPath);

	int createImage(std::string dirPath);
	int loadImage(std::string dirPath);

	int saveFolder(Directory* dir, std::ofstream &fstream);
	int recreateFolder(Directory* dir, std::ifstream &fstream);

	void format();


};

#endif // FILESYSTEM_H
