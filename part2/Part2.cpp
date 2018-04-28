#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <iostream>
using namespace std;


#define FRAME_SIZE 256        // size of the frame
#define NUMBER_OF_FRAMES 128  // total number of frames in physical memory


#define TLB_SIZE 16       // size of the TLB
#define PAGE_TABLE_SIZE 256  // size of the page table

int pageTableIndex[PAGE_TABLE_SIZE];  // array to hold the page numbers in the page table
int pageTableFrames[PAGE_TABLE_SIZE];   // array to hold the frame numbers in the page table

int pageTable[PAGE_TABLE_SIZE];

int TLBPage[TLB_SIZE];  // array to hold the page numbers in the TLB
int TLBFrame[TLB_SIZE]; // array to hold the frame numbers in the TLB

int timer[NUMBER_OF_FRAMES];

int physicalMemory[NUMBER_OF_FRAMES][FRAME_SIZE]; // physical memory 2D array

int pageFaults = 0;   // counter to track page faults
int TLBHits = 0;      // counter to track TLB hits

int firstAvailableFrame = 0;  // counter to track the first available frame
int firstAvailablePageTableNumber = 0;  // counter to track the first available page table entry
int numberOfTLBEntries = 0;             // counter to track the number of entries in the TLB
 int freeFrame = 0, countFrame = 0;
 int count = 0;
 int frameNumber;
 bool initialized = true;
 bool tlbhit,incre;
 int countTime = 0;
 bool lru, fifo;

// number of characters to read for each line from input file
#define address_size         12

// number of bytes to read
#define page_size               256

// input file and backing store
FILE    *address_file;
FILE    *backing_store;
FILE * output;

// how we store reads from input file
char    address[address_size];
int     logicalAddress;

// the backStoreValue containing reads from backing store
signed char     backStoreValue[page_size];

// the value of the byte (signed char) in memory
signed char     value;

// headers for the functions used below
void getPage(int address);
void  handlePageFaultsLRU(int pageNumber);
void handlePageFaultsFIFO(int pageNumber);
void insertIntoTLB(int pageNumber, int frameNumber);
int findLRU(int time[], int n);


int findLRU(int time[], int n){
    int minimum = 9999, pos = 0, lruFrame;

 
    for(int i = 0; i < n; i++){

        
        if( time[i] < minimum ){

            minimum = time[i];
            pos = i;

        }
    }
    
    return pos;
}

// function to take the logical address and obtain the physical address and value stored at that address
void getPage(int logicalAddress){
    
    
      int offset = logicalAddress & 0xff;
     int pageNumber = logicalAddress >> 8;

     if (initialized){

        for ( int i = 0; i < PAGE_TABLE_SIZE; i++){

            pageTable[i] = -1;
            initialized = false;
        
         }


        for ( int i = 0; i < TLB_SIZE; i++){

            TLBPage[i] = -1;
            TLBFrame[i] = -1;
            initialized = false;
         }

 
     }
    
    // first try to get page from TLB
    frameNumber = -1; // initialized to -1 to tell if it's valid in the conditionals below
    tlbhit = false;
    int i;  // look through TLB for a match
    for(i = 0; i < TLB_SIZE; i++){

        if(TLBPage[i] == pageNumber){   // if the TLB index is equal to the page number

                frameNumber = TLBFrame[i];  // then the frame number is extracted
                TLBHits++;                // and the TLBHit counter is incremented
                tlbhit = true;
                break;
        }
          
    }
        



    
    // if the frameNumber was not found
    if(frameNumber == -1){
           // walk the contents of the page table
        for(int i = 0; i <= pageNumber; i++){

            if( pageNumber == i ){         // if the page is found in those contents

                frameNumber = pageTable[i];          // extract the frameNumber from its twin array
                
            
            }

         }

        if(frameNumber == -1){                     // if the page is not found in those contents
                   
          if(fifo == true && lru == false){

            handlePageFaultsFIFO(pageNumber);             // page fault, call to handlePageFaults to get the frame into physical memory and the page table
          
          }else if (lru == true && fifo == false){

                handlePageFaultsLRU(pageNumber);  

          }

            pageFaults++;                          // increment the number of page faults
           

            frameNumber = firstAvailableFrame - 1;  // and set the frameNumber to the current firstAvailableFrame index
           

        }
    }



    
    if (!tlbhit){

    insertIntoTLB(pageNumber, frameNumber);  // call to function to insert the page number and frame number into the TLB
   
   }

    value = physicalMemory[frameNumber][offset];  // frame number and offset used to get the signed value stored at that address
   

    countTime++;
    timer[frameNumber] = countTime;
    
    fprintf(output,"Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, (frameNumber << 8) | offset, value);


}


void insertIntoTLB(int pageNumber, int frameNumber){


    for (int i = 0; i < numberOfTLBEntries ; i++){
     
       if ( TLBPage[i] == pageNumber){
            
              break;
        }
    }

  
        if (numberOfTLBEntries  >= TLB_SIZE){

                 numberOfTLBEntries = 0;
        }

    
              TLBPage[numberOfTLBEntries ] = pageNumber;
              TLBFrame[numberOfTLBEntries ] = frameNumber;

          numberOfTLBEntries ++;
          
      
}

void handlePageFaultsFIFO(int pageNumber){
    
    // first seek to byte page_size in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    fseek(backing_store, pageNumber * page_size, SEEK_SET);
    
    // now read page_size bytes from the backing store to the backStoreValue
    fread(backStoreValue, sizeof(signed char), page_size, backing_store);
    
   
    // load the bits into the first available frame in the physical memory 2D array
   

    if ( firstAvailableFrame >= NUMBER_OF_FRAMES){

        firstAvailableFrame = 0 ;

    }
    

    for(int i = 0; i < page_size; i++){

        physicalMemory[firstAvailableFrame][i] = backStoreValue[i];

    }

       for ( int i = 0; i < PAGE_TABLE_SIZE; i++){

       if ( pageTable[i] == firstAvailableFrame){
  
                pageTable[i] = -1;
            }
         } 



    // and then load the frame number into the page table in the first available frame

    pageTable[pageNumber] = firstAvailableFrame;
    
    // increment the counters that track the next available frames
    firstAvailableFrame++;
    firstAvailablePageTableNumber++;
    
}




// function to read from the backing store and bring the frame into physical memory and the page table
void  handlePageFaultsLRU(int pageNumber){
    
    // first seek to byte page_size in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    fseek(backing_store, pageNumber * page_size, SEEK_SET);
    
    // now read page_size bytes from the backing store to the backStoreValue
    fread(backStoreValue, sizeof(signed char), page_size, backing_store);
    
    
    // load the bits into the first available frame in the physical memory 2D array
 
         if ( firstAvailablePageTableNumber >= NUMBER_OF_FRAMES){


        firstAvailableFrame = findLRU(timer,NUMBER_OF_FRAMES) ;
       

    }

  
    

    for(int i = 0; i < page_size; i++){

        physicalMemory[firstAvailableFrame][i] = backStoreValue[i];

    }

       for ( int i = 0; i < PAGE_TABLE_SIZE; i++){

       if ( pageTable[i] == firstAvailableFrame){
          
                pageTable[i] = -1;
            }
         } 




    // and then load the frame number into the page table in the first available frame

    pageTable[pageNumber] = firstAvailableFrame;
    
    // increment the counters that track the next available frames
    if (firstAvailableFrame < NUMBER_OF_FRAMES){
    firstAvailableFrame++;
  }
    firstAvailablePageTableNumber++;
    
}





// main opens necessary files and calls on getPage for every entry in the addresses file
int main(int argc, char *argv[])
{
    // perform basic error checking
    if (argc != 4) {
        fprintf(stderr,"Usage: ./a.out BACKING_STORE.bin addresses.txt technique \n");
        return -1;
    }
    


    if (  strcmp(argv[3], "lru") && strcmp(argv[3], "fifo") ){
        
          printf(" Invalid Input for strategy. Choose between {fifo or lru} \n");
           return -1;

    }


    if (strcmp(argv[3], "lru") ){

      fifo = true;
      lru = false;
    }else if (strcmp(argv[3], "fifo") ){

      lru = true;
      fifo = false;
    }
   
    // open the file containing the backing store
    backing_store = fopen(argv[1], "rb");

     // open the file containing the logical addresses
    address_file = fopen(argv[2], "r");

    output = fopen("correct2.txt", "w+");
    
    
    int numberOfTranslatedAddresses = 0;


    // read through the input file and output each logical address
    while ( fgets(address, address_size, address_file) != NULL) {
        logicalAddress = atoi(address);


        // get the physical address and value stored at that address
        getPage(logicalAddress);
        numberOfTranslatedAddresses++;  // increment the number of translated addresses        
    }


    
    // calculate and print out the stats
   
    fprintf(output,"Number of Translated Addresses = %d\n", numberOfTranslatedAddresses);
    double pfRate = pageFaults / (double)numberOfTranslatedAddresses;
    double TLBRate = TLBHits / (double)numberOfTranslatedAddresses;
    
    //printf("Page Faults = %d\n", pageFaults);
    fprintf(output,"Page Faults = %d\n", pageFaults);
    
    fprintf(output,"Page Fault Rate = %.3f\n",pfRate);
  
    fprintf(output,"TLB Hits = %d\n", TLBHits);
  
    fprintf(output,"TLB Hit Rate = %.3f\n", TLBRate);
    
    // close the input file and backing store
    fclose(address_file);
    fclose(backing_store);
    
    return 0;
}