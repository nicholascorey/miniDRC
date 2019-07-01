#include "types.h"

#define DELAY_BUFFER_SIZE 4800


stereo_sample_t scale_sample(stereo_sample_t input, scalr_t scale) {
  stereo_sample_t out;
  out.left = input.left * scale;
  out.right = input.right * scale;
  return out;
}


//////////////////////
// DB conversion
scalr_t db_pow_table_frac[1001];
scalr_t db_pow_table_base[20];

scalr_t get_db_pow_frac(scalr_t db) {
  db = db - floor(db/10.0)*10.0;
  scalr_t frac_i = db*100.0;
  int index = floor(frac_i);
  scalr_t frac = frac_i - scalr_t(index);
  return (db_pow_table_frac[index]*(1.0-frac)) + (db_pow_table_frac[index+1]*frac);
}

scalr_t get_db_pow(scalr_t db) {
  return db_pow_table_base[int(floor(db/10.0))+10] * get_db_pow_frac(db);
}


//////////////////////
// Delay buffer
stereo_sample_t delay_buffer[DELAY_BUFFER_SIZE];
uint32_t buf_index = 0;

stereo_sample_t process_delay(stereo_sample_t input, int delay_amount) {
  delay_amount = min(delay_amount, DELAY_BUFFER_SIZE);
  delay_buffer[buf_index] = input;
  buf_index = (buf_index + 1) % delay_amount;
  return delay_buffer[buf_index];
}


//////////////////////
// Init
void init_audio_proc() {
  for(int i=0; i<1001; i++) {
    db_pow_table_frac[i] = pow(10, scalr_t(i)/1000.0);
  }
  for(int i=0; i<20; i++) {
    db_pow_table_base[i] = pow(10, i-10);
  }

  stereo_sample_t sample = {0, 0};
  for(int i=0; i<DELAY_BUFFER_SIZE; i++) {
    delay_buffer[i] = sample;
  }
}
