// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    	numBytes = fileSize;
    	numSectors  = divRoundUp(fileSize, SectorSize);
    	int direct;
    	int indirect;
    	int doubleIndirect;
    	
    	printf("File Size = %i\n", numBytes);
    	
    	/*
    	**	For Files <= MaxFileSize1 (15872), use 62 direct pointers, 0 indirect pointers, and 0 double indirect pointers
    	**	For Files > MaxFileSize1 (15872) and <= MaxFileSize2 (32000), use 61 direct pointers, 1 indirect pointer, and 0 double indirect pointers
    	**	For Files > MaxFileSize2, use 60 direct pointers, 1 indirect pointer, and 1 double indirect pointer
    	*/
    	 if (freeMap->NumClear() < numSectors){
    		printf("NOT ENOUGH SPACE FOR FILE\n");
			return FALSE;		// not enough space
		}
    	if(fileSize <= MaxFileSize1)
    	{
    		direct = numSectors;
    		indirect = 0;
    		doubleIndirect = 0;
    		printf("Requires %i Sector(s) (including %i pointer blocks).\n\n", numSectors, indirect + doubleIndirect);
    		
			printf("Consumed Sectors (Before allocation) - ");
			freeMap->Print();
			printf("\n");
	
   			for (int i = 0; i < direct; i++)
				dataSectors[i] = freeMap->Find();
    	}
    	else if (fileSize > MaxFileSize1 && fileSize <= MaxFileSize2)
    	{
    		direct = NumDirect - 1;
    		indirect = 1;
    		doubleIndirect = 0;
    		printf("Requires %i Sector(s) (including %i pointer blocks).\n\n", numSectors, indirect + doubleIndirect);
			
			printf("Consumed Sectors (Before allocation) - ");
			freeMap->Print();
			printf("\n");
	
   			for (int i = 0; i < direct; i++)
   			{
				dataSectors[i] = freeMap->Find();
			}	
			
			//single indirect ptr
			dataSectors[direct] = freeMap->Find();
			int singleIndirects = numSectors - direct;
			
			//pointers in the sector
			int directPtrs_of_sector[SectorSize / sizeof(int)] = {0};
			
			for(int i = 0; i < singleIndirects; i++)
			{
				directPtrs_of_sector[i] = freeMap->Find();
			}
			
			synchDisk->WriteSector(dataSectors[direct], (char*)directPtrs_of_sector);
			
    	}
    	else
    	{
    	    direct = NumDirect - 2;
    		indirect = 1;
    		doubleIndirect = 1;
    		printf("Requires %i Sector(s) (including %i pointer blocks).\n\n", numSectors, indirect + doubleIndirect);
			
			printf("Consumed Sectors (Before allocation) - ");
			freeMap->Print();
			printf("\n");
	
			for (int i = 0; i < direct; i++)
   			{
				dataSectors[i] = freeMap->Find();
			}	
			//single indirect ptr
			
			dataSectors[direct] = freeMap->Find();
			
			//pointers in the sector
			int directPtrs_of_sector[SectorSize / sizeof(int)] = {0};
			
			for(int i = 0; i < SectorSize / sizeof(int); i++)
			{
				directPtrs_of_sector[i] = freeMap->Find();
			}
			
			synchDisk->WriteSector(dataSectors[direct], (char*)directPtrs_of_sector);
			
			
			
			//double indirect ptr
			
			int ptrsPerSector = SectorSize / sizeof(int);
    		int directOffset = (NumDirect - 2) * SectorSize;
    	
    		int indirectOffset = (ptrsPerSector * SectorSize);
    		
    		int bytesPerSingleIndirectPtr = ptrsPerSector * SectorSize;
    		
			int singleIndirectPtrs = (numBytes - directOffset - indirectOffset) / bytesPerSingleIndirectPtr;
			
			
			
			
			
			
			
			
			dataSectors[direct+1] = freeMap->Find();
			
			int singlePtrs_of_sector[SectorSize / sizeof(int)] = {0};
			for(int i = 0; i < SectorSize / sizeof(int); i++)
			{
				singlePtrs_of_sector[i] = freeMap->Find();			
			}
			synchDisk->WriteSector(dataSectors[direct+1], (char*)singlePtrs_of_sector);
			
			for(int i = 0; i < SectorSize / sizeof(int); i++)
			{
				int doublePtrs_of_sector[SectorSize / sizeof(int)] = {0};
				for(int j = 0; j < SectorSize / sizeof(int); j++)
				{
					doublePtrs_of_sector[j] = freeMap->Find();
				}
				synchDisk->WriteSector(singlePtrs_of_sector[i], (char*)doublePtrs_of_sector);
				
			}
			
			
			
    	}
    

	printf("Used %i direct pointers\n", direct);
	printf("Used %i indirect pointers\n", indirect);
	printf("Used %i double indirect pointers\n\n", doubleIndirect);
	
	printf("Consumed Sectors (After allocation) - ");
	freeMap->Print();
	printf("\n");
	
    	return TRUE;
    	
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

void 
FileHeader::Deallocate(BitMap *freeMap)
{
	//Show the bitmap before deallocation of data blocks for the file
	printf("*****FileHeader Deconstructor is invoked!!\n");
	
	printf("Consumed Sectors (Before deallocation) - ");
	freeMap->Print();
	printf("\n");
	if(numBytes <= MaxFileSize1)
    {
    	for (int i = 0; i < numSectors; i++) 
    	{
			ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
			freeMap->Clear((int) dataSectors[i]);
			dataSectors[i] = 0;
    	}
    }
    else if (numBytes > MaxFileSize1 && numBytes <= MaxFileSize2)
    {
    
        int readSector[(SectorSize / sizeof(int))];
    	synchDisk->ReadSector(dataSectors[NumDirect - 1], (char*)readSector);
    	for (int i = 0; i <= numSectors-NumDirect; i++) 
    	{
			ASSERT(freeMap->Test((int) readSector[i]));  // ought to be marked!
			freeMap->Clear((int) readSector[i]);
			readSector[i] = 0;
    	}
    	for (int i = 0; i < NumDirect; i++) 
    	{
			ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
			freeMap->Clear((int) dataSectors[i]);
			dataSectors[i] = 0;
    	}

    	
    	
    }
    
    	//Show the bitmap after deallocation of the data blocks for the file
    	printf("Consumed Sectors (After deallocation) - ");
	freeMap->Print();
	printf("\n");
	
	printf("All Data Sectors have been released.\n\n");
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
	if(numBytes <= MaxFileSize1)
	{
		return(dataSectors[offset / SectorSize]);
    }
    else if (numBytes > MaxFileSize1 && numBytes <= MaxFileSize2) 
    {
    	int directOffset = (NumDirect - 1) * SectorSize;
    
    	if(offset < directOffset) 
    	{
    		return(dataSectors[offset / SectorSize]);
    	}
  		else 
  		{
  			int ptrsPerSector = SectorSize / sizeof(int);
  			int readSector[ptrsPerSector];
  			
  			synchDisk->ReadSector(dataSectors[NumDirect - 1], (char*)readSector);
  			
  			return readSector[ (offset - directOffset) / SectorSize ];
  			
  		}
  		
    }
    else 
    {
        int ptrsPerSector = SectorSize / sizeof(int);
    	int directOffset = (NumDirect - 2) * SectorSize;
    	
    	int indirectOffset = (ptrsPerSector * SectorSize);
    	
    	if(offset < directOffset) 
    	{
    		return(dataSectors[offset / SectorSize]);
    	}
    	else if (offset > directOffset && offset < directOffset + indirectOffset)
  		{
  			int readSector[ptrsPerSector];
  			
  			synchDisk->ReadSector(dataSectors[NumDirect - 2], (char*)readSector);
  			
  			return readSector[ (offset - directOffset) / SectorSize ];
  			
  		}
  		else
  		{
  			int readSector[ptrsPerSector];
  			
  			synchDisk->ReadSector(dataSectors[NumDirect - 1], (char*)readSector);
  			
  			int doubleoffsetloc = (offset - directOffset - indirectOffset) / (SectorSize * ptrsPerSector);
  			
  			int readSector2[ptrsPerSector];
  			synchDisk->ReadSector(readSector[doubleoffsetloc], (char*)readSector2);
  			
  			return readSector2[ (offset - directOffset - indirectOffset - (indirectOffset*doubleoffsetloc)) / SectorSize ];
  			
  			
  		}
    }
    	
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    for (i = 0; i < numSectors; i++)
	printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
	synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
	    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
		printf("%c", data[j]);
            else
		printf("\\%x", (unsigned char)data[j]);
	}
        printf("\n"); 
    }
    delete [] data;
}
