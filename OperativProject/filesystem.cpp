#include "filesystem.h"

FileSystem::FileSystem() : currentDir(rootDir), currentPath("") {
	this->rootDir->parent = NULL;

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
