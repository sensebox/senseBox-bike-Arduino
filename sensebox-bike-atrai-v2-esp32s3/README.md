# Results of the performance tests

## Test 1: Distance, Overtaking Prediction
Threading done without mutex

**senseBox MCUS2**
- linear: 83ms
- threading: 230ms

**Adafruit Feather ESP32S3**
- linear: 65ms
- threading: 116ms

## Test 2: Distance, Overtaking Prediction, Surface Classification, BLE
Threading with mutex

**senseBox MCUS2**
- linear: 86ms
- threading: 184ms

**Adafruit Feather ESP32S3**
- linear: 68ms
- threading: 123ms

# Some lessons learnt
- I had to rename the edge impulse library (the previous name was too long)
- ESP32 BLE
    - expects different format for characteristic/service ids
    - no need to actively regularly poll, it does that on its own somehow

# Open Issues
- How to set and get BLE device id like its done for senseBox?