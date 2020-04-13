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


#define NO_OF_RAM_BITS 32 // This is 4GB size of RAM
#define NO_OF_CACHE_LINE_BITS 16 // Since Cache is 64kb
#define CACHE_BLOCK_BASE_SIZE 16
#define NO_OF_CACHE_TAG_BITS 16 


uint32_t extract_tag(uint32_t address);
uint32_t extract_index(uint32_t address);
uint32_t extract_offset(uint32_t address);
void process_record(record_t * record);

int compare_tag_value(uint32_t index, uint32_t tag_value);
void mem_write(uint32_t index, uint32_t block_offset, uint32_t data);
uint32_t mem_read(uint32_t index, uint32_t block_offset);
void update_tag_table(uint32_t index, uint32_t tag_value);
void print_simulation_result(void);

void run_direct_mapped_simulation(FILE * fp, Cache_Line_size_t size);

uint32_t extract_placement_data(uint32_t address);

void allocate_cahe_mem_block(int length, int width);
void allocate_tag_table_block(int size);



