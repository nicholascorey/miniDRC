#include "wm8731_config.h"
#include "driver/i2s.h"
#include "types.h"

#define MAX_CODEC_SAMPLE LONG_MAX
#define I2S_BCK_PIN 25
#define I2S_WS_PIN 26
#define I2S_OUT_PIN 32
#define I2S_IN_PIN 33

void ws8731_init() {
  wm8731_config();  // Initialize chip over I2C

  // Create I2S transciever config object
  i2s_config_t i2s_config;
  i2s_config.mode                 = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_RX);
  i2s_config.sample_rate          = 48000;
  i2s_config.bits_per_sample      = I2S_BITS_PER_SAMPLE_32BIT;
  i2s_config.channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2s_config.communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB);
  i2s_config.intr_alloc_flags     = 0; // default interrupt priority
  i2s_config.dma_buf_count        = 32;
  i2s_config.dma_buf_len          = 32*8;
  i2s_config.use_apll             = true;

  // Create I2S pin config object
  static const i2s_pin_config_t i2s_pin_config = {
    .bck_io_num           = I2S_BCK_PIN,
    .ws_io_num            = I2S_WS_PIN,
    .data_out_num         = I2S_OUT_PIN,
    .data_in_num          = I2S_IN_PIN
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_config.mode                 = (i2s_mode_t)(I2S_MODE_SLAVE | I2S_MODE_TX);
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);

  i2s_set_pin(I2S_NUM_0, &i2s_pin_config);
  i2s_set_pin(I2S_NUM_1, &i2s_pin_config);
  i2s_set_clk(I2S_NUM_0, 48000, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
  i2s_set_clk(I2S_NUM_1, 48000, I2S_BITS_PER_SAMPLE_32BIT, I2S_CHANNEL_STEREO);
}

void wm8731_write(stereo_sample_t sample) {
  size_t bytes_written;
  esp_err_t ret;

  codec_sample_t conv_sample = codecSample(sample);
  ret = i2s_write(I2S_NUM_1, &conv_sample, sizeof(codec_sample_t), &bytes_written, 100);

  if(ret != ESP_OK) {
    while(1);
  }
  if(bytes_written != sizeof(codec_sample_t)) {
    while(1);
  }
}

stereo_sample_t wm8731_read() {
  codec_sample_t sample;
  size_t bytes_read;
  esp_err_t ret;
  
  ret = i2s_read(I2S_NUM_0, &sample, sizeof(codec_sample_t), &bytes_read, 100);

  if(ret != ESP_OK) {
    while(1);
  }
  if(bytes_read != sizeof(codec_sample_t)) {
    while(1);
  }
  return audioSample(sample);
}
