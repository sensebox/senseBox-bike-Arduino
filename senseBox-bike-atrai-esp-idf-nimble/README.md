### Exemplary implementation of senseBox:bike with esp-idf
Only surface classification is implemented. The classification is done with esp-dl and results are broadcasted via BLE using Nimble.

The implementation is at the moment a mix of the [blehr Nimble BLE example](https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/nimble/blehr) of esp-idf and the [inference example](https://github.com/TinyAIoT/TinyEnergyBench/tree/main/inference/mobilenet) of TinyEnergyBench project.