#include <ti_pch.h>

void fs_image_load(fs_image_t *image, fs_file *file) {
  memset(image, 0, sizeof(fs_image_t));

  fs_file_read(file, image->name, TI_PATH_SIZE, 0);
  fs_file_read(file, &image->width, sizeof(uint32_t), 0);
  fs_file_read(file, &image->height, sizeof(uint32_t), 0);
  fs_file_read(file, &image->depth, sizeof(uint32_t), 0);
  fs_file_read(file, &image->mip_levels, sizeof(uint32_t), 0);
  fs_file_read(file, &image->format, sizeof(VkFormat), 0);
  fs_file_read(file, &image->image_layout, sizeof(VkImageLayout), 0);
  fs_file_read(file, &image->image_type, sizeof(VkImageType), 0);
  fs_file_read(file, &image->image_tiling, sizeof(VkImageTiling), 0);
  fs_file_read(file, &image->image_view_type, sizeof(VkImageViewType), 0);
  fs_file_read(file, &image->image_usage_flags, sizeof(VkImageUsageFlags), 0);
  fs_file_read(file, &image->image_aspect_flags, sizeof(VkImageAspectFlags), 0);
  fs_file_read(file, &image->memory_property_flags, sizeof(VkMemoryPropertyFlags), 0);
  fs_file_read(file, &image->memory_allocate_flags, sizeof(VkMemoryAllocateFlags), 0);
}
void fs_image_store(fs_image_t *image, fs_file *file) {
  fs_file_write(file, image->name, TI_PATH_SIZE, 0);
  fs_file_write(file, &image->width, sizeof(uint32_t), 0);
  fs_file_write(file, &image->height, sizeof(uint32_t), 0);
  fs_file_write(file, &image->depth, sizeof(uint32_t), 0);
  fs_file_write(file, &image->mip_levels, sizeof(uint32_t), 0);
  fs_file_write(file, &image->format, sizeof(VkFormat), 0);
  fs_file_write(file, &image->image_layout, sizeof(VkImageLayout), 0);
  fs_file_write(file, &image->image_type, sizeof(VkImageType), 0);
  fs_file_write(file, &image->image_tiling, sizeof(VkImageTiling), 0);
  fs_file_write(file, &image->image_view_type, sizeof(VkImageViewType), 0);
  fs_file_write(file, &image->image_usage_flags, sizeof(VkImageUsageFlags), 0);
  fs_file_write(file, &image->image_aspect_flags, sizeof(VkImageAspectFlags), 0);
  fs_file_write(file, &image->memory_property_flags, sizeof(VkMemoryPropertyFlags), 0);
  fs_file_write(file, &image->memory_allocate_flags, sizeof(VkMemoryAllocateFlags), 0);
}
void fs_image_destroy(fs_image_t *image) {
  memset(image, 0, sizeof(fs_image_t));
}
