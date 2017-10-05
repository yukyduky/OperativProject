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

std::string FileSystem::listDir()
{
	std::string list;

	for (std::list<Directory>::iterator it = this->currentDir->dirs.begin(); it != this->currentDir->dirs.end(); it++) {
		list += it->name + "\n";
	}

	for (std::list<File>::iterator it = this->currentDir->files.begin(); it != this->currentDir->files.end(); it++) {
		list += it->name + " ";
		list += it->contains.size();
		list += " bytes\n";
	}

	return list;
}
