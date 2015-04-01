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
    unsigned int i, size, pAddr, counter;
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
	pageList = new List();

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
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
	
	memMap->Print();	// Useful!
	
	//Create the Swap File to Write to
	//Go to filesys/fstest.cc for more info on how to do
	sprintf(FileName, "%s%d%s", "../SwapFiles/", currentThread->getID(), ".swap");
	
	ASSERT( fileSystem->Create(FileName, 0) );
     	
     	//Some code to write "1234567890" to the file
     	char* contents = "1234567890";
     	int ContentSize = strlen(contents);
     	//int FileSize = ((int)(ContentSize * 5000));
     	int numBytes, i;
     	
	
	/*swapFile = fileSystem->Open(FileName);
    	if (swapFile == NULL) {
		printf("Perf test: unable to open %s\n", FileName);
		return;
	}
    	
    	


        numBytes = swapFile->Write(contents, ContentSize);
	if (numBytes < 10) {
	    printf("Perf test: unable to write %s\n", FileName);
	    delete swapFile;
	    return;
	}


	
	//Read file.
	char *buffer = new char[ContentSize];
		numBytes = swapFile->ReadAt(buffer, ContentSize, 0);
		
	printf("Content read in %s was: %s\n", FileName, buffer);
	
    	delete buffer;
    	
    	delete swapFile;*/
     	
     	//Delete the file when done
     	/*int status = remove(FileName);
     	if(status == 0)
     		printf("%s file deleted Successfully.\n", FileName);
     	else
     		printf("Unable to delete the file.\n");*/
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
//    bzero(machine->mainMemory, size); rm for Solaris
	//Edited version adds startPage * PageSize to the address. Hopefully this is proper.
	//Either way, it appears to zero out only however much memory is needed,
	//so zeroing out all memory doesn't seem to be an issue. - Devin
	
	pAddr = startPage * PageSize;
	
   // memset(machine->mainMemory + pAddr, 0, size);
    //Paging();
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
AddrSpace::DeleteExe()
{
	delete executable;
}

void
AddrSpace::Paging(int vpn)
{
	//printf("VPN: %d\n\n", vpn);
	vpn = vpn / PageSize;
	
	//printf("NEW VPN: %d\n\n", vpn);
	//Set the current page in the page table to valid
	pageTable[vpn].valid = TRUE;

	//Trying to figure out how to maths
	/*
	printf("Code VirtualAddr: %i\n",noffH.code.virtualAddr);
	printf("StartPage: %i\n",startPage);
	printf("PageToInit: %i\n",pageToInit);
	printf("Size: %i\n", PageSize);
	printf("Position: %i\n", noffH.code.inFileAddr + ( pageToInit * PageSize));
	printf("Index: %i\n", noffH.code.virtualAddr + (startPage * PageSize) + (pageToInit * PageSize));

	*/
	//Copy the code segments for the current page into memory
	
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
	if(counter < numPages)
	{
		printf("Not enough contiguous memory for new process; terminating!.\n");
		interrupt->Halt();
		return;
	}
	memMap->Mark(startPage);
	
	pageList->Append((void*)startPage);
	
	memMap->Print();
	pAddr = startPage * PageSize;
	
	memset(machine->mainMemory + pAddr, 0, PageSize);
	
	//printf("CODE VIRT ADDR: %d\n", noffH.code.virtualAddr);
	//printf("CODE INFILE ADDR: %d\n", noffH.code.inFileAddr);
	
	/*printf("MM Begin: %d\n", pAddr);
	printf("MM End: %d\n", pAddr + PageSize);
	printf("Start Position %d\n", noffH.code.inFileAddr + ( vpn * PageSize));
	printf("End Position %d\n", noffH.code.inFileAddr + ( vpn * PageSize) + PageSize);
	printf("START PAGE: %d\n", startPage);
	printf("VPN: %d\n", vpn);*/
	
	swapFile = fileSystem->Open(FileName);
	ASSERT( swapFile != NULL);
	
	
    if (noffH.code.size > 0) {
		//printf("*************************StartPage: %i\n", startPage);
		//printf("*************************VPN: %i\n", vpn);
        executable->ReadAt(&(machine->mainMemory[pAddr]),
			PageSize, noffH.code.inFileAddr + ( vpn * PageSize));
			
	swapFile->WriteAt(&(machine->mainMemory[pAddr]),
			PageSize, noffH.code.inFileAddr + ( vpn * PageSize));

			
    }
    //PrintMainMem();
  //  printf("AFTER READ AT\n\n");

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
		while(!pageList->IsEmpty()){
			int pageToRemove = (int)pageList->Remove();
			memMap->Clear(pageToRemove);
		}
			


		delete pageTable;	
		memMap->Print();
	}
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
