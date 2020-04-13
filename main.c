#include "main.h"

int read_count;
int write_count;
int instruction_count;
int line_count;
int cache_miss;
int cache_hit;
int access_attempt;
Cache_Line_size_t line_size;

uint16_t *tag_table_ptr;
uint_fast8_t *cache_mem_block_ptr;

// Look up table to kep the tag data
uint16_t tag_table[4096] = {

};

uint_fast8_t Cache_block[1024][64] = {

};

void mem_write(uint32_t index, uint32_t block_offset, uint32_t data) {
  // int i = 0;
  // //unpack 32 bit into 4 bytes, assume little endian
  // uint_fast8_t byte[4];
  // uint32_t mask = 0x000000ff;
  // for (int i = 0; i < 4; i++)
  // {
  //     byte[i] = data & mask;
  //     mask = mask << 8;
  // }
  // for (int i = 3; i >= 0; i--)
  // {
  //     Cache_block[index][block_offset + i] = byte[i];
  // }
}

uint32_t mem_read(uint32_t index, uint32_t block_offset) {
  // uint32_t data;
  // uint_fast8_t byte[4];
  // int i = 0;

  // for (int i = 0; i < 4; i++)
  // {
  //     byte[i] = Cache_block[index][block_offset + i];
  // }

  // // Pack the data back into unsigned 32 bit
  // data = byte[3];
  // for (int i = 2; i >= 0; i--)
  // {
  //     data = (data << 8) | byte[i];
  // }
  // return data;
}

void update_tag_table(uint32_t index, uint32_t tag_value) {
  tag_table_ptr[index] =
      (uint16_t)tag_value; // if a use the tag pointer here instead of the
                           // global array, it breaks
}

int compare_tag_value(uint32_t index, uint32_t tag_value) {
  ++access_attempt;

  // if a use the tag pointer here instead of the global array, it breaks
  if (tag_table_ptr[index] != (uint16_t)tag_value) {
    ++cache_miss;
    return 0;
  } else {
    ++cache_hit;
    return 1;
  }
}

uint32_t extract_placement_data(uint32_t address) {
  uint32_t placement_value;
  placement_value = address & 0x000003ff;
  return placement_value;
}

uint32_t extract_offset(uint32_t address) {
  uint32_t offset_value;

  if (line_size == SIXTEEN_BYTES) {
    offset_value = address & 0x0000000f;
    return offset_value;
  } else if (line_size == THIRTY_TWO_BYTES) {
    offset_value = address & 0x0000001f;
    return offset_value;
  } else if (line_size == SIXTY_FOUR_BYTES) {
    offset_value = address & 0x0000003f;
    return offset_value;
  } else {
    offset_value = address & 0x0000007f;
    return offset_value;
  }
}

// function to extract the tag data value
uint32_t extract_tag(uint32_t address) {
  uint32_t tag_value;
  tag_value = address >> 16;
  tag_value = tag_value & 0x0000ffff;
  return tag_value;
}

// function to extract index data
uint32_t extract_index(uint32_t address) {
  uint32_t index_value;

  if (line_size == SIXTEEN_BYTES) {
    index_value = address >> 4;
    index_value = index_value & 0x00000fff;
    // return index_value;
  } else if (line_size == THIRTY_TWO_BYTES) {
    index_value = address >> 5;
    index_value = index_value & 0x000007ff;
    // return index_value;
  } else if (line_size == SIXTY_FOUR_BYTES) {
    index_value = address >> 6;
    index_value = index_value & 0x000003ff;
    // return index_value;
  } else {
    index_value = address >> 7;
    index_value = index_value & 0x000001ff;
    // return index_value;
  }
  // printf("index value = %d\n", index_value);

  if (index_value >= 4096) {
    printf("BUGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n");
    while (1)
      ;
  }
  return index_value;
}

void process_record(record_t *record) {
  uint32_t tag_bits = extract_tag(record->address);
  uint32_t index_bits = extract_index(record->address);

  // if this return a zero, we have a miss
  if (!compare_tag_value(index_bits, tag_bits)) {
    // Update Cache block
    update_tag_table(index_bits, tag_bits);
  }
  access_type_t access_type = (access_type_t)record->access_type;
  switch (access_type) {
  case READ:
    ++read_count;
    // data = mem_read(index_bits, offset_bits);
    // printf("Tag bits value in decimal is %d\n", tag_bits);
    // printf("Index bits value in decimal is %d\n", index_bits);
    break;
  case WRITE:
    ++write_count;
    // mem_write(index_bits, offset_bits, data);
    // printf("Tag bit vslue in decimal is %d\n", tag_bits);
    // printf("Index bits value in decimal is %d\n", index_bits);
    break;
  default:
    ++instruction_count;
    // printf("Tag bit value in decimal is %d\n", tag_bits);
    // printf("Index bits value in decimal is %d\n", index_bits);
    break;
  }
}

void print_simulation_result(void) {
  printf("No of reads = %d\n", read_count);
  printf("No of write = %d\n", write_count);
  printf("No of fetch = %d\n", instruction_count);
  printf("No of lines = %d\n", line_count);
  printf("No of memory reference = %d\n", access_attempt);
  printf("No of Cache miss = %d\n", cache_miss);
  printf("No of Cache hits = %d\n", cache_hit);
  printf("Reseting result variables ..................................\n");
  read_count = write_count = instruction_count = 0;
  access_attempt = cache_miss = cache_hit = 0;
  line_count = 0;
  printf("====================================================================="
         "=\n");
}

void allocate_tag_table_block(int size) {
  printf("Allocating %d table block in memory\n", size);
  tag_table_ptr = (uint16_t *)malloc(sizeof(uint16_t) * size);
}

void allocate_cahe_mem_block(int length, int width) {
  printf("Allocating cache block in memory\n");
  cache_mem_block_ptr = (uint_fast8_t *)malloc(sizeof(length * width));
}

void run_simulation(FILE *fp, Cache_Line_size_t size) {
  printf("simulating cache of size %d KB, block size %d Bytes\n", 64, size);

  uint32_t line_data[3];
  uint32_t store = 0;
  int i = 0;
  line_size = size;

  double no_of_cache_line_offset_bits = log2((double)line_size);
  // printf("%f\n", no_of_cache_line_offset_bits);
  double no_of_cache_line_index_bits =
      (double)NO_OF_CACHE_LINE_BITS - no_of_cache_line_offset_bits;
  // printf("%f\n", no_of_cache_line_index_bits);
  double tag_table_allocation_size = pow(2, no_of_cache_line_index_bits);
  // printf("%f\n", tag_table_allocation_size);

  allocate_tag_table_block((int)tag_table_allocation_size);
  allocate_cahe_mem_block((int)tag_table_allocation_size,
                          (int)no_of_cache_line_offset_bits);

  record_t record;
  while (1) {
    fscanf(fp, "%x %x %x%*[\r\n]", &record.access_type, &record.address,
           &record.data);
    if (feof(fp)) break;
    process_record(&record);
  }
  fclose(fp);
  free(tag_table_ptr);
  free(cache_mem_block_ptr);
  print_simulation_result();
}

int main(int argc, char *argv[]) {

  printf("The given argument is %s trace file\n", argv[1]);
  // char *filename = argv[1];

  FILE *fp;
  fp = fopen("085.gcc.din.txt", "r");

  // Check if file exists
  if (!fp) {
    printf("Could not open sample.txt");
    return 0;
  } else {

    printf("FILE OPENED\n");
    run_simulation(fp, SIXTEEN_BYTES);
  }

  return 0;
}
