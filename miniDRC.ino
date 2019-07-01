#include "wm8731_io.h"
#include "audio_proc.h"

#define RMS_VOLT_MULTIPLY 3.054393305
#define LINE_REF_RMS_VOLT 1.228
#define LINE_REF_DBU 4.0
#define OUTPUT_SCALE 2.274143302

#define ATTACK 50   // dB/ds
#define RELEASE 5  // dB/ds
#define THRESHOLD -35
#define RATIO 4
#define MAKEUP 10
#define SAMPLE_RATE 48000
#define ADVANCE_LOOKUP_SECONDS 0.01

#define RMS_BUF_SIZE 256
#define GAIN_ADJUSTMENT_INTERVAL 10   // in audio samples

scalr_t cur_gain = -100;
scalr_t new_gain = 0;

const scalr_t release_amount = 1.0 / SAMPLE_RATE * RELEASE * 10.0 * GAIN_ADJUSTMENT_INTERVAL;
const scalr_t attack_amount = 1.0 / SAMPLE_RATE * ATTACK * 10.0 * GAIN_ADJUSTMENT_INTERVAL;

void setup() {
  Serial.begin(115200, SERIAL_8N1);
  Serial.println("\n");

  ws8731_init();

  ////////////////////////////////////////////////////////////////
  /*  This section of code stands as a testament to hours of
   *  debugging, lots of dead-ends, tons of research, and the
   *  broken-ass state of this this processor's I2S hardware
   *  and/or drivers. All I can guess is that the I2S hardware
   *  requires CPU usage of sufficient time AND complexity to
   *  after initialization to properly sync with the source. If
   *  it doesn't sync, the input audio sounds crackly.  A delay()
   *  doesn't work, putting this section of code at the end of
   *  the init function doesn't work, changing the init timing
   *  doesn't work.
   *  
   *  Stand ye at the foot of this shrine and tremble in awe of
   *  this wretched kludge.
   */
  for(int i=0; i<10000; i++) {
    sqrt(i);
  }
  ////////////////////////////////////////////////////////////////

//  Serial.println("Testing");
//  stereo_sample_t sample;
//  for(int i=0; i<50000; i++) {
//    sample = wm8731_read();
//    wm8731_write(scale_sample(sample, 0.003));
//  }
//  Serial.println("Done");

  Serial.println("Initialized");
}

bool first_run = true;

void loop() {
  static int serial_count;
  static int delay_samples;

  if(first_run) {
    first_run = false;
    init_audio_proc();
    serial_count = 0;
    delay_samples = int(ADVANCE_LOOKUP_SECONDS * SAMPLE_RATE);
  }

  stereo_sample_t sample;
  stereo_sample_t delay_sample;
  stereo_sample_t out_sample;
  stereo_sample_t rms;
  scalr_t drc_scale;
  
  rms.left = 0;
  rms.right = 0;

  for(int i=0; i<RMS_BUF_SIZE; i++) {
    sample = wm8731_read();
    delay_sample = process_delay(sample, delay_samples);

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

    wm8731_write(scale_sample(delay_sample, drc_scale));

    rms.left += sample.left * sample.left;
    rms.right += sample.right * sample.right;
  }
  rms.left = sqrt(rms.left / RMS_BUF_SIZE) * RMS_VOLT_MULTIPLY;
  rms.right = sqrt(rms.right / RMS_BUF_SIZE) * RMS_VOLT_MULTIPLY;
  rms.left = 20 * log10(rms.left / 0.775);
  rms.right = 20 * log10(rms.right / 0.775);

  new_gain = max(rms.left, rms.right) - THRESHOLD;        // find amplitude over threshold
  new_gain = max(new_gain, scalr_t(0.0)) / RATIO * (RATIO - 1.0);  // find amplitude to reduce signal by
  new_gain = -new_gain + MAKEUP;                                     // apply makeup gain

  serial_count = (serial_count + 1) % 2;
  if(serial_count == 0) {
    Serial.printf("%.5f, %.5f\n", new_gain, cur_gain);
  }
}
