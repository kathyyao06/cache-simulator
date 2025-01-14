#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"
#include <math.h>
#include <string.h>
#include <getopt.h>


long **build_cache(long s, long E, long b)
{
    long **matrix = (long **)malloc(sizeof(long *) * s);
    for (int i = 0; i < s; i++) {
        matrix[i] = (long *)malloc(sizeof(long) * (E * 3));
    }

    for (int i = 0; i < s; i++) {
        for (int j = 0; j < E * 3; j++) {
            matrix[i][j] = 0;
        }
    }
    return matrix; 
}

void print_cache(long **cache, long s, long E, long b) 
{
    for (int i = 0; i < s; i ++) {
        for (int j = 0; j < E * 3; j++) {
            printf("%lu ", cache[i][j]);
        }
        printf("\n");
    }
}

void free_cache(long s, long **matrix)
{
    for (int i = 0; i < s; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

 
void print_help(int h) {
    if (h == 1) {
        printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>");
    }
}

int main(int argc, char *argv[])
{
    int opt;
    long h = 0;
    long v = 0;
    long s, E, b, set_num;
    char *input = NULL;
    int hits = 0;
    int misses = 0;
    int evicts = 0;

    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        if (opt == 'h') {
            h = 1;           
            print_help(h);
	} else if (opt == 'v') {
            v = 1;
            if (v == 1) {
                ;
            }    
        } else if (opt == 's') {
            s = atoi (optarg);
            set_num = pow(2, s);
        } else if (opt == 'E') {
	        E = atoi (optarg);
        } else if (opt == 'b') {
	        b = atoi (optarg);
        } else if (opt == 't') {
	        input = optarg;
        } else {
	        fprintf(stderr, "error");
        }
    }

    long **cache = build_cache(set_num, E, b);
    FILE *file = fopen(input, "r");

    if (file == NULL) {
        fprintf(stderr, "error");
        exit(1);
    }

    char oper;
    long unsigned address, size;
    unsigned int counter = 1;
    while (fscanf (file, " %c %lx,%lu", &oper, &address, &size) != EOF) {
        int did_hit = 0;
        if (oper == 'I') {
            continue;
        }

        long unsigned curr_tag = address >> (s + b);
        long unsigned curr_set = (address << (64 - s - b)) >> (64 - s);
        curr_set = curr_set % set_num;
        for (int i = 0; i < E * 3; i += 3) {
            if (cache[curr_set][i] == 1 && cache[curr_set][i + 1] == curr_tag) {
                hits++;
                if (oper == 'M') {
                    hits++;
                }
                cache[curr_set][i] = 1;
                cache[curr_set][i + 2] = counter;
                did_hit = 1;
                break;
            }
        }

        if (did_hit == 0) {
            int curr_LRU = 0;
            unsigned int min = counter + 1;
            int index = 0;
            for (int i = 2; i < E * 3; i += 3) {
               curr_LRU = cache[curr_set][i];
                if (curr_LRU < min) {
                    min = curr_LRU;
                    index = i - 2;
                }
            } 
            misses++;
            if (cache[curr_set][index] == 1) {
                evicts++;
            }
            cache[curr_set][index] = 1;
            cache[curr_set][index + 1] = curr_tag;
            cache[curr_set][index + 2] = counter;
            if (oper == 'M') {
                hits++;
            }
        }
        counter++;
    } 

    free_cache(set_num, cache);
    printSummary(hits, misses, evicts);
    return 0;
}



