


#include "hash.h"
#include "data.h"
#include "input.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


typedef struct {
	const unsigned char * data;
	uint64_t     hash;
} frame;

static inline void print_frame(frame * f){
	printf("%10s: 0x%016lx\n", f->data, f->hash);
}

static inline const void * key(const void * data){
	return &(((frame*)data)->hash);
}

static inline imax cmp_key(const void * a, const void * b){
	return (*(uint64_t*)b - *(uint64_t*)a);
}

uint8_t rot;

int main(void){
	FILE * fd, * hashfile;
	const uint8_t zero =0x0000, one = 0x0001, two = 0x0010;
	DS collision_table;
	frame   f;
	frame * f_pt;
	uint collisions;
	char num_str[10];
	uint64_t (*hash_funcs[_FUN_CNT])(uint64_t hash, uint32_t chunk) = {
		&hash_fnv1a,
		&hash_a,
		&hash_b,
	};
	
	
	collision_table = DS_new_bst(
		sizeof(frame),
		false,
		&key,
		&cmp_key
	);
	
	fd = fopen("/home/ammon/Documents/word_dic/passwords.txt", "r");
	hashfile = fopen("hash.h", "r");
	
	for(uint i=0; i<_FUN_CNT; i++){
		printf("\n\tFUNCTION %u\n", i);
		collisions =0;
		
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
		while( (f.data = grabline(fd)) != NULL ){
			f.hash = string_hash(0, hash_funcs[i], f.data);
			
			f_pt = (frame*) DS_insert(collision_table, &f);
			
			if(!f_pt){
/*				print_frame( (frame*) DS_current(collision_table));*/
/*				print_frame(&f);*/
/*				puts("");*/
				collisions++;
			}
		}
		
		DS_empty(collision_table);
		
		for(uint j =0; j<99999; j++){
			sprintf(num_str, "%05u", j);
			f.hash = array_hash(0, hash_funcs[i], num_str, 5);
			
			f_pt = (frame*) DS_insert(collision_table, &f);
			
			if(!f_pt){
				//print_frame( (frame*) DS_current(collision_table));
				//print_frame(&f);
				//puts("");
				collisions++;
			}
		}
		
		printf("%u collisions\n", collisions);
		DS_empty(collision_table);
	}
	
	
	DS_delete(collision_table);
	
	fclose(fd);
	fclose(hashfile);
	
/*	printf("\n\tHASH DEAD BEAF\n");*/
/*	printf("hash_djb2 : 0x%016lx.\n", array_hash(0, &hash_djb2 , &db, 4));*/
/*	printf("hash_djb2a: 0x%016lx.\n", array_hash(0, &hash_djb2a, &db, 4));*/
/*	printf("hash_gawk : 0x%016lx.\n", array_hash(0, &hash_gawk , &db, 4));*/
/*	printf("hash_sdbm : 0x%016lx.\n", array_hash(0, &hash_sdbm , &db, 4));*/
/*	printf("hash_fnv1a: 0x%016lx.\n", array_hash(0, &hash_fnv1a, &db, 4));*/
/*	printf("hash_a    : 0x%016lx.\n", array_hash(0, &hash_a    , &db, 4));*/
/*	printf("hash_b    : 0x%016lx.\n", array_hash(0, &hash_b    , &db, 4));*/
	
/*	fd = fopen("hash.h", "r");*/
/*	*/
/*	printf("\n\tHASH THIS FILE\n");*/
/*	printf("hash_djb2 : 0x%016lx.\n", file_hash(0, &hash_djb2, fd));*/
/*	rewind(fd);*/
/*	printf("hash_djb2a: 0x%016lx.\n", file_hash(0, &hash_djb2a, fd));*/
/*	rewind(fd);*/
/*	printf("hash_gawk : 0x%016lx.\n", file_hash(0, &hash_gawk, fd));*/
/*	rewind(fd);*/
/*	printf("hash_sdbm : 0x%016lx.\n", file_hash(0, &hash_sdbm, fd));*/
/*	rewind(fd);*/
/*	printf("hash_fnv1a: 0x%016lx.\n", file_hash(0, &hash_fnv1a, fd));*/
/*	rewind(fd);*/
/*	printf("hash_a    : 0x%016lx.\n", file_hash(0, &hash_a    , fd));*/
/*	rewind(fd);*/
/*	printf("hash_b    : 0x%016lx.\n", file_hash(0, &hash_b    , fd));*/
/*	*/
/*	fclose(fd);*/
/*	*/
/*	*/
	
	
	
	
	return EXIT_SUCCESS;
}


