#include "filesystem.h"

int FileSystem::insertIntoString(std::string& moddedString, int moddedStartPos, std::string content)
{
	int result = -1;

	if (moddedStartPos + content.size() <= moddedString.size()) {
		result = 0;
		for (int i = 0; i < content.size(); i++) {
			moddedString[moddedStartPos + i] = content[i];
		}
	}

	return result;
}

std::string FileSystem::listDir(Directory* dir)
{
	std::string list;

	for (std::list<Directory>::iterator it = dir->dirs.begin(); it != dir->dirs.end(); it++) {
		list += it->name + "\n";
	}

	for (std::list<File>::iterator it = dir->files.begin(); it != dir->files.end(); it++) {
		list += it->name + " ";

		if (this->mMemblockDevice.readBlock(it->blockPositions.back()).getNulTerminatorPos() != -1) {
			// Assuming that all the blocks up until the last block are maxed out: ((Number of blocks - last block) * size of a block) + size until nul terminator of last block
			int nrOfBytes = (int)(((it->blockPositions.size() - 1) * 512) + this->mMemblockDevice.readBlock(it->blockPositions.back()).getNulTerminatorPos());

			list += std::to_string(nrOfBytes);
		}

		list += "bytes\n";
	}

	return list;
}

int FileSystem::parsePathAndDir(std::string& path, Directory** pathDir)
{
	int result = 0;

	if (path != "") {
		// Absolute path
		if (path.front() == '/') {
			*pathDir = &this->rootDir;
			// Removes the '/' at the front since we don't want it anymore
			path = path.substr(1, path.size());
		}
		// Relative path
		else {
			*pathDir = this->workingDir;
		}
	}
	else {
		result = -1;
	}

	return result;
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

FileSystem::FileSystem() : currentPath(""), workingDir(&rootDir), rootDir("root", nullptr) {

}

FileSystem::~FileSystem() {

}

int FileSystem::createFile(std::string name, std::string content)
{
	int result = 0;
	Directory* pathDir = nullptr;
	std::string path = name;
	std::string fileName;
	
	if (parsePathAndDir(path, &pathDir) == 0) {

		this->getFileNameFromPath(path, fileName);

		this->getDirFromPath(path, &pathDir, true);

		// Calculate how many complete blocks are needed. We add 1 since it always rounds downwards.
		int numBlocks = (content.size() / 512) + 1;
		std::vector<std::string> parsedContent(numBlocks);

		for (int i = 0; i < numBlocks - 1; i++) {
			parsedContent[i].resize(512);
			// Insert the content in blocks of 512
			parsedContent[i] = content.substr(0, 512);
			// Remove what we just added
			content = content.substr(512);
		}
		parsedContent[numBlocks - 1].resize(512);
		// Insert the odd left overs and put a nul terminator at the end
		if (this->insertIntoString(parsedContent[numBlocks - 1], 0, content) == -1) {
			result = 1;
		}

		std::vector<int> blockPositions(numBlocks);

		// Get the first available blocks required to store the content
		for (int i = 0; i < numBlocks; i++) {
			if (this->mMemblockDevice.getFirstAvailableBlock() != -1) {
				blockPositions[i] = this->mMemblockDevice.getFirstAvailableBlock();
			}
			else {
				result = 2;
				break;
			}
		}

		if (result == 0) {
			// Try and write the content to the blocks
			for (int i = 0; i < numBlocks; i++) {
				if (this->mMemblockDevice.writeBlock(blockPositions[i], parsedContent[i]) != 1) {
					// If it should fail to write one after having already written at least one, clear the ones that have been written to before breaking out
					for (int k = 0; k < i; k++) {
						this->mMemblockDevice.clearBlock(blockPositions[k]);
					}

					result = 3;
					break;
				}
			}
		}

		if (result == 0) {
			// Add the file to the dir
			pathDir->files.push_back(File(fileName, blockPositions));
		}
	}
	else {
		result = -1;
	}

	return result;
}

int FileSystem::createDirectory(std::string name)
{
	int result = 0;
	Directory* pathDir = nullptr;
	std::string path = name;
	std::string dirName;

	if(parsePathAndDir(path, &pathDir) == 0) {
		// Split the new dir from the rest of the path
		this->getFileNameFromPath(path, dirName);

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
	else {
		result = -1;
	}

	return result;
}

std::string FileSystem::list(std::string name)
{
	int result = 0;
	std::string list;
	std::string path = name;
	std::string dirName;
	Directory* pathDir;

	// Try parsing the path is there is one
	if (this->parsePathAndDir(path, &pathDir) == 0) {
		// Get the dir that is suppose to hold the new dir
		result = this->getDirFromPath(path, &pathDir, false);

		switch (result)
		{
		case 0:
			list = this->listDir(pathDir);
			break;
		case 2:
			list = "Directory doesn't exist.";
			break;
		}
	}
	// If a path wasnt provided, then assume working dir
	else {
		list = this->listDir(this->workingDir);
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
