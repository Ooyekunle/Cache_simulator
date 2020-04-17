#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


typedef enum access_type{
    READ,
    WRITE,
    INSTRUCTION_FETCH
} access_type_t;

typedef enum Cache_Line_size{
    SIXTEEN_BYTES = 16,
    THIRTY_TWO_BYTES = 32,
    SIXTY_FOUR_BYTES = 64,
    ONE_TWENTY_EIGHT_BYTES = 128 
} Cache_Line_size_t;

typedef struct record_s {
  uint32_t access_type;
  uint32_t address;
  uint32_t data;
} record_t;

typedef struct address_node {
  int tag_bit_size;
  int index_bit_size;
  int offset_bit_size;
} address_node_t;


#define NO_OF_RAM_ADRESS_BITS 32 // This is 4GB size of RAM
#define NO_OF_CACHE_MEM_BITS 16 // Since Cache is 64kb


#define CACHE_BLOCK_BASE_SIZE 16
#define NO_OF_CACHE_TAG_BITS 16 


uint32_t extract_tag_direct(uint32_t address);
uint32_t extract_index_direct(uint32_t address);
uint32_t extract_offset_direct(uint32_t address);
void process_record_direct(record_t * record);
int compare_tag_value(uint32_t index, uint32_t tag_value);
void mem_write(uint32_t index, uint32_t block_offset, uint32_t data);
uint32_t mem_read(uint32_t index, uint32_t block_offset);
void update_tag_table_direct(uint32_t index, uint32_t tag_value);
void print_simulation_result(void);
void setup_cache_mem_block(int cache_line_size , address_node_t * address);
void run_direct_mapped_simulation(FILE * fp, Cache_Line_size_t size);
void update_tag_table_associative(uint32_t set_value, uint32_t tag_value);

void update_stats(record_t * record);
float get_hit_ratio();
uint32_t extract_tag_associative(uint32_t address);
uint32_t extract_set_associative(uint32_t address);
void process_record_associative(record_t *record,  int no_of_ways);

void run_associativity(FILE * fp, Cache_Line_size_t size, int no_of_ways);
int compare_set_value(uint32_t set_index, uint32_t tag_value, int factor);
void update_block_with_fifo(uint32_t set_index, uint32_t tag_value, int factor);
void get_range(uint32_t set_index, int *min, int *max, int factor);
void process_file(const char *);


void allocate_cahe_mem_block(int length, int width);
void allocate_tag_table_block(int size);



