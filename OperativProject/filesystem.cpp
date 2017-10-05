#include "filesystem.h"

FileSystem::FileSystem() : currentDir(rootDir), currentPath("") {

}

FileSystem::~FileSystem() {

}

int FileSystem::createDirectory(std::string name)
{
	Directory* startDir = nullptr;

	if (name == "") {
		return -1;
	}
	else {
		if (name[0] == '/') {
			int i = 1;

			while (i < name.size()) {
				std::string dirName;

				while (name[i] != '/' && i < name.size()) {
					dirName += name[i];
					i++;
				}


			}			
		}
		else {

		}
	}
}


/* Please insert your code */