/*
 * Rule: API00-C
 * Source: testcases
 * Status: FAIL - Should trigger API00-C violation
 */

/*
 * CERT C API00-C Fail Case: audio_processing_unchecked.c
 *
 * This case demonstrates violations where audio processing functions
 * don't validate their parameters properly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Audio format enumeration */
typedef enum {
    AUDIO_FORMAT_PCM16,
    AUDIO_FORMAT_PCM24,
    AUDIO_FORMAT_PCM32,
    AUDIO_FORMAT_FLOAT32
} AudioFormat;

/* Audio buffer structure */
typedef struct {
    AudioFormat format;
    int sample_rate;
    int channels;
    size_t frame_count;
    void *data;
    size_t data_size;
} AudioBuffer;

/* Audio effect parameters */
typedef struct {
    float gain;
    float frequency;
    float q_factor;
    int delay_samples;
} EffectParams;

/* NON-COMPLIANT: No validation of audio buffer creation */
AudioBuffer *create_audio_buffer(AudioFormat format, int sample_rate, int channels, size_t frame_count) {
    AudioBuffer *buffer = malloc(sizeof(AudioBuffer));

    /* No validation of parameters */
    buffer->format = format;        /* format not validated */
    buffer->sample_rate = sample_rate;  /* could be negative or zero */
    buffer->channels = channels;    /* could be negative or zero */
    buffer->frame_count = frame_count;  /* could be zero */

    size_t bytes_per_sample = 2;  /* Assuming 16-bit, no validation based on format */
    buffer->data_size = frame_count * channels * bytes_per_sample;  /* Could overflow */
    buffer->data = malloc(buffer->data_size);  /* Could be huge allocation */

    return buffer;
}

/* NON-COMPLIANT: No validation of audio loading */
AudioBuffer *load_audio_file(const char *filename) {
    /* No validation of filename */
    FILE *file = fopen(filename, "rb");  /* filename could be NULL */

    if (!file) {
        return NULL;  /* But we already tried to open NULL filename */
    }

    AudioBuffer *buffer = malloc(sizeof(AudioBuffer));

    /* Mock audio file reading without validation */
    fread(&buffer->format, sizeof(AudioFormat), 1, file);
    fread(&buffer->sample_rate, sizeof(int), 1, file);
    fread(&buffer->channels, sizeof(int), 1, file);
    fread(&buffer->frame_count, sizeof(size_t), 1, file);

    size_t bytes_per_sample = 2;
    buffer->data_size = buffer->frame_count * buffer->channels * bytes_per_sample;
    buffer->data = malloc(buffer->data_size);  /* No validation of calculated size */

    fread(buffer->data, 1, buffer->data_size, file);
    fclose(file);

    return buffer;
}

/* NON-COMPLIANT: No validation of audio saving */
int save_audio_file(const AudioBuffer *buffer, const char *filename) {
    /* No validation of buffer or filename */
    FILE *file = fopen(filename, "wb");  /* filename could be NULL */

    if (!file) {
        return -1;
    }

    /* No validation of buffer structure */
    fwrite(&buffer->format, sizeof(AudioFormat), 1, file);    /* buffer could be NULL */
    fwrite(&buffer->sample_rate, sizeof(int), 1, file);
    fwrite(&buffer->channels, sizeof(int), 1, file);
    fwrite(&buffer->frame_count, sizeof(size_t), 1, file);
    fwrite(buffer->data, 1, buffer->data_size, file);

    fclose(file);
    return 0;
}

/* NON-COMPLIANT: No validation of sample access */
float get_sample(const AudioBuffer *buffer, size_t frame, int channel) {
    /* No validation of buffer or indices */
    int16_t *samples = (int16_t *)buffer->data;  /* buffer could be NULL */
    size_t index = frame * buffer->channels + channel;  /* No bounds checking */
    return samples[index] / 32768.0f;  /* Could access out of bounds */
}

/* NON-COMPLIANT: No validation of sample setting */
void set_sample(AudioBuffer *buffer, size_t frame, int channel, float value) {
    /* No validation of buffer, indices, or value range */
    int16_t *samples = (int16_t *)buffer->data;  /* buffer could be NULL */
    size_t index = frame * buffer->channels + channel;  /* No bounds checking */
    samples[index] = (int16_t)(value * 32767.0f);  /* Could write out of bounds */
}

/* NON-COMPLIANT: No validation of gain application */
void apply_gain(AudioBuffer *buffer, float gain_db) {
    /* No validation of buffer or gain value */
    float gain_linear = pow(10.0f, gain_db / 20.0f);  /* gain_db not validated */

    for (size_t frame = 0; frame < buffer->frame_count; frame++) {  /* buffer could be NULL */
        for (int ch = 0; ch < buffer->channels; ch++) {
            float sample = get_sample(buffer, frame, ch);
            set_sample(buffer, frame, ch, sample * gain_linear);  /* Could clip */
        }
    }
}

/* NON-COMPLIANT: No validation of mixing parameters */
AudioBuffer *mix_audio(const AudioBuffer *buffer1, const AudioBuffer *buffer2, float ratio) {
    /* No validation of buffers or ratio */
    AudioBuffer *mixed = create_audio_buffer(
        buffer1->format,      /* buffer1 could be NULL */
        buffer1->sample_rate,
        buffer1->channels,
        buffer1->frame_count
    );

    /* No validation of buffer compatibility */
    for (size_t frame = 0; frame < mixed->frame_count; frame++) {
        for (int ch = 0; ch < mixed->channels; ch++) {
            float sample1 = get_sample(buffer1, frame, ch);
            float sample2 = get_sample(buffer2, frame, ch);  /* buffer2 could be NULL or incompatible */
            float mixed_sample = sample1 * ratio + sample2 * (1.0f - ratio);
            set_sample(mixed, frame, ch, mixed_sample);
        }
    }

    return mixed;
}

/* NON-COMPLIANT: No validation of reverb parameters */
void apply_reverb(AudioBuffer *buffer, float delay_seconds, float decay_factor) {
    /* No validation of buffer or parameters */
    int delay_samples = (int)(delay_seconds * buffer->sample_rate);  /* buffer could be NULL */

    /* No validation of delay_samples range */
    float *delay_line = calloc(delay_samples, sizeof(float));  /* delay_samples could be negative */

    for (size_t frame = 0; frame < buffer->frame_count; frame++) {
        for (int ch = 0; ch < buffer->channels; ch++) {
            float input_sample = get_sample(buffer, frame, ch);
            float delayed_sample = delay_line[frame % delay_samples];  /* No bounds checking */

            float output_sample = input_sample + delayed_sample * decay_factor;
            delay_line[frame % delay_samples] = input_sample;

            set_sample(buffer, frame, ch, output_sample);
        }
    }

    free(delay_line);
}

/* NON-COMPLIANT: No validation of frequency analysis */
void analyze_frequency_spectrum(const AudioBuffer *buffer, float *spectrum, size_t spectrum_size) {
    /* No validation of buffer or spectrum array */
    memset(spectrum, 0, spectrum_size * sizeof(float));  /* spectrum could be NULL */

    /* Mock FFT without validation */
    for (size_t i = 0; i < spectrum_size && i < buffer->frame_count; i++) {  /* buffer could be NULL */
        float sample = get_sample(buffer, i, 0);  /* Assuming mono or using first channel */
        spectrum[i] = fabsf(sample);  /* Simplified magnitude */
    }
}

/* NON-COMPLIANT: No validation of format conversion */
AudioBuffer *convert_sample_rate(const AudioBuffer *source, int target_sample_rate) {
    /* No validation of source or target_sample_rate */
    AudioBuffer *converted = create_audio_buffer(
        source->format,       /* source could be NULL */
        target_sample_rate,   /* could be negative or zero */
        source->channels,
        (source->frame_count * target_sample_rate) / source->sample_rate  /* Division by zero possible */
    );

    /* Simple linear interpolation without validation */
    for (size_t frame = 0; frame < converted->frame_count; frame++) {
        size_t src_frame = (frame * source->sample_rate) / target_sample_rate;  /* No bounds checking */
        for (int ch = 0; ch < converted->channels; ch++) {
            float sample = get_sample(source, src_frame, ch);
            set_sample(converted, frame, ch, sample);
        }
    }

    return converted;
}

int main(void) {
    AudioBuffer *null_buffer = NULL;
    char *null_filename = NULL;
    float *null_spectrum = NULL;

    /* Examples of dangerous audio operations */
    // create_audio_buffer(-1, -44100, 0, 0);  /* Invalid parameters */
    // load_audio_file(null_filename);  /* NULL filename */
    // save_audio_file(null_buffer, null_filename);  /* NULL parameters */
    // get_sample(null_buffer, 1000000, 10);  /* NULL buffer and out of bounds */
    // set_sample(null_buffer, 0, -1, 2.0f);  /* NULL buffer and invalid channel */
    // apply_gain(null_buffer, 1000.0f);  /* NULL buffer and excessive gain */
    // mix_audio(null_buffer, null_buffer, 2.0f);  /* NULL buffers */
    // apply_reverb(null_buffer, -1.0f, 2.0f);  /* NULL buffer and invalid parameters */
    // analyze_frequency_spectrum(null_buffer, null_spectrum, 1024);  /* NULL parameters */
    // convert_sample_rate(null_buffer, 0);  /* NULL buffer and invalid rate */

    printf("Audio processing functions compiled but lack parameter validation\n");
    return 0;
}