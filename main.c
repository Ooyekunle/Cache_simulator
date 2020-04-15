#include "main.h"

int read_count;
int write_count;
int instruction_count;
int line_count;
int cache_miss;
int cache_hit;
int access_attempt;
Cache_Line_size_t line_size;
int placement_count;
int replacement_count;

uint16_t *tag_table_ptr;
uint_fast8_t *cache_mem_block_ptr;

// Look up table to kep the tag data
uint16_t tag_table[4096] = {

};

uint_fast8_t Cache_block[1024][64] = {

};

void mem_write(uint32_t index, uint32_t block_offset, uint32_t data)
{
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

uint32_t mem_read(uint32_t index, uint32_t block_offset)
{
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

void update_tag_table(uint32_t index, uint32_t tag_value)
{
  tag_table_ptr[index] =
      (uint16_t)tag_value;
}

int compare_tag_value(uint32_t index, uint32_t tag_value)
{
  ++access_attempt;

  // if a use the tag pointer here instead of the global array, it breaks
  if (tag_table_ptr[index] != (uint16_t)tag_value)
  {
    ++cache_miss;
    return 0;
  }
  else
  {
    ++cache_hit;
    return 1;
  }
}

uint32_t extract_offset_direct(uint32_t address)
{
  uint32_t offset_value;

  if (line_size == SIXTEEN_BYTES)
  {
    offset_value = address & 0x0000000f;
    return offset_value;
  }
  else if (line_size == THIRTY_TWO_BYTES)
  {
    offset_value = address & 0x0000001f;
    return offset_value;
  }
  else if (line_size == SIXTY_FOUR_BYTES)
  {
    offset_value = address & 0x0000003f;
    return offset_value;
  }
  else
  {
    offset_value = address & 0x0000007f;
    return offset_value;
  }
}

// function to extract the tag data value
uint32_t extract_tag_direct(uint32_t address)
{
  uint32_t tag_value;
  tag_value = address >> 16;
  tag_value = tag_value & 0x0000ffff;
  return tag_value;
}

// function to extract index data
uint32_t extract_index_direct(uint32_t address)
{
  uint32_t index_value;

  if (line_size == SIXTEEN_BYTES)
  {
    index_value = address >> 4;
    index_value = index_value & 0x00000fff;
  }
  else if (line_size == THIRTY_TWO_BYTES)
  {
    index_value = address >> 5;
    index_value = index_value & 0x000007ff;
  }
  else if (line_size == SIXTY_FOUR_BYTES)
  {
    index_value = address >> 6;
    index_value = index_value & 0x000003ff;
  }
  else
  {
    index_value = address >> 7;
    index_value = index_value & 0x000001ff;
  }

  return index_value;
}

void update_stats(record_t *record)
{
  access_type_t access_type = (access_type_t)record->access_type;
  switch (access_type)
  {
  case READ:
    ++read_count;
    break;
  case WRITE:
    ++write_count;
    break;
  default:
    ++instruction_count;
    break;
  }
}

void process_record_direct(record_t *record)
{
  uint32_t tag_bits = extract_tag_direct(record->address);
  uint32_t index_bits = extract_index_direct(record->address);

  // if this return a zero, we have a miss
  if (!compare_tag_value(index_bits, tag_bits))

  {
    // Update Cache block
    update_tag_table(index_bits, tag_bits);
  }

  update_stats(record);
}

void print_simulation_result(void)
{
  printf("No of reads = %d\n", read_count);
  printf("No of write = %d\n", write_count);
  printf("No of fetch = %d\n", instruction_count);
  printf("No of memory reference = %d\n", ++access_attempt);
  printf("No of Cache miss = %d\n", cache_miss);
  printf("No of Cache hits = %d\n", cache_hit);
  printf("Total number of placement = %d\n", placement_count);
  printf("No of replacement = %d\n", replacement_count);
  printf("Simulation Completed\n");
  printf("Reseting result variables ..................................\n");
  read_count = write_count = instruction_count = 0;
  access_attempt = cache_miss = cache_hit = 0;
  printf("====================================================================="
         "=\n");
}

void allocate_tag_table_block(int size)
{
  printf("Allocating %d table block in memory\n", size);
  tag_table_ptr = (uint16_t *)calloc(size, sizeof(uint16_t));
}

void allocate_cahe_mem_block(int length, int width)
{
  printf("Allocating cache memory block %d x %d Bytes in memory\n", length, width);
  cache_mem_block_ptr = (uint_fast8_t *)calloc(length * width, sizeof(uint8_t));
}

void setup_cache_mem_block(int cache_line_size, address_node_t *address)
{
  address->offset_bit_size = 0;
  while (1)
  {
    cache_line_size = cache_line_size >> 1;
    address->offset_bit_size++;
    if (cache_line_size == 1)
      break;
  }
}

uint32_t extract_tag_associative(uint32_t address)
{
  // We know the tag bit wold always be 17
  // Mask out 17 MSB
  uint32_t set_value;
  set_value = address >> 15;
  set_value = set_value & 0x0001ffff;
  return set_value;
}

uint32_t extract_set_associative(uint32_t address)
{
  uint32_t index_value;

  if (line_size == SIXTEEN_BYTES)
  {
    index_value = address >> 4;
    index_value = index_value & 0x000007ff;
  }
  else if (line_size == THIRTY_TWO_BYTES)
  {
    index_value = address >> 5;
    index_value = index_value & 0x000003ff;
  }
  else if (line_size == SIXTY_FOUR_BYTES)
  {
    index_value = address >> 6;
    index_value = index_value & 0x000001ff;
  }
  else
  {
    index_value = address >> 7;
    index_value = index_value & 0x000000ff;
  }

  return index_value;
}

void get_range(uint32_t set_index, int *min, int *max, int factor)
{
  int search_offset = factor - 1;
  *min = set_index * factor;
  *max = *min + search_offset;
}

void update_block_with_fifo(uint32_t set_index, uint32_t tag_value, int factor)
{
  int i = 0;
  uint32_t min, max;
  get_range(set_index, &min, &max, factor);

  for (i = min; i < max + 1; i++)
  {
    if (tag_table_ptr[i] == 0)
    {
      ++placement_count;
      tag_table_ptr[i] = tag_value;
      return;
    }
    ++replacement_count;
    tag_table_ptr[min] = tag_value;
  }
}

int compare_set_value(uint32_t set_index, uint32_t tag_value, int factor)
{
  ++access_attempt;
  uint32_t min, max;
  // if a use the tag pointer here instead of the global array, it breaks
  // int search_offset = factor - 1;
  // int min = set_index * factor;
  // int max = min + search_offset;
  get_range(set_index, &min, &max, factor);
  int i = 0;

  for (i = min; i < max + 1; i++)
  {
    if (tag_value == tag_table_ptr[i])
    {
      ++cache_hit;
      return 1;
    }
    else
    {
      ++cache_miss;
      return 0;
    }
  }
}

void process_record_associative(record_t *record, int no_of_ways)
{
  uint32_t tag_bits_value = extract_tag_associative(record->address);
  uint32_t set_bits_value = extract_set_associative(record->address);
  update_stats(record);

  if (!compare_set_value(set_bits_value, tag_bits_value, no_of_ways))
  {
    update_block_with_fifo(set_bits_value, tag_bits_value, no_of_ways);
  }
}

void run_two_way_associative_simulation(FILE *fp, Cache_Line_size_t size, int no_of_ways)
{
  record_t record;
  address_node_t address;
  int width = 1;
  uint32_t tag_block_size = 1;
  uint32_t set_index_size = 1;
  int tag_bit_size;
  int i = 0;

  setup_cache_mem_block(size, &address);

  address.index_bit_size = 16 - address.offset_bit_size;

  int shft_amt = address.index_bit_size - (no_of_ways / 2);
  for (i = 0; i < shft_amt; i++)
  {
    set_index_size = set_index_size << 1;
  }

  for (i = 0; i < address.offset_bit_size; i++)
  {
    width = width << 1;
  }

  tag_bit_size = NO_OF_RAM_ADRESS_BITS - address.offset_bit_size - shft_amt;

  for (int i = 0; i < tag_bit_size; i++)
  {
    tag_block_size = tag_block_size << 1;
  }

  printf("No of tag bits = %d\n", tag_bit_size);
  printf("No of index bits = %d\n", shft_amt);
  printf("No of offset bits = %d\n", address.offset_bit_size);

  allocate_cahe_mem_block(set_index_size, width);
  allocate_tag_table_block(tag_block_size);

  while (1)
  {
    fscanf(fp, "%x %x %x%*[\r\n]", &record.access_type, &record.address,
           &record.data);
    if (feof(fp))
      break;
    process_record_associative(&record, no_of_ways);
  }
  fclose(fp);
  free(tag_table_ptr);
  free(cache_mem_block_ptr);
  print_simulation_result();
}

void run_direct_mapped_simulation(FILE *fp, Cache_Line_size_t size)
{
  printf("simulating cache of size %d KB, block size %d Bytes\n", 64, size);
  record_t record;
  address_node_t address_node;
  line_size = size;
  uint32_t tag_table_size = 1;
  uint32_t memory_width = 1;

  setup_cache_mem_block(size, &address_node);

  address_node.index_bit_size = 16 - address_node.offset_bit_size;

  for (int i = 0; i < address_node.index_bit_size; i++)
  {
    tag_table_size = tag_table_size << 1;
  }

  for (int i = 0; i < address_node.offset_bit_size; i++)
  {
    memory_width = memory_width << 1;
  }

  printf("No of bits for address index bits is %d\n", address_node.index_bit_size);
  printf("No of bits for address offset bits is %d\n", address_node.offset_bit_size);

  allocate_tag_table_block(tag_table_size);
  allocate_cahe_mem_block(tag_table_size, memory_width);

  while (1)
  {
    fscanf(fp, "%x %x %x%*[\r\n]", &record.access_type, &record.address,
           &record.data);
    if (feof(fp))
      break;
    process_record_direct(&record);
  }
  fclose(fp);
  free(tag_table_ptr);
  free(cache_mem_block_ptr);
  print_simulation_result();
}

int main(int argc, char *argv[])
{
  printf("The given argument is %s trace file\n", argv[1]);
  // char *filename = argv[1];

  FILE *fp;

  int size = CACHE_BLOCK_BASE_SIZE;
  for (int i = 0; i < 4; i++)
  {
    fp = fopen("085.gcc.din.txt", "r");
    if (fp)
      run_direct_mapped_simulation(fp, size);
    size = size << 1;
  }

  int k = 8;
  size = CACHE_BLOCK_BASE_SIZE;
  for (int i = 0; i < 4; i++)
  {
    fp = fopen("085.gcc.din.txt", "r");
    if(fp)
      run_two_way_associative_simulation(fp, size, k);
    size = size << 1;
  }
  
  return 0;
}
