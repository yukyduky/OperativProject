#include "filesystem.h"

FileSystem::FileSystem() : currentPath(""), currentDir(&rootDir), rootDir("root", nullptr) {

}

FileSystem::~FileSystem() {

}

int FileSystem::createDirectory(std::string name)
{
	int result = 0;
	Directory* startDir = nullptr;
	bool sameNameFound = false;
	int i = 0;

	// Fails if no argument is given
	if (name == "") {
		result = -1;
	}
	else if (name.back() == '/') {
		result = 1;
	}
	else {
		// A '/' as the first symbol in the pathname signifies an absolute path
		if (name.front() == '/') {
			i = 1;
			startDir = &this->rootDir;
		}
		else {
			startDir = this->currentDir;
		}

		// Continue as long as we haven't reached the end of the pathname
		while (i < name.size()) {
			std::string dirName;

			// Get the name of the directory in between two '/'
			while (name[i] != '/' && i < name.size()) {
				dirName += name[i];
				i++;
			}

			// If ".." go back to the parent
			if (dirName == "..") {
				if (startDir->parent != nullptr) {
					startDir = startDir->parent;
				}
			}
			// If there are no directories in the current directory, then there can't be another directory with the same name
			else if (startDir->dirs.size() == 0) {
				// Create the new directory and add it to the list of directories in the current directory along the path
				startDir->dirs.push_back(Directory(dirName, startDir));
				// Move up the current directory to the one we just created
				startDir = &startDir->dirs.back();
			}
			// Else we need to iterate through them to check if there exists one with the same name already and use that one instead of creating a new one
			else
			{
				sameNameFound = false;

				for (std::list<Directory>::iterator it = startDir->dirs.begin(); it != startDir->dirs.end(); it++) {
					if (it->name == dirName) {
						sameNameFound = true;
						startDir = &(*it);
						break;
					}
				}

				if (!sameNameFound) {
					// Create the new directory and add it to the list of directories in the current directory along the path
					startDir->dirs.push_back(Directory(dirName, startDir));
					// Move up the current directory to the one we just created
					startDir = &startDir->dirs.back();
				}
			}

			i++;
		}
		// If last directory wasn't a unique name, return error
		if (sameNameFound) {
			result = 2;
		}
	}

	return result;
}


/* Please insert your code */

void FileSystem::format()
{
	this->mMemblockDevice.reset();
	
}

int FileSystem::removeFolder(std::string dirPath)
{
	int returnValue = 0;
	Directory* currDir;

	if (dirPath == "")
	{
		returnValue = -1;
	}
	else if (dirPath.back() == '/')
	{
		returnValue = 1;
	}
	else
	{
		int i = 0;
		bool checker = false;
		std::string latest;
	
		if (dirPath[0] == '/') //Check if it's an absolut path
		{
			currDir = &this->rootDir;
			i = 1;
		}
		else 
		{
			currDir = this->currentDir;
		}
		
		while (i < dirPath.size())
		{
			std::string nextDir;
			if (dirPath[i] == '/')
			{
				i++;
			}
			
			while (dirPath[i] != '/' && i < dirPath.size())
			{
				nextDir += dirPath[i];
				i++;
			}
			latest = nextDir;
			if (nextDir == "..")
			{
				currDir = currDir->parent;
			}
			else
			{
				for (std::list<Directory>::iterator j=currDir->dirs.begin(); j != currDir->dirs.end(); j++)
				{
					if (j->name == nextDir)
					{
						currDir = &(*j);
						checker = true;
						break;
					}
				} 
				if (!checker)
				{
					returnValue = -1;
				}
			}
		}
		currDir = currDir->parent;
		for (std::list<Directory>::iterator j = currDir->dirs.begin(); j != currDir->dirs.end(); j++)
		{
			if (j->name == latest)
			{
				j = currDir->dirs.erase(j);
				break;
			}
		}			
	}

	return 0;
}
