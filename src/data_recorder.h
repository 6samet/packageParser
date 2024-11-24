#ifndef DATA_RECORDER_H
#define DATA_RECORDER_H

#include <stdint.h>
#include <stdio.h>

#define INITIAL_DATA_CAPACITY 10000

typedef struct {
    float *type1_data;
    float *type2_data;
    uint32_t type1_count;
    uint32_t type2_count;
    uint32_t type1_capacity;
    uint32_t type2_capacity;
} DataRecorder;

void data_recorder_init(DataRecorder *recorder);

void data_recorder_destroy(DataRecorder *recorder);

void data_recorder_record_type1(DataRecorder *recorder, float value);

void data_recorder_record_type2(DataRecorder *recorder, float value);

void data_recorder_save_to_files(const DataRecorder *recorder, const char *type1_filename, const char *type2_filename);

#endif
