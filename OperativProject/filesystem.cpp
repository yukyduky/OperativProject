#include "filesystem.h"

FileSystem::FileSystem() : currentPath(""), workingDir(&rootDir), rootDir("root", nullptr) {

}

FileSystem::~FileSystem() {

}

int FileSystem::createFile(std::string name, std::string contains)
{
	int result = 0;
	/*int i = 0;

	if (name == "") {
		result = -1;
	}
	else {
		std::string path = name;
		std::string fileName = this->getCurrentDirNameFromPath(path);

		this->createDirectory(path);



	}*/



	return result;
}

int FileSystem::createDirectory(std::string name)
{
	int result = 0;
	Directory* pathDir = nullptr;
	std::string path = name;
	std::string dirName;

	if (path != "") {
		if (path.front() == '/') {
			pathDir = &this->rootDir;
			path = path.substr(1, path.size());
		}
		else {
			pathDir = this->workingDir;
		}
		// Split the new dir from the rest of the path
		if (this->getFileNameFromPath(path, dirName) != 0) {
			result = -1;
		}
		else {
			// Get the dir that is suppose to hold the new dir. If the dirs leading up to it don't exist, create them.
			result = this->getDirFromPath(path, &pathDir, true);
			switch (result)
			{
			case -1:
			case 0:
				// If it can't go to the next dir, then that means that no other dir exists with the same name as the one we want to create
				if (this->getNextDir(dirName, &pathDir) != 0) {
					pathDir->dirs.push_back(Directory(dirName, pathDir));
				}
				else {
					result = 2;
				}
				break;
			case 2:
				result = 3;
			case 1:
				break;
			}
		}
	}
	else {
		result = -1;
	}

	return result;
}

std::string FileSystem::listDir()
{
	std::string list;

	for (std::list<Directory>::iterator it = this->workingDir->dirs.begin(); it != this->workingDir->dirs.end(); it++) {
		list += it->name + "\n";
	}

	for (std::list<File>::iterator it = this->workingDir->files.begin(); it != this->workingDir->files.end(); it++) {
		list += it->name + " ";
		list += it->contains.size();
		list += " bytes\n";
	}

	return list;
}

std::string FileSystem::printWorkingDir()
{
	std::string path;
	Directory* currentDir = this->workingDir;

	// Get the name of the current dir
	path += currentDir->name;

	// While currentDir has a parent, continue
	while (currentDir->parent != nullptr) {
		// Set currentDir to its parent
		currentDir = currentDir->parent;
		// Get the name of currentDir and add the rest of the path on top
		path = currentDir->name + '/' + path;		
	}

	path = '/' + path;

	return path;
}

int FileSystem::getNextDir(std::string dirName, Directory** currentDir)
{
	int result = 0;
	Directory* nextDir = nullptr;

	// Iterate through and look for a dir with the correct name
	for (std::list<Directory>::iterator it = (*currentDir)->dirs.begin(); it != (*currentDir)->dirs.end(); it++) {
		if (it->name == dirName) {
			nextDir = &(*it);
			break;
		}
	}
	if (nextDir != nullptr) {
		*currentDir = nextDir;
	}
	else {
		result = -1;
	}

	return result;
}

int FileSystem::getPrevDir(std::string dirName, Directory** currentDir)
{
	int result = 0;

	// Get the previous by assigning it's parent to itself
	if ((*currentDir)->parent != nullptr) {
		*currentDir = (*currentDir)->parent;
	}
	else {
		result = -1;
	}

	return result;
}

int FileSystem::getDirFromPath(std::string path, Directory** pathDir, bool createDirs)
{
	int result = 0;
	int i = 0;

	std::string currentPath = path;
	std::string dirName;

	// Get next dir name and then look for the next dir from that
	while (this->getNextDirNameFromPath(currentPath, dirName) == 0) {
		if (this->getNextDir(dirName, pathDir) != 0) {
			if (!createDirs) {
				// Exit if the dir doesn't exist
				result = 2;
				break;
			}
			else {
				// Create the dir that didn't exist
				(*pathDir)->dirs.push_back(Directory(dirName, *pathDir));
				*pathDir = &(*pathDir)->dirs.back();
				result = 0;
			}
		}
	}

	return result;
}

int FileSystem::getFileNameFromPath(std::string& path, std::string& fileName)
{
	int result = 0;

	// Finds the position of the last '/'
	size_t fileNamePos = path.find_last_of('/');

	// If no '/' was found
	if (fileNamePos != std::string::npos) {
		// Using substr to split the path from the name
		fileName = path.substr(fileNamePos + 1);
		path = path.substr(0, fileNamePos);
	}
	// Else if no '/' and path isn't empty
	else if (path != "") {
		fileName = path;
		path = "";
	}
	else {
		result = -1;
	}

	return result;
}

int FileSystem::getNextDirNameFromPath(std::string& path, std::string& dirName)
{
	int result = 0;
	size_t dirNamePos = path.find_first_of('/');
	if (dirNamePos != std::string::npos) {
		dirName = path.substr(0, dirNamePos);
		path = path.substr(dirNamePos + 1);
	}
	else if (path != "") {
		dirName = path;
		path = "";
	}
	else {
		result = -1;
	}

	return result;
}
