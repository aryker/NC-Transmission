
















#define __STDC_FORMAT_MACROS
#include "fake_coding.h"


char* ALEX_RYKER;
bool* BOOKKEEPING;
int PIECE_COUNT = 0;
int BLOCK_TOTAL = 0; //The total number of blocks in the torrent. Set by initializeBookkeeping.
bool BK_FINISHED = false;
uint64_t FILE_SIZE;
bool FILE_DOWNLOADED = false;
bool SEEDER = false;

void printFileInfo(tr_torrent * tor) {
  tr_file_index_t i;
  const tr_file_index_t n = tor->info.fileCount;

  //  readDataFile(tor);
  
  printf("Name of Torrent: %s\n", tor->info.name);
  printf("Download Directory: %s\n", tor->downloadDir);
  printf("Number of Files: %d\n", n);
  printf("File names, sizes and offsets:\n");
  for(i = 0; i < n; ++i) {    
    printf("%d:  %s  --  %u  --  %u\n", i, tor->info.files[i].name, tor->info.files[i].length, tor->info.files[i].offset);
  }
}

void printProgress() {
  int haveCntr; //Counter for pieces we have.
  int haveNotCntr; //Counter for pieces we don't have.
  float percentDone; //have/haveNot

  int i;
  for(i = 0; i < PIECE_COUNT; i++) {
    if(BOOKKEEPING[i]) {
      haveCntr++;
    }else {
      haveNotCntr++;
    }
  }

  percentDone = haveCntr / haveNotCntr;
  printf("Our Progress: %f\n", percentDone);
}

/**OLD**
void initializeData(uint8_t * fileContents, size_t fileSize) {
  printf("Initializing data...\n");

  ALEX_RYKER = (char*)calloc(1, fileSize);  
  printf("Putting file data in RAM buffer...\n");
  memcpy(ALEX_RYKER, fileContents, fileSize);


  printf("\n===FAKE CODING BUFFER INITIALIZED===\n");
  printf("Size: %zu \n", fileSize);

  FILE_SIZE = fileSize; //Set this to be used in other functions.
}
*/

// **NEW**
int initializeData(tr_torrent * tor) {
  printf("Initializing data...\n");

  char * downloadDir = tor->downloadDir;
  //We are only interested in the first file.
  char * fileName = tor->info.files[0].name;  
  uint64_t fileSize = tor->info.files[0].length;
  FILE_SIZE = fileSize; //For use in writeData.
  char path[256];
  strcpy(path, downloadDir);
  strcat(path, "/");
  strcat(path, fileName);  
  printf("Path to data: %s\n", path);

  ALEX_RYKER = (char*)calloc(1, (fileSize + 1));  
  if(!ALEX_RYKER) {
    printf("Error allocating memory!\n");
    return -1;
  }

  if(hasAll()) {
    //If we are not a seeder, we should not try to read a file that doesn't exist.
    FILE * fp;
    fp = fopen(path, "r");
    if(!fp) {
      printf("Error opening data file!\n");
      return -1;
    }

    printf("Putting file data in RAM buffer...\n");
    fread(ALEX_RYKER, fileSize, 1, fp);

    printf("We must be a seeder...\n");
    writeFile();
  }


  printf("\n===FAKE CODING BUFFER INITIALIZED===\n");
  printf("Size: %zu \n", fileSize);

  return 0;
}



void initializeBookkeeping(uint32_t blockSize, tr_block_index_t blockCount) {

  /*
  size_t torrentSize = blockSize * blockCount;
  printf("torrentSize is: %d\n", torrentSize);

  size_t bookSize = sizeof(bool) * blockCount;
  BOOKKEEPING = (bool*)calloc(1, bookSize);

  int i;
  for(i = 0; i < blockCount; i++) {
    if((BOOKKEEPING[i] = tr_cpBlockIsComplete(cp, i))) {
      printf("WE HAVE BLOCK %d.\n", i);
    }else {
      printf("WE DO NOT HAVE BLOCK %d.\n", i);
    }
  }

  BLOCK_TOTAL = blockCount;

  BK_FINISHED = true;

  printf("Bookkeeping buffer initialized.\n");

  //Check if we are done. If so, write data to file.
  //FIXME: This is for testing and will need to be moved.
  /*
  if(hasAll()) {
    printf("We have all our data, so we will write it to a file now.\n");
    FILE * fp;
    fp = fopen("TESTING", "wb");
    size_t retVal = fwrite(ALEX_RYKER, FILE_SIZE, 1, fp);
    exit(0);
  }
  */
}


bool checkIndex(int index) {
  printf("Checking %d...\n", index);
  if(BOOKKEEPING[index]) {
    printf("We have %d!\n", index);
    return true;
  }else {
    printf("We don't have %d!\n", index);
    return false;
  }
}

bool hasAll() {
  //  printf("hasAll called. Return value: ");
  bool done = true;
  int i;
  for(i = 0; i < BLOCK_TOTAL; i++) {
    if(!BOOKKEEPING[i]) {
     done = false;
    }
  }

  /*
  if(done) {
    printf("true\n");
  }else {
    printf("false\n");
  }
  */

  return done;
}

char* getPiece(int index, uint32_t len) {
  char* data = (ALEX_RYKER + index * len);
  logProgress(index, true);
  return data;
}

void writeData(int index, struct evbuffer *writeme, uint32_t length) {
  memcpy((ALEX_RYKER + (length - sizeof(int)) * index), writeme, length);
}

void setHave(int index) {
  printf("setHave called on piece %d\n", index);
  BOOKKEEPING[index] = true;
  PIECE_COUNT++;
  logProgress(index, false);

  if(hasAll() && !FILE_DOWNLOADED) {
    //If we just finished downloading, write the buffer to a file.
    writeFile();
  }
}

void logProgress(int newPiece, bool sending) {
  FILE *logFile;
  if(sending) {
    logFile = fopen("sendLog.txt", "a");
    fprintf(logFile, "Sent piece %d.\n", newPiece);
  }else {
    logFile = fopen("receiveLog.txt", "a");
    fprintf(logFile, "Received piece %d.\n", newPiece);
  }
  
  fclose(logFile);
}

int writeFile() {
  if(!ALEX_RYKER) {
    printf("Error: No data in buffer!\n");
    return -1;
  }

  FILE * outFile;
  outFile = fopen("outFile.tar", "w");
  if(!outFile) {
    printf("Error opening output file!\n");
    return -1;
  }

  fwrite(ALEX_RYKER, FILE_SIZE, 1, outFile);  

  FILE_DOWNLOADED = true;
  
  printf("We have downloaded the file!\n");

  return 0;
}
