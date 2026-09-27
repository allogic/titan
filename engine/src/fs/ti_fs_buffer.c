#include <ti_pch.h>

void fs_buffer_load(fs_buffer_t *buffer, fs_file *file) {
  memset(buffer, 0, sizeof(fs_buffer_t));

  fs_file_read(file, &buffer->zero_data, sizeof(uint8_t), 0);
  fs_file_read(file, &buffer->size, sizeof(uint64_t), 0);
  fs_file_read(file, &buffer->buffer_usage_flags, sizeof(VkBufferUsageFlags), 0);
  fs_file_read(file, &buffer->memory_property_flags, sizeof(VkMemoryPropertyFlags), 0);
  fs_file_read(file, &buffer->memory_allocate_flags, sizeof(VkMemoryAllocateFlags), 0);
}
void fs_buffer_store(fs_buffer_t *buffer, fs_file *file) {
  fs_file_write(file, &buffer->zero_data, sizeof(uint8_t), 0);
  fs_file_write(file, &buffer->size, sizeof(uint64_t), 0);
  fs_file_write(file, &buffer->buffer_usage_flags, sizeof(VkBufferUsageFlags), 0);
  fs_file_write(file, &buffer->memory_property_flags, sizeof(VkMemoryPropertyFlags), 0);
  fs_file_write(file, &buffer->memory_allocate_flags, sizeof(VkMemoryAllocateFlags), 0);
}
void fs_buffer_destroy(fs_buffer_t *buffer) {
  memset(buffer, 0, sizeof(fs_buffer_t));
}
