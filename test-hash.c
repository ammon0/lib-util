
#define _TEST_HASH

#include "hash.h"
#include "data.h"
#include "input.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#define NUMSTR_SZ 16

typedef struct {
	const unsigned char * data;
	uint32_t     hash;
	char num_str[NUMSTR_SZ];
} frame;

static inline void print_frame(frame * f){
	if(f->data) printf("'%10s': 0x%08x\n", f->data, f->hash);
	else printf("'%s': 0x%08x\n", f->num_str, f->hash);
}

static inline const void * key(const void * data){
	return &(((frame*)data)->hash);
}

static inline imax cmp_key(const void * a, const void * b){
	return (*(uint64_t*)b - *(uint64_t*)a);
}


int main(void){
	FILE * fd, * hashfile;
	const uint8_t zero =0x0000, one = 0x0001, two = 0x0002;
	DS collision_table;
	frame   f;
	frame * f_pt;
	uint collisions, entries;
	uint64_t full_hash;
	
	uint64_t (*hash_funcs[_FUN_CNT])(uint64_t hash, uint32_t chunk) = {
		&hash_a,
		&hash_b,
		&hash_c,
		&hash_d
	};
	
	
	collision_table = DS_new_bst(
		sizeof(frame),
		false,
		&key,
		&cmp_key
	);
	
	fd = fopen("/home/ammon/Documents/word_dic/passwords.txt", "r");
	hashfile = fopen("hash.h", "r");
	
	if(!fd) puts("could not open file");
	
	for(uint i=0; i<_FUN_CNT; i++){
		printf("\n\tFUNCTION %u\n", i);
		collisions =0;
		entries = 0;
		
		printf("hash 0x0000: 0x%016lx\n",
			array_hash(0, hash_funcs[i], &zero, 1)
		);
		printf("hash 0x0001: 0x%016lx\n",
			array_hash(0, hash_funcs[i], &one, 1)
		);
		printf("hash 0x0002: 0x%016lx\n",
			array_hash(0, hash_funcs[i], &two, 1)
		);
		printf("hash hash.h: 0x%016lx\n",
			file_hash(0, hash_funcs[i], hashfile)
		);
		
		rewind(hashfile);
		rewind(fd);
		
		memset(f.num_str, 0, 8);
		
		while( (f.data = grabline(fd)) != NULL ){
			entries++;
			
			full_hash = string_hash(0, hash_funcs[i], f.data);
			
			f.hash = (full_hash>>32) ^ full_hash;
			
			f_pt = (frame*) DS_insert(collision_table, &f);
			
			if(!f_pt){
//				print_frame( (frame*) DS_current(collision_table));
//				print_frame(&f);
//				puts("");
				collisions++;
			}
		}
		
		DS_empty(collision_table);
		
		f.data = NULL;
		
		for(uint j =0; j<999999; j++){
			entries++;
			
			sprintf(f.num_str, "%015u", j);
			full_hash = array_hash(0, hash_funcs[i], f.num_str, NUMSTR_SZ);
			
			f.hash = (full_hash>>32) ^ full_hash;
			
			f_pt = (frame*) DS_insert(collision_table, &f);
			
			if(!f_pt){
//				print_frame( (frame*) DS_current(collision_table));
//				print_frame(&f);
//				puts("");
				collisions++;
			}
		}
		
		printf("%u collisions in %u entries.\n",
			collisions, entries
		);
		DS_empty(collision_table);
	}
	
	
	DS_delete(collision_table);
	
	fclose(fd);
	fclose(hashfile);
	
	return EXIT_SUCCESS;
}


