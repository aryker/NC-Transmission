#ifndef FAKE_CODING_H
#define FAKE_CODING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h> /* bsearch () */
#include <string.h> /* memcmp () */

#include <openssl/sha.h>

#include "transmission.h"
#include "cache.h" /* tr_cacheReadBlock () */
#include "inout.h"
#include "log.h"
#include "peer-common.h" /* MAX_BLOCK_SIZE */
#include "stats.h" /* tr_statsFileCreated () */
#include "torrent.h"
#include "utils.h"
#include "completion.h"
#include "bandwidth.h"
#include "session.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

//Array for torrent data
extern char* ALEX_RYKER;

//Array for keeping track of what we have
extern bool* BOOKKEEPING;

//For keeping track of the piece count ourselves.
extern int PIECE_COUNT;

//The total number of blocks in the torrent(so we don't need to keep passing it in)
extern int BLOCK_TOTAL;

//A flag for keeping track of whether the bookkeeping array is loaded. Used for the tr_completion stuff.
extern bool BK_FINISHED;

//For keeping track of the size of the data file.
extern uint64_t FILE_SIZE;

//A flag for keeping track of whether or not we have downloaded the file.
extern bool FILE_DOWNLOADED;



/**
 * Prints the info on the current torrent's files.
 */
void printFileInfo(tr_torrent * tor);

/**
 * Prints out the percentage of the torrent we have based on our bookkeeping.
 */
void printProgress();

/**
 * Initializes appropriate memory for torrent data buffer and loads file data into it.
 * Returns 0 if successful, non-zero if not.
*/
int initializeData(tr_torrent * tor);

/**
 *Initializes appropriate data for the bookkeeping array.
 */
void initializeBookkeeping(uint32_t blockSize, tr_block_index_t blockCount);

/**
 *Checks the given index to see if we have the piece associated with it. Returns 1 if we have it, 0 if not.
 */
bool checkIndex(int index);

/**
 * Returns true if we have all pieces in the data array, false if not.
 */
bool hasAll();

/**
 * Returns a pointer to the data in ALEX_RYKER at the given index.
*/
char* getPiece(int index, uint32_t len);

/**
 * Writes the given writeme buffer to ALEX_RYKER.
 */
void writeData(int index, struct evbuffer *writeme, uint32_t length);

/**
 *Sets the boolean value in BOOKKEEPING to true
 * for the corresponding piece.
 */
void setHave(int index);

/**
 * Prints a logging message to a log file. If the sending value is true,
 * it prints a "sent" log message. If it is false, it prints a "received"
 * log message.
 */
void logProgress(int newPiece, bool sending);

/**
 * Writes the data buffer to a file.
 * Returns 0 if finished correctly, non-zero if not.
 */
int writeFile();

#endif


