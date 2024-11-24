#include "data_recorder.h"
#include <stdlib.h>
#include <stdint.h>

void data_recorder_init(DataRecorder *recorder) {
    recorder->type1_data = (float *) malloc(INITIAL_DATA_CAPACITY * sizeof(float));
    recorder->type2_data = (float *) malloc(INITIAL_DATA_CAPACITY * sizeof(float));
    if (!recorder->type1_data || !recorder->type2_data) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    recorder->type1_count = 0;
    recorder->type2_count = 0;
    recorder->type1_capacity = INITIAL_DATA_CAPACITY;
    recorder->type2_capacity = INITIAL_DATA_CAPACITY;
}

void data_recorder_destroy(DataRecorder *recorder) {
    if (recorder->type1_data) {
        free(recorder->type1_data);
        recorder->type1_data = NULL;
    }
    if (recorder->type2_data) {
        free(recorder->type2_data);
        recorder->type2_data = NULL;
    }
    recorder->type1_count = 0;
    recorder->type2_count = 0;
    recorder->type1_capacity = 0;
    recorder->type2_capacity = 0;
}

void data_recorder_record_type1(DataRecorder *recorder, float value) {
    if (recorder->type1_count >= recorder->type1_capacity) {
        if (recorder->type1_capacity > UINT32_MAX / 2) {
            fprintf(stderr, "Maximum capacity reached for Type1 data!\n");
            exit(EXIT_FAILURE);
        }
        uint32_t const new_capacity = recorder->type1_capacity * 2;
        float *new_data = (float *) realloc(recorder->type1_data, new_capacity * sizeof(float));
        if (!new_data) {
            fprintf(stderr, "Memory reallocation failed for Type1 data!\n");
            exit(EXIT_FAILURE);
        }
        recorder->type1_data = new_data;
        recorder->type1_capacity = new_capacity;
    }
    recorder->type1_data[recorder->type1_count++] = value;
}

void data_recorder_record_type2(DataRecorder *recorder, float value) {
    if (recorder->type2_count >= recorder->type2_capacity) {
        if (recorder->type2_capacity > UINT32_MAX / 2) {
            fprintf(stderr, "Maximum capacity reached for Type2 data!\n");
            exit(EXIT_FAILURE);
        }
        uint32_t const new_capacity = recorder->type2_capacity * 2;
        float *new_data = (float *) realloc(recorder->type2_data, new_capacity * sizeof(float));
        if (!new_data) {
            fprintf(stderr, "Memory reallocation failed for Type2 data!\n");
            exit(EXIT_FAILURE);
        }
        recorder->type2_data = new_data;
        recorder->type2_capacity = new_capacity;
    }
    recorder->type2_data[recorder->type2_count++] = value;
}

void data_recorder_save_to_files(const DataRecorder *recorder, const char *type1_filename, const char *type2_filename) {
    FILE *file1 = fopen(type1_filename, "w");
    if (file1 != NULL) {
        for (uint32_t i = 0; i < recorder->type1_count; i++) {
            fprintf(file1, "%f\n", recorder->type1_data[i]);
        }
        fclose(file1);
    } else {
        fprintf(stderr, "Error occurred while opening the Type1 data file");
    }

    FILE *file2 = fopen(type2_filename, "w");
    if (file2 != NULL) {
        for (uint32_t i = 0; i < recorder->type2_count; i++) {
            fprintf(file2, "%f\n", recorder->type2_data[i]);
        }
        fclose(file2);
    } else {
        fprintf(stderr, "Error occurred while opening the Type2 data file");
    }
}
