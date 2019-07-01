
#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

typedef float scalar_t;


typedef float mono_sample_t;

typedef struct {
  mono_sample_t left;
  mono_sample_t right;
} stereo_sample_t;


#define MAX_CODEC_SAMPLE LONG_MAX

typedef struct {
  int32_t left;
  int32_t right;
} codec_sample_t;

stereo_sample_t audioSample(codec_sample_t sample) {
  stereo_sample_t retVal;
  retVal.left  = mono_sample_t(sample.left) / MAX_CODEC_SAMPLE;
  retVal.right = mono_sample_t(sample.right) / MAX_CODEC_SAMPLE;
  return retVal;
}

codec_sample_t codecSample(stereo_sample_t sample) {
  codec_sample_t retVal;
  retVal.left  = sample.left * MAX_CODEC_SAMPLE;
  retVal.right = sample.right * MAX_CODEC_SAMPLE;
  return retVal;
}

#endif
