#include "filesystem.h"

FileSystem::FileSystem() : currentPath(""), workingDir(&rootDir), rootDir("root", nullptr) {

}

FileSystem::~FileSystem() {

}

void FileSystem::format()
{
	this->mMemblockDevice.reset();
	this->rootDir.dirs.clear();
	this->rootDir.files.clear();
}

int FileSystem::createDirectory(std::string name)
{
	int result = 0;
	Directory* currentDir = nullptr;
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
			currentDir = &this->rootDir;
		}
		else {
			currentDir = this->workingDir;
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
				if (currentDir->parent != nullptr) {
					currentDir = currentDir->parent;
				}
			}
			// If there are no directories in the current directory, then there can't be another directory with the same name
			else if (currentDir->dirs.size() == 0) {
				// Create the new directory and add it to the list of directories in the current directory along the path
				currentDir->dirs.push_back(Directory(dirName, currentDir));
				// Move up the current directory to the one we just created
				currentDir = &currentDir->dirs.back();
			}
			// Else we need to iterate through them to check if there exists one with the same name already and use that one instead of creating a new one
			else
			{
				sameNameFound = false;

				for (std::list<Directory>::iterator it = currentDir->dirs.begin(); it != currentDir->dirs.end(); it++) {
					if (it->name == dirName) {
						sameNameFound = true;
						currentDir = &(*it);
						break;
					}
				}

				if (!sameNameFound) {
					// Create the new directory and add it to the list of directories in the current directory along the path
					currentDir->dirs.push_back(Directory(dirName, currentDir));
					// Move up the current directory to the one we just created
					currentDir = &currentDir->dirs.back();
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

int FileSystem::removeFolder(std::string dirPath)
{
	int returnValue = 0;
	Directory* currDir;

	if (dirPath == "") //check if there is a path
	{
		returnValue = -1;
	}
	else if (dirPath.back() == '/') //check if the path ends with '/' and therefore is invalide
	{
		returnValue = 1;
	}
	else
	{
		int i = 0;
		bool checker = false;
		std::string latest;
	
		if (dirPath[0] == '/') //Check if it's an absolut path and if it is we set the starting directory to the root directory.
		{
			currDir = &this->rootDir;
			i = 1;
		}
		else 
		{
			currDir = this->workingDir; //set starting directory to current directory if the given path is realitve
		}
		
		while (i < dirPath.size())
		{
			std::string nextDir;
			if (dirPath[i] == '/') //removes the '/' between directories
			{
				i++;
			}
			
			while (dirPath[i] != '/' && i < dirPath.size()) //saves the next directory to a string
			{
				nextDir += dirPath[i];
				i++;
			}
			latest = nextDir; //saves the name of the latest directory for future use.
			if (nextDir == "..") // if the next directory name is ".." we move the currDir to the parent. 
			{
				currDir = currDir->parent;
			}
			else
			{
				//Search through the current directories children for the next directory to step into.
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
		//We now stand inside the folder we want to remove, we therefore step to it's parent and remove the folder 
		// we just stood in.
		if (checker)
		{
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
	}

	return returnValue;
}

int FileSystem::changeDirectory(std::string dirPath)
{
	int returnValue = 0;

	Directory* currDir;

	if (dirPath == "")
	{
		this->workingDir = &this->rootDir;
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

		if (dirPath[0] == '/') //Check if it's an absolut path and if it is we set the starting directory to the root directory.
		{
			currDir = &this->rootDir;
			i = 1;
		}
		else
		{
			currDir = this->workingDir; //set starting directory to current directory if the given path is realitve
		}

		while (i < dirPath.size() && returnValue != -1)
		{
			checker = false;
			std::string nextDir;
			if (dirPath[i] == '/') //removes the '/' between directories
			{
				i++;
			}

			while (dirPath[i] != '/' && i < dirPath.size()) //saves the next directory to a string
			{
				nextDir += dirPath[i];
				i++;
			}
			latest = nextDir; //saves the name of the latest directory for future use.
			if (nextDir == "..") // if the next directory name is ".." we move the currDir to the parent. 
			{
				currDir = currDir->parent;
				checker = true;
			}
			else
			{
				//Search through the current directories children for the next directory to step into.
				for (std::list<Directory>::iterator j = currDir->dirs.begin(); j != currDir->dirs.end(); j++)
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
		if (checker)
		{
			this->workingDir = currDir;
		}
	}
	return returnValue;
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

std::string FileSystem::printFile(std::string dirPath)
{
	std::string returnValue;

	Directory* currDir;

	if (dirPath == "" || dirPath.back() == '/')
	{
		returnValue = "Invalid path.";
	}
	else
	{
		int i = 0;
		bool checker = false;
		std::string latest;

		if (dirPath[0] == '/') //Check if it's an absolut path and if it is we set the starting directory to the root directory.
		{
			currDir = &this->rootDir;
			i = 1;
		}
		else
		{
			currDir = this->workingDir; //set starting directory to current directory if the given path is realitve
		}

		while (i < dirPath.size())
		{
			checker = false;
			std::string nextDir;
			if (dirPath[i] == '/') //removes the '/' between directories
			{
				i++;
			}

			while (dirPath[i] != '/' && i < dirPath.size()) //saves the next directory to a string
			{
				nextDir += dirPath[i];
				i++;
			}
			if (dirPath[i + 1] < dirPath.size())
			{
				latest = nextDir; //saves the name of the latest directory for future use.
				if (nextDir == "..") // if the next directory name is ".." we move the currDir to the parent. 
				{
					currDir = currDir->parent;
					checker = true;
				}
				else
				{
					//Search through the current directory for the next directory to step into.
					for (std::list<Directory>::iterator j = currDir->dirs.begin(); j != currDir->dirs.end(); j++)
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
						returnValue = "Invalid path.";
					}
				}
			}
			else
			{
				//Search through the current directories files for the one we want to print.
				for (std::list<File>::iterator j = currDir->files.begin(); j != currDir->files.end(); j++)
				{
					if (j->name == nextDir)
					{
						returnValue += j->name + "\n";
						returnValue += j->contains + "\n";
						break;
					}
				}			
			}
		}
	}

	return returnValue;
}