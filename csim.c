/*dgonzalezvillalo-ereimertburro
Diego Gonzalez Villalobos
Erik Reimert*/

#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <string.h>
#include <math.h>

typedef unsigned long long int memoryAddress;




struct set {          // store necessary info of a Line
	int latestUsed;
	int valid;
	memoryAddress tag;
	char *block;
};

struct cacheparam {      // store all the  basic info of cache
	int s;
	int b;
	int E;
	int S;
	int B;
	int hits;
	int misses;
	int evicts;
};

struct cache{      //make different sets of cache
	 struct cacheSet *sets;
};

struct cacheSet{    // sets are made of many lines
	struct set *lines;
};

struct cache initCache(long long setNum, int numlines, long long blockSize)
{    // struct for cache

	struct cache newCache;
	struct cacheSet set;
	struct set line;
	int indexOfSet;
	int indexOfLine;

	newCache.sets = (struct cacheSet*) malloc(sizeof(struct cacheSet) * setNum);      // allocate memory for the cache

	for (indexOfSet = 0; indexOfSet < setNum; indexOfSet ++)
	{        // loop makes every spot in every set 0, because the cache is empty at first

		set.lines =  (struct set *) malloc(sizeof(struct set) * numlines);
		newCache.sets[indexOfSet] = set;

		for (indexOfLine = 0; indexOfLine < numlines; indexOfLine ++)
		{
			line.latestUsed = 0;
			line.valid = 0;
			line.tag = 0;
			set.lines[indexOfLine] = line;
		}

	}

	return newCache;

}

void cacheEmpty(struct cache myCache, long long setNum, int numlines, long long blockSize)
{     //clears up cache because memory is not infinite and C doesnt do it for us



	int indexOfSet;
	for (indexOfSet = 0; indexOfSet < setNum; indexOfSet ++)
	{
		struct cacheSet set = myCache.sets[indexOfSet];

		if (set.lines != NULL) {
			free(set.lines);
		}

	}
	if (myCache.sets != NULL) {
		free(myCache.sets);
	}
}

int detectEvictLine(struct cacheSet exampleSet, struct cacheparam param, int *usedLines) {
	// this function detects the line that can be evicted ( the least recently used line)



	int numlines = param.E;
	int maxFreqUsage = exampleSet.lines[0].latestUsed;     //store usage frequency
	int minFreqUsage = exampleSet.lines[0].latestUsed;	 // store usage frequency
	int minFreqUsage_index = 0;
	int indexOfLine;


	//very basic loop, compare each line with max & min to decide
	for (indexOfLine = 1; indexOfLine < numlines; indexOfLine ++) {
		if (minFreqUsage > exampleSet.lines[indexOfLine].latestUsed) {
			minFreqUsage_index = indexOfLine;
			minFreqUsage = exampleSet.lines[indexOfLine].latestUsed;
		}

		if (maxFreqUsage < exampleSet.lines[indexOfLine].latestUsed) {
			maxFreqUsage = exampleSet.lines[indexOfLine].latestUsed;
		}
	}

	usedLines[0] = minFreqUsage;
	usedLines[1] = maxFreqUsage;
	return minFreqUsage_index;
}

int detectEmptyLine(struct cacheSet exampleSet, struct cacheparam param) {
	// check if whether the line that is matched is empty or not



	int numlines = param.E;
	int index;
	struct set line;

	for (index = 0; index < numlines; index ++) {
		line = exampleSet.lines[index];
		if (line.valid == 0) {    // one line is available
			return index;
		}
	}
	return -1;     // no line is available
}



struct cacheparam accessCache(struct cache myCache, struct cacheparam param, memoryAddress addr) {
		int indexOfLine;
		int checkFullCache = 1;     // assume that cache is full

		int numlines = param.E;
		int previousHit = param.hits;

		int tagSize = (64 - (param.s + param.b));    // take the valid tag out t = m-s-b
		memoryAddress inputTag = addr >> (param.s + param.b);
		unsigned long long temp = addr << (tagSize);
		unsigned long long indexOfSet = temp >> (tagSize + param.b);

  		struct cacheSet exampleSet = myCache.sets[indexOfSet];

		for (indexOfLine = 0; indexOfLine < numlines; indexOfLine ++) 	{

			if (exampleSet.lines[indexOfLine].valid) {   // check the valid tag != 0

				if (exampleSet.lines[indexOfLine].tag == inputTag) {
						//check for matching tag


					exampleSet.lines[indexOfLine].latestUsed ++;  // update for later use of eviction
					param.hits ++;
				}
				// If the valid tag is different from 0 and the input tag matches that line tag, its a cache hit


			} else if (!(exampleSet.lines[indexOfLine].valid) && (checkFullCache)) {
				// valid tag = 0, fullcache != 0

				checkFullCache = 0;	    // reset this to 0	because there is empty space left.
			}

		}

		if (previousHit == param.hits) {   // if after the above loop nothing hit -> we miss
			param.misses++;    // raise miss
		} else {
			return param;
		}
		int *usedLines = (int*) malloc(sizeof(int) * 2);
		int minFreqUsage_index = detectEvictLine(exampleSet, param, usedLines);

		if (checkFullCache)     // if cache is full (checkFullCache!=0) do eviction
		{
			param.evicts++;
			exampleSet.lines[minFreqUsage_index].tag = inputTag;
			exampleSet.lines[minFreqUsage_index].latestUsed = usedLines[1] + 1;

		}

		else        // else write to tge empty line
	        {
			int empty_index = detectEmptyLine(exampleSet, param);
			exampleSet.lines[empty_index].tag = inputTag;
			exampleSet.lines[empty_index].valid = 1;
			exampleSet.lines[empty_index].latestUsed = usedLines[1] + 1;
		}

		free(usedLines);
		return param;
}

int main(int argc, char **argv)
{

	struct cache myCache;
	struct cacheparam param;
	memset(&param, 0, sizeof(param));
	long long setNum;
	long long blockSize;
	FILE *trace;
	char tfileInstruct;
	memoryAddress addr;
	int size;
	char *tFile;
	char c;
	/* takes arguments and directs to proper output as an inputTag*/
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1)
	{
        switch(c)
		{
        case 's':
            param.s = atoi(optarg);
            break;
        case 'E':
            param.E = atoi(optarg);
            break;
        case 'b':
            param.b = atoi(optarg);
            break;
        case 't':
            tFile = optarg;
            break;
        case 'h':
            exit(0);
        default:
            exit(1);
        }
    }


 	setNum = pow(2.0, param.s);   // get Number of set by 2^s
	blockSize = pow(2.0, param.b);  //  get blockSize by 2^b
	param.hits = 0;
	param.misses = 0;
	param.evicts = 0;
	myCache = initCache (setNum, param.E, blockSize);

	trace  = fopen(tFile, "r"); //open tracefile and read it
	if (trace != NULL) {
		while (fscanf(trace, " %c %llx,%d", &tfileInstruct, &addr, &size) == 3) {
			switch(tfileInstruct) {
				case 'I':
					break;
				case 'L':
					param = accessCache(myCache, param, addr);
					break;
				case 'S':
					param = accessCache(myCache, param, addr);
					break;
				case 'M':
					param = accessCache(myCache, param, addr);
					param = accessCache(myCache, param, addr);
					break;
				default:
					break;
			}
		}
	}


    printSummary(param.hits, param.misses, param.evicts);
	cacheEmpty(myCache, setNum, param.E, blockSize);
	fclose(trace);
    return 0;
}
