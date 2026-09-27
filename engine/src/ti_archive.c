#include <ti_pch.h>

static void calc_offset(archive_t *archive);
static void write_header(archive_t *archive, fs_file *file);

void archive_create(archive_t *archive) {
  map_create(&archive->records);
}
uint8_t archive_insert(archive_t *archive, char const *name, void *buffer, uint64_t buffer_size) {
  archive_record_t record = {
    .buffer = TI_ALLOC(buffer_size, 0, buffer),
    .buffer_size = buffer_size,
  };

  strcpy(record.name, name);

  return map_insert(&archive->records, name, strlen(name), &record, sizeof(archive_record_t));
}
uint8_t archive_remove(archive_t *archive, char const *name) {
  return map_remove(&archive->records, name, strlen(name), 0, 0);
}
void archive_clear(archive_t *archive) {
  map_iter_t record_it = map_iter(&archive->records);

  while (map_next(&record_it)) {

    archive_record_t *record = (archive_record_t *)map_value(&record_it);

    if (record->buffer) {
      TI_FREE(record->buffer);
    }
  }

  map_clear(&archive->records);
}
void archive_load(archive_t *archive) {
  fs_file *file = 0;

  uint64_t record_index = 0;
  uint64_t record_count = 0;

  if (fs_file_open(g_fs, archive->file_path, FS_READ, &file) == FS_SUCCESS) {

    fs_file_read(file, &record_count, sizeof(uint64_t), 0);

    while (record_index < record_count) {

      archive_record_t record = {0};

      fs_file_read(file, record.name, TI_PATH_SIZE, 0);
      fs_file_read(file, &record.buffer_size, sizeof(uint64_t), 0);
      fs_file_read(file, &record.global_offset, sizeof(uint64_t), 0);

      map_insert(&archive->records, record.name, strlen(record.name), &record, sizeof(archive_record_t));

      record_index++;
    }

    map_iter_t record_it = map_iter(&archive->records);

    while (map_next(&record_it)) {

      archive_record_t *record = (archive_record_t *)map_value(&record_it);

      fs_file_seek(file, record->global_offset, FS_SEEK_SET);

      record->buffer = TI_ALLOC(record->buffer_size, 0, 0);

      fs_file_read(file, record->buffer, record->buffer_size, 0);
    }

    fs_file_close(file);
  }
}
void archive_store(archive_t *archive) {
  fs_file *file = 0;

  if (fs_file_open(g_fs, archive->file_path, FS_WRITE, &file) == FS_SUCCESS) {

    calc_offset(archive);
    write_header(archive, file);

    map_iter_t record_it = map_iter(&archive->records);

    while (map_next(&record_it)) {

      archive_record_t *record = (archive_record_t *)map_value(&record_it);

      fs_file_write(file, record->buffer, record->buffer_size, 0);
    }

    fs_file_close(file);
  }
}
void archive_destroy(archive_t *archive) {
  map_iter_t record_it = map_iter(&archive->records);

  while (map_next(&record_it)) {

    archive_record_t *record = (archive_record_t *)map_value(&record_it);

    if (record->buffer) {
      TI_FREE(record->buffer);
    }
  }

  map_destroy(&archive->records);
}

static void calc_offset(archive_t *archive) {
  uint64_t record_count = map_count(&archive->records);
  uint64_t record_size = TI_PATH_SIZE + sizeof(uint64_t) * 2;
  uint64_t global_offset = sizeof(uint64_t) + record_size * record_count;

  map_iter_t record_it = map_iter(&archive->records);

  while (map_next(&record_it)) {

    archive_record_t *record = (archive_record_t *)map_value(&record_it);

    record->global_offset = global_offset;
    global_offset += record->buffer_size;
  }
}
static void write_header(archive_t *archive, fs_file *file) {
  uint64_t record_count = map_count(&archive->records);

  fs_file_write(file, &record_count, sizeof(uint64_t), 0);

  map_iter_t record_it = map_iter(&archive->records);

  while (map_next(&record_it)) {

    archive_record_t *record = (archive_record_t *)map_value(&record_it);

    fs_file_write(file, record->name, TI_PATH_SIZE, 0);
    fs_file_write(file, &record->buffer_size, sizeof(uint64_t), 0);
    fs_file_write(file, &record->global_offset, sizeof(uint64_t), 0);
  }
}
