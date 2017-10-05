#include "filesystem.h"

FileSystem::FileSystem() : currentDir(rootDir), currentPath("") {
	this->rootDir->parent = NULL;

}

FileSystem::~FileSystem() {

}

int FileSystem::createDirectory(std::string name)
{
	Directory* startDir = nullptr;
	int i = 0;

	// Fails if no argument is given
	if (name == "") {
		return -1;
	}
	else if (name.back() == '/') {
		return 1;
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
				std::list<Directory>::iterator it = startDir->dirs.begin();

				// Find the right element
				while (it->name != dirName && it != it->dirs.end()) {
					it++;
				}

				// If loop ended before the end of the list, then we got a directory with the same name
				if (it != it->dirs.end()) {
					startDir = &(*it);
				}
				// We miss the last element in the loop so we have to do a separate check for it
				else if (it->dirs.end()->name == dirName) {
					startDir = &(*it);
				}
				// Else the directory wasn't found and we create one and add it to the list
				else {
					// Create the new directory and add it to the list of directories in the current directory along the path
					startDir->dirs.push_back(Directory(dirName, startDir));
					// Move up the current directory to the one we just created
					startDir = &startDir->dirs.back();
				}
			}

			i++;
		}
	}

	return 0;
}


/* Please insert your code */

void FileSystem::format()
{
	this->mMemblockDevice.reset();
	
}

int FileSystem::removeFolder(std::string dirPath)
{
	Directory* currDir;

	if (dirPath == "")
	{
		return -1;
	}
	else
	{
		int i = 0;
		bool checker = false;
		std::string latest;
	
		if (dirPath[0] == '/') //Check if it's an absolut path
		{
			currDir = this->rootDir;
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
					}
				} 
				if (!checker)
				{
					return -1;
				}
			}
		}
		currDir = currDir->parent;
		for (std::list<Directory>::iterator j = currDir->dirs.begin(); j != currDir->dirs.end(); j++)
		{
			if (j->name == latest)
			{
				j = currDir->dirs.erase(j);
			}
		}			
	}

	return 1;
}
