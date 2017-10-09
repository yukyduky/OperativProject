#ifndef MEMBLOCKDEVICE_H
#define MEMBLOCKDEVICE_H
#include "blockdevice.h"
#include <array>

class MemBlockDevice: public BlockDevice
{
private:
	std::array<bool, 250> occupiedBlock;

public:
    MemBlockDevice(int nrOfBlocks = 250);
    MemBlockDevice(const MemBlockDevice &other);

    ~MemBlockDevice();

    /* Operators */
    MemBlockDevice& operator=(const MemBlockDevice &other);
    Block &operator[] (int index) const;

    /* Returns amount of free blocks */
    int spaceLeft() const;

    /* Writes a block */
    int writeBlock(int blockNr, const std::vector<char> &vec);
    int writeBlock(int blockNr, const std::string &strBlock);
    int writeBlock(int blockNr, const char cArr[]);     // Use with care, make sure that cArr has at least the same dimension as block

    /* Reads a block */
    Block readBlock(int blockNr) const;

	/* Clears a block*/
	int clearBlock(int blockNr);

    /* Resets all the blocks */
    void reset();

    /* Return the size */
    int size() const;
	std::vector<int> MemBlockDevice::getFirstAvailableBlocks(int nrOfBlocks);
	void resetMemBlock();
};

#endif // MEMBLOCKDEVICE_H
