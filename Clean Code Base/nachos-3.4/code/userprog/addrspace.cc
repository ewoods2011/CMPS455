// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"

    NoffHeader noffH;
    unsigned int i, size, newsize, pAddr, counter;
    char FileName[32];


//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *theExecutable)
{

	space = false;
	pageToInit = 0;
	executable = theExecutable;
	myPageList = new List();

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
	newsize = sizeof(noffH) + size;
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

	//Change this to reference the bitmap for free pages
	//instead of total amount of pages
	//This requires a global bitmap instance
	
	//Don't need to find start page anymore
	/*
	counter = 0;
	
	for(i = 0; i < NumPhysPages && counter < numPages; i++)
	{
		if(!memMap->Test(i))
		{
			if(counter == 0)
				startPage = i;	//startPage is a class data member
								//Should it be public or private? (Currently private)
			counter++;
		}
		else
			counter = 0;
	}
	*/
	
	DEBUG('a', "%i contiguous blocks found for %i pages\n", counter, numPages);
	counter = 0;
	//TODO - Insert bitmap function to see if bitmap is full
	//If no memory available, terminate
	/*
	if(counter < numPages)
	{
		printf("Not enough contiguous memory for new process; terminating!.\n");
		currentThread->killNewChild = true;
		return;
	}
	*/

	//If we get past the if statement, then there was sufficient space
	space = true;

	//This safeguards against the loop terminating due to reaching
	//the end of the bitmap but no contiguous space being available

    DEBUG('a', "Initializing address space, numPages=%d, size=%d\n", 
					numPages, size);
// first, set up the translation 
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
		pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
		//pageTable[i].physicalPage = i;	//Replace with pageTable[i].physicalPage = i + startPage;
		pageTable[i].physicalPage = i + startPage;
		pageTable[i].valid = FALSE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
						// a separate page, we could set its 
						// pages to be read-only

		//Take the global bitmap and set the relevant chunks
		//to indicate that the memory is in use
		//memMap->Mark(i + startPage);
    }
    pAddr = startPage * PageSize;
	
	memMap->Print();	// Useful!

// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
//    bzero(machine->mainMemory, size); rm for Solaris
	//Edited version adds startPage * PageSize to the address. Hopefully this is proper.
	//Either way, it appears to zero out only however much memory is needed,
	//so zeroing out all memory doesn't seem to be an issue. - Devin
	

	
   // memset(machine->mainMemory + pAddr, 0, size);

/*
// then, copy in the code and data segments into memory
//Change these too since they assume virtual page = physical page
	  //Fix this by adding startPage times page size as an offset
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr + (startPage * PageSize), noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr + pAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr + (startPage * PageSize), noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr + pAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
*/
}


void
AddrSpace::Paging(int vpn)
{
	
	vpn = vpn / PageSize;
	
	//Find the first free page in memory
	counter = 0;
	for(i = 0; i < NumPhysPages && counter < numPages; i++)
	{
		if(!memMap->Test(i))
		{
			if(counter == 0)
				startPage = i;	//startPage is a class data member
								//Should it be public or private? (Currently private)
			counter++;
		}
		else
			counter = 0;
	}
	memMap->Mark(startPage);
	
	IPT[startPage] = currentThread;
	
	myPageList->Append((void*)startPage);
	
	memMap->Print();
	pAddr = startPage * PageSize;
	
	
    //PrintMainMem();
  	//  printf("AFTER READ AT\n\n");
  
  	//FIFO
  	if(pageRepChoice == 1)
  	{
/*  		if(pageList->getSize() == NumPhysPages)
  			pageList->Remove();	 */
  			
  		//Get the address to the thread that has the page we are removing
  		Thread *threadNeeded = IPT[startPage];
  		//Set the page from the thread's page table to invalid
  		threadNeeded->space->pageTable[vpn].valid = FALSE;
  		IPT[startPage] = NULL;
  		//Zero out the main mem at that point.
  		memset(machine->mainMemory + pAddr, 0, size); 	
  	}
  
  	//Random
  	else if(pageRepChoice == 2){
  		//if(pageList->getSize() == NumPhysPages)
  		//{
  		//	int pageIndex = Random() % NumPhysPages;
  		//	pageList->RemoveAt(pageIndex);*/
  		
  			//Get the address to the thread that has the page we are removing
  			Thread *threadNeeded = IPT[startPage];
  			//Set the page from the thread's page table to invalid
  			threadNeeded->space->pageTable[vpn].valid = FALSE;
  			IPT[startPage] = NULL;
  			//Zero out the main mem at that point.
  			memset(machine->mainMemory + pAddr, 0, size); 
  	
  		//}
  	}
  	//Demand Paging
  	else
  	{
  		if(memMap->NumClear() == 0)
		{
			printf("Not enough contiguous memory for new process; terminating!.\n");
			interrupt->Halt();
			return;
		}
  	}

  		
  	swapFile->ReadAt(&(machine->mainMemory[pAddr]),
		PageSize, noffH.code.inFileAddr +( vpn * PageSize));
	
	//Set the current page in the page table to valid
	//Update the physical page of the current page
	//Update the virtual page of the current page
	pageTable[vpn].valid = TRUE;
	pageTable[vpn].physicalPage = startPage;
	pageTable[vpn].virtualPage = vpn;


  	//PrintMainMem();	
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

//Because the initialization already zeroes out the memory to be used,
//is it even necessary to clear out any garbage data during deallocation?

AddrSpace::~AddrSpace()
{
	// Only clear the memory if it was set to begin with
	// which in turn only happens after space is set to true
	if(space)
	{
		while(!myPageList->IsEmpty()){  	
			int pageToRemove = (int)myPageList->Remove();
			memMap->Clear(pageToRemove);
		}
			
		int status = remove(FileName);
    	if(status == 0)
    		printf("%s file deleted Successfully.\n", FileName);
    	else
    		printf("Unable to delete the file.\n");

		delete pageTable;	
		memMap->Print();
	}
	delete myPageList;
	delete executable;
	delete swapFile;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i; 

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

void AddrSpace::PrintMainMem() {
	for (int j = 0; j < numPages; j++) {
		printf("Page Number: %d\n\n", j); 
		for (int k = 0; k < PageSize; k++) {
			printf("%d ", machine->mainMemory[(j*PageSize) + k]);
		}
		printf("\n\n");
	}
}

void
AddrSpace::CreateSwapFile(int threadID)
{
	//Create the Swap File to Write to
	//Go to filesys/fstest.cc for more info on how to do
	
	sprintf(FileName, "%s%d%s", "../SwapFiles/", threadID, ".swap");
	//printf("Creating file: %s\n", FileName);
	ASSERT( fileSystem->Create(FileName, 0) );
		
	swapFile = fileSystem->Open(FileName);
	ASSERT( swapFile != NULL);
	
	//Buffer used to copy from the executable to the swap file
	char* buffer = new char[newsize];
	
	//Reading in from the executable to the buffer
    executable->Read(buffer, newsize);
  
	
	//Writing in to the swapFile from the buffer
    swapFile->Write(buffer, newsize);
	delete buffer;

}

