
#define _TEST_HASH

#include <util/hash.h>
#include <util/data.h>
#include <util/input.h>
#include <util/io.h>

#include <stdlib.h>

#include <sys/time.h>
#include <math.h>


#define NUMSTR_SZ 16
#define HASH_SEED 0

static char ** strings;
static uint    str_cnt;


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
	return (*(int64_t*)b - *(int64_t*)a);
}

static void stage_strings(FILE * f){
	DS      temp_list;
	char *  line;
	
	temp_list = DS_new_list(sizeof(char*));
	
	while(( line = grabline(f) )) DS_nq(temp_list, &line);
	
	str_cnt = DS_count(temp_list);
	strings = (char**)malloc(str_cnt*sizeof(char*));
	
	for(uint i=0; i<str_cnt; i++) strings[i] = *(char**)DS_remove(temp_list);
	
	DS_delete(temp_list);
}


static void
collision_check(uint64_t (*hf)(uint64_t hash, uint32_t chunk)){
	DS collision_table;
	uint collisions=0, entries=0;
	uint64_t full_hash;
	frame f;
	frame * f_pt;
	
	#define SEQ_LIM 99999
	
	collision_table = DS_new_bst(
		sizeof(frame),
		false,
		&key,
		&cmp_key
	);
	
	for(uint i=0; i<str_cnt; i++){
		full_hash = string_hash(HASH_SEED, hf, strings[i]);
		
		// fold the hash
		f.hash = (uint32_t)((full_hash>>32) ^ full_hash);
		
		// check for collisions
		entries++;
		f_pt = (frame*) DS_insert(collision_table, &f);
		if(!f_pt) collisions++;
	}
	
	for(uint j =0; j<SEQ_LIM; j++){
		// hash the number
		snprintf(f.num_str, NUMSTR_SZ, "%015u", j);
		full_hash = array_hash(HASH_SEED, hf, f.num_str, NUMSTR_SZ);
		
		// fold the hash
		f.hash = (uint32_t)((full_hash>>32) ^ full_hash);
		
		// check for collisions
		entries++;
		f_pt = (frame*) DS_insert(collision_table, &f);
		if(!f_pt) collisions++;
	}
	
	// report results
	printf("%u collisions from %u entries.\n",
		collisions, entries
	);
}


static void time_check(uint64_t (*hf)(uint64_t hash, uint32_t chunk)){
	struct timeval start, stop;
	char           array[NUMSTR_SZ];
	uint64_t       full_hash=0;
	
	#define TC_COUNT 0xFFFFFFF
	
	gettimeofday(&start, NULL);
	
	// hash strings
	for(uint i=0; i<str_cnt; i++) full_hash += string_hash(HASH_SEED, hf, strings[i]);
	
	// hash numbers
	for(uint32_t j =0; j<TC_COUNT; j++){
		snprintf(array, NUMSTR_SZ, "%015u", j);
		full_hash += array_hash(HASH_SEED, hf, array, 4);
	}
	gettimeofday(&stop, NULL);
	
	printf("full_hash is %lu\n", full_hash);
	
	printf("start: %ld.%06ld, stop us: %ld.%06ld\n",
		start.tv_sec, start.tv_usec,
		stop.tv_sec, stop.tv_usec
	);
	
	printf("%u hashes in seconds: %ld\n",
		TC_COUNT+str_cnt,
		stop.tv_sec-start.tv_sec
	);
}




int main(void){
	FILE * fd, * hashfile;
	const uint8_t zero =0x0000, one = 0x0001, two = 0x0002;
	
	uint64_t (*hash_funcs[_FUN_CNT])(uint64_t hash, uint32_t chunk) = {
		&hash_a,
		&hash_b,
		&hash_c,
		&hash_d
	};
	
	// setup test strings
	fd = fopen("test_file.txt", "r");
	if(!fd){
		puts("could not open test_file.txt");
		return EXIT_FAILURE;
	}
	stage_strings(fd);
	fclose(fd);
	
	
	hashfile = fopen("util/hash.h", "r");
	
	for(uint i=0; i<_FUN_CNT; i++){
		printf("\n\tFUNCTION %u\n", i);
		
		/********************* show the first few numbers *********************/
		printf("hash 0x0000: 0x%016lx\n",
			array_hash(HASH_SEED, hash_funcs[i], &zero, 1)
		);
		printf("hash 0x0001: 0x%016lx\n",
			array_hash(HASH_SEED, hash_funcs[i], &one, 1)
		);
		printf("hash 0x0002: 0x%016lx\n",
			array_hash(HASH_SEED, hash_funcs[i], &two, 1)
		);
		
		// prove that file hashing works
		printf("hash hash.h: 0x%016lx\n",
			file_hash(HASH_SEED, hash_funcs[i], hashfile)
		);
		rewind(hashfile);
		
		collision_check(hash_funcs[i]);
		time_check     (hash_funcs[i]);
	}
	
	fclose(hashfile);
	
	return EXIT_SUCCESS;
}


