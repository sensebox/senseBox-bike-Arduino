#include "particulate_matter.hpp"

void init_sps30() {
    Wire.begin(2,1);
    sensirion_i2c_init();
    sps30_set_fan_auto_cleaning_interval_days(4);
    sps30_start_measurement();
}
bool read_sps30(float& pm1_0, float& pm2_5, float& pm4_0, float& pm10) {
    sps30_measurement m;
    uint16_t data_ready;
    int16_t ret;

    do {
        ret = sps30_read_data_ready(&data_ready);
        if (ret < 0) {
            return false;
        } else if (!data_ready)  {
            // wait
        } else
            break;
        delay(100); /* retry in 100ms */
    } while (1);

    ret = sps30_read_measurement(&m);
    if (ret < 0) {
        return false;
    }

    pm1_0 = m.mc_1p0;
    pm2_5 = m.mc_2p5;
    pm4_0 = m.mc_4p0;
    pm10  = m.mc_10p0;
    return true;
}