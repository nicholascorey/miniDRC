#include "audio_proc.h"
#include "wm8731_io.h"


scalar_t db_pow_table_frac[1001];
scalar_t db_pow_table_base[20];

void audio_proc_init() {
  for(int i=0; i<1001; i++) {
    db_pow_table_frac[i] = pow(10, scalar_t(i)/1000.0);
  }
  for(int i=0; i<20; i++) {
    db_pow_table_base[i] = pow(10, i-10);
  }
}


scalar_t get_db_pow_frac(scalar_t db) {
  db = db - floor(db/10.0)*10.0;
  scalar_t frac_i = db*100.0;
  int index = floor(frac_i);
  scalar_t frac = frac_i - scalar_t(index);
  return (db_pow_table_frac[index]*(1.0-frac)) + (db_pow_table_frac[index+1]*frac);
}

scalar_t get_db_pow(scalar_t db) {
  return db_pow_table_base[int(floor(db/10.0))+10] * get_db_pow_frac(db);
}


stereo_sample_t scale_sample(stereo_sample_t input, scalar_t scale) {
  stereo_sample_t out;
  out.left = input.left * scale;
  out.right = input.right * scale;
  return out;
}



#define RMS_BUF_SIZE 128
#define RMS_VOLT_MULTIPLY 3.054393305
#define LINE_REF_RMS_VOLT 1.228
#define LINE_REF_DBU 4.0
#define OUTPUT_SCALE 2.274143302

#define ATTACK 100   // dB/ds
#define RELEASE 5  // dB/ds
#define THRESHOLD -35
#define RATIO 4
#define MAKEUP 0
#define SAMPLE_RATE 48000

#define GAIN_ADJUSTMENT_INTERVAL 100   // in audio samples

scalar_t cur_gain = -100;
scalar_t new_gain = 0;

const scalar_t release_amount = 1.0 / SAMPLE_RATE * RELEASE * 10.0 * GAIN_ADJUSTMENT_INTERVAL;
const scalar_t attack_amount = 1.0 / SAMPLE_RATE * ATTACK * 10.0 * GAIN_ADJUSTMENT_INTERVAL;

void setup() {
  Serial.begin(115200, SERIAL_8N1);
  Serial.printf("\n\nInitializing...\n");
  Serial.printf("audio proc...");
  audio_proc_init();
  Serial.printf("done\n");
  Serial.printf("codec...");
  ws8731_init();
  Serial.printf("done\n");
  Serial.println("Init completed");
}

void loop() {
  stereo_sample_t sample;
  stereo_sample_t out_sample;
  stereo_sample_t rms;
  scalar_t drc_scale;
  
  rms.left = 0;
  rms.right = 0;

  for(int i=0; i<RMS_BUF_SIZE; i++) {
    sample = wm8731_read();

    if(i%GAIN_ADJUSTMENT_INTERVAL==0) {
      bool lowered = false;
      if(cur_gain > new_gain) {
        cur_gain -= attack_amount;
        lowered = true;
      }
      if(cur_gain < new_gain) {
        if(lowered) {
          cur_gain = new_gain;
        }else {
          cur_gain += release_amount;
        }
      }
      drc_scale = get_db_pow(cur_gain / 2.0) * OUTPUT_SCALE;
    }

    wm8731_write(scale_sample(sample, drc_scale));

    rms.left += sample.left * sample.left;
    rms.right += sample.right * sample.right;
  }
  rms.left = sqrt(rms.left / RMS_BUF_SIZE) * RMS_VOLT_MULTIPLY;
  rms.right = sqrt(rms.right / RMS_BUF_SIZE) * RMS_VOLT_MULTIPLY;
  rms.left = 20 * log10(rms.left / 0.775);
  rms.right = 20 * log10(rms.right / 0.775);

  new_gain = max(rms.left, rms.right) - THRESHOLD;        // find amplitude over threshold
  new_gain = max(new_gain, scalar_t(0.0)) / RATIO * (RATIO - 1.0);  // find amplitude to reduce signal by
  new_gain = -new_gain + MAKEUP;                                     // apply makeup gain

//  Serial.printf("%.5f, %.5f\n", new_gain, cur_gain);
}
