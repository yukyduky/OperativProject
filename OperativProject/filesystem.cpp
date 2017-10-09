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

	// Iterate through and look for a dir with the correct dirPath
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
	Directory* currentDir = nullptr;
	bool sameNameFound = false;
	int i = 0;

	std::string currentPath = path;
	std::string dirName;

	// Get next dir dirPath and then look for the next dir from that
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
		// Using substr to split the path from the dirPath
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

int FileSystem::createFile(std::string dirPath, std::string content)
{
	int result = 0;
	Directory* pathDir = nullptr;
	std::string path = dirPath;
	std::string fileName;
	
	if (parsePathAndDir(path, &pathDir) == 0) {

		this->getFileNameFromPath(path, fileName);

		this->getDirFromPath(path, &pathDir, true);

		for (std::list<File>::iterator it = pathDir->files.begin(); it != pathDir->files.end(); it++) {
			if (it->name == fileName) {
				result = 4;
				break;
			}
		}

		if (result == 0) {
			// Calculate how many complete blocks are needed. We add 1 since it always rounds downwards.
			int numBlocks = (content.size() / 512) + 1;
			std::vector<std::string> parsedContent(numBlocks);

			for (int i = 0; i < numBlocks - 1; i++) {
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

			// Get the first available blocks of the required amount
			std::vector<int> blockPositions = this->mMemblockDevice.getFirstAvailableBlocks(numBlocks);

			if (blockPositions.back() != -1) {
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

				if (result == 0) {
					// Add the file to the dir
					pathDir->files.push_back(File(fileName, blockPositions));
				}
			}
			else {
				result = 2;
			}
		}
		else {
			result = -1;
		}
	}

	return result;
}

int FileSystem::appendFileToFile(std::string dirPath1, std::string dirPath2)
{
	int result = -1;
	Directory* pathDir1 = nullptr;
	Directory* pathDir2 = nullptr;
	std::string path1 = dirPath1;
	std::string path2 = dirPath2;
	std::string fileName1;
	std::string fileName2;

	if (parsePathAndDir(path1, &pathDir1) == 0 && parsePathAndDir(path2, &pathDir2) == 0) {
		result = 0;

		if (this->getFileNameFromPath(path1, fileName1) != -1 && this->getFileNameFromPath(path2, fileName2) != 0) {
			result = 4;
		}

		if (this->getDirFromPath(path1, &pathDir1, false) != 0 && this->getDirFromPath(path2, &pathDir2, false != 0)) {
			result = 1;
		}

		if (result == 0) {
			bool finished = false;
			// Iterate through until right file is found. Clear the blocks that the file occupies and remove it from the list.
			for (std::list<File>::iterator it1 = pathDir1->files.begin(); it1 != pathDir1->files.end(); it1++) {
				if (it1->name == fileName1) {
					for (std::list<File>::iterator it2 = pathDir2->files.begin(); it2 != pathDir2->files.end(); it2++) {
						if (it2->name == fileName2) {
							std::string content = this->mMemblockDevice.readBlock(it2->blockPositions[0]).toString();
							int numBlocks = it2->blockPositions.size();
							for (int i = 1; i < numBlocks; i++) {
								std::string tmp = this->mMemblockDevice.readBlock(it2->blockPositions[i]).toString();
								for (int k = 0; k < tmp.size(); k++) {
									content += tmp[k];
								}
							}

							// Get the last content of the file
							std::string lastBlock = mMemblockDevice.readBlock(it1->blockPositions.back()).toString();
							std::string parsedLastBlock;
							parsedLastBlock.resize(512);

							for (int i = 0; i < lastBlock.size(); i++) {
								parsedLastBlock[i] = lastBlock[i];
							}

							// Get the position of the end of that content
							int contentStartPos = lastBlock.size();

							for (int i = 0; i < content.size(); i++) {
								parsedLastBlock[contentStartPos + i] += content[i];
							}
							// Overwrite the last block with the now topped off one
							if (this->mMemblockDevice.writeBlock(it1->blockPositions.back(), parsedLastBlock) == 1 && content.size() > 512) {
								// Remove the part that we put in the lastBlock
								content = content.substr(512 - contentStartPos, content.size());
								// Get the amount of blocks left to write
								numBlocks = (content.size() / 512) + 1;
								std::vector<std::string> parsedContent(numBlocks);

								for (int i = 1; i < numBlocks - 1; i++) {
									// Insert the content in blocks of 512
									parsedContent[i] = content.substr(0, 512);
									// Remove what we just added
									content = content.substr(512);
								}
								parsedContent[numBlocks - 1].resize(512);
								// Insert the odd left overs and put a nul terminator at the end
								if (this->insertIntoString(parsedContent[numBlocks - 1], 0, content) != -1) {
									// Get the first available blocks of the required amount
									std::vector<int> blockPositions = this->mMemblockDevice.getFirstAvailableBlocks(numBlocks);

									if (blockPositions.back() != -1) {
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
										// Add the new block positions to the first files block positions
										for (int i = 0; i < blockPositions.size(); i++) {
											it1->blockPositions.push_back(blockPositions[i]);
										}
									}
								}
								else {
									result = 2;
								}

								finished = true;
								break;
							}
						}
					}
				}
				
				if (finished) {
					break;
				}
			}
		}
	}

	return result;
}

int FileSystem::appendContentToFile(std::string dirPath, std::string content)
{
	int result = -1;
	Directory* pathDir = nullptr;
	std::string path = dirPath;
	std::string fileName;

	if (parsePathAndDir(path, &pathDir) == 0) {
		result = 0;

		this->getFileNameFromPath(path, fileName);

		if (this->getDirFromPath(path, &pathDir, false) != 0) {
			result = 1;
		}

		if (result == 0) {
			// Iterate through until right file is found. Clear the blocks that the file occupies and remove it from the list.
			for (std::list<File>::iterator it = pathDir->files.begin(); it != pathDir->files.end(); it++) {
				if (it->name == fileName) {
					// Calculate how many complete blocks are needed. We add 1 since it always rounds downwards.
					int numBlocks = (content.size() / 512) + 1;
					std::vector<std::string> parsedContent(numBlocks);
					// Get the last content of the file
					parsedContent[0] = mMemblockDevice.readBlock(it->blockPositions.back()).toString();
					// Get the position of the end of that content
					int contentStartPos = parsedContent[0].size();

					// Insert content until the block is filled
					if (this->insertIntoString(parsedContent[0], contentStartPos, content) == -1) {
						result = -1;
					}

					if (result == 0) {
						for (int i = 1; i < numBlocks - 1; i++) {
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
					}

					break;
				}
			}
		}
	}

	return result;
}

int FileSystem::createDirectory(std::string dirPath)
{
	int result = 0;
	Directory* pathDir = nullptr;
	std::string path = dirPath;
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
			// If it can't go to the next dir, then that means that no other dir exists with the same dirPath as the one we want to create
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

int FileSystem::removeFile(std::string dirPath)
{
	int result = -1;
	Directory* pathDir = nullptr;
	std::string path = dirPath;
	std::string fileName;

	if (parsePathAndDir(path, &pathDir) == 0) {
		result = 0;

		this->getFileNameFromPath(path, fileName);

		if (this->getDirFromPath(path, &pathDir, false) != 0) {
			result = 1;
		}

		if (result == 0) {
			// Iterate through until right file is found. Clear the blocks that the file occupies and remove it from the list.
			for (std::list<File>::iterator it = pathDir->files.begin(); it != pathDir->files.end(); it++) {
				if (it->name == fileName) {
					for (int i = 0; i < it->blockPositions.size(); i++) {
						this->mMemblockDevice.clearBlock(it->blockPositions[i]);
					}
					pathDir->files.erase(it);
					break;
				}
			}
		}
	}

	return result;
}

int FileSystem::removeFolder(std::string dirPath)
{
	int returnValue = 0;
	Directory* currDir;
	std::string toRemove;

	returnValue = parsePathAndDir(dirPath, &currDir);
	if (returnValue == 0)
	{
		returnValue = getFileNameFromPath(dirPath, toRemove);
		if (returnValue == 0)
		{
			returnValue = getDirFromPath(dirPath, &currDir, false);
			if (returnValue == 0)
			{
				for (std::list<Directory>::iterator j = currDir->dirs.begin(); j != currDir->dirs.end(); j++)
				{
					if (j->name == toRemove)
					{
						j = currDir->dirs.erase(j);
						returnValue = 0;
						break;
					}
					else
					{
						returnValue = -1;
					}
				}
			}
		}
	}
	return returnValue;
}

int FileSystem::changeDirectory(std::string dirPath)
{
	int returnValue = 0;

	if (dirPath == "")
	{
		this->workingDir = &this->rootDir;
	}
	else
	{
		Directory* currDir;
		returnValue = parsePathAndDir(dirPath, &currDir);
		if (returnValue == 0)
		{
			returnValue = getDirFromPath(dirPath, &currDir, false);
			if (returnValue == 0)
			{
				this->workingDir = currDir;
			}
			else
			{
				returnValue = -1;
			}
		}
	}

	return returnValue;
}

std::string FileSystem::list(std::string dirPath)
{
	int result = 0;
	std::string list;
	std::string path = dirPath;
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

	// Get the dirPath of the current dir
	path += currentDir->name;

	// While currentDir has a parent, continue
	while (currentDir->parent != nullptr) {
		// Set currentDir to its parent
		currentDir = currentDir->parent;
		// Get the dirPath of currentDir and add the rest of the path on top
		path = currentDir->name + '/' + path;		
	}

	path = '/' + path;

	return path;
}

std::string FileSystem::printFile(std::string dirPath)
{
	std::string returnValue;
	std::string fileToPrint;
	int checker;
	Directory* currDir;
	File* theFile;
	Block currBlock;

	checker = parsePathAndDir(dirPath, &currDir);
	if (checker == 0)
	{
		checker = getFileNameFromPath(dirPath, fileToPrint);
		if (checker == 0)
		{
			checker = getDirFromPath(dirPath, &currDir, false);
			if (checker == 0)
			{
				for (std::list<File>::iterator j = currDir->files.begin(); j != currDir->files.end(); j++)
				{
					if (j->name == fileToPrint)
					{
						theFile = &(*j);
						returnValue = theFile->name + '\n';
						for (int i = 0; i < theFile->blockPositions.size(); i++)
						{
							currBlock = this->mMemblockDevice.readBlock(theFile->blockPositions[i]);
							returnValue += currBlock.toString();
						}
						break;
					}
					else
					{
						returnValue = "Invalid path\n";
					}
				}
			}
			else
			{
				returnValue = "Invalid path\n";
			}
		}
	}

	return returnValue;
}