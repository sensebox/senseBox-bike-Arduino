#include "DistanceSensor.h"

#include "model_data.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Wire.h>

#include <vl53l8cx_class.h>


DistanceSensor::DistanceSensor() : BaseSensor("distanceTask", 
8192, // taskStackSize,
0, // taskDelay,
20, // taskPriority,
0 // core
) {}

// String distanceUUID = "B3491B60C0F34306A30D49C91F37A62B";
String distanceUUID = "7e14e070-84ea-489f-b45a-e1317364b979";
int distanceCharacteristic = 0;

// String overtakingUUID = "FC01C6882C444965AE18373AF9FED18D";
String overtakingUUID = "2cdf2174-35be-fdc4-4Ca2-6fd173f8b3a8";
int overtakingCharacteristic = 0;

VL53L8CX sensor_vl53l8cx_top(&Wire, -1, -1);
const int kChannelNumber = 64;
const int kFrameNumber = 20;
// const tflite::Model *model = nullptr;
// tflite::MicroInterpreter *interpreter = nullptr;
// TfLiteTensor *model_input = nullptr;
// int input_length;
// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
// constexpr int kTensorArenaSize = 14 * 1024 + 1008;
// uint8_t tensor_arena[kTensorArenaSize];

// A buffer holding the last 20 sets of 8x8 pixels
const int RING_BUFFER_SIZE = 1280;
float save_data[RING_BUFFER_SIZE] = {0.0};
float save_data2[RING_BUFFER_SIZE] = {0.0};
// Most recent position in the save_data buffer
int begin_index = 0;
int begin_index2 = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
bool pending_initial_data2 = true;

int input_frame_size = impulse_587727_0.dsp_input_frame_size;

float* buffer = new float[input_frame_size]();
float* buffer2 = new float[input_frame_size]();

long prevDistanceTime = millis();

#define TCAADDR 0x70

void DistanceSensor::tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void DistanceSensor::initSensor()
{

    distanceCharacteristic = BLEModule::createCharacteristic(distanceUUID.c_str());
    overtakingCharacteristic = BLEModule::createCharacteristic(overtakingUUID.c_str());
    // ------------------------------ setup VL53L8CX ------------------------------
    Serial.println("setting up VL53L8CX...");
    Wire.begin();
    Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz

    // Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
    // sensor_vl53l8cx_top.set_i2c_address(0x51); // need to change address, because default address is shared with other sensor
    // Serial.println(-5);

    // sensor_vl53l8cx_top.begin();
    // Serial.println(-4);
    // sensor_vl53l8cx_top.init();
    // Serial.println(-3);
    // sensor_vl53l8cx_top.set_ranging_frequency_hz(30);
    // Serial.println(-2);
    // sensor_vl53l8cx_top.set_resolution(VL53L8CX_RESOLUTION_8X8);
    // Serial.println(-1);
    // sensor_vl53l8cx_top.start_ranging();
    // Serial.println(0);

    tcaselect(1);
    delay(50);

    sensor_vl53l8cx_top.vl53l8cx_set_i2c_address(0x51); // need to change address, because default address is shared with other sensor

    sensor_vl53l8cx_top.begin();
    sensor_vl53l8cx_top.init_sensor();
    sensor_vl53l8cx_top.vl53l8cx_set_ranging_frequency_hz(30);
    sensor_vl53l8cx_top.vl53l8cx_set_resolution(VL53L8CX_RESOLUTION_8X8);
    sensor_vl53l8cx_top.vl53l8cx_start_ranging();

    tcaselect(0);
    delay(50);

    sensor_vl53l8cx_top.vl53l8cx_set_i2c_address(0x51); // need to change address, because default address is shared with other sensor

    sensor_vl53l8cx_top.begin();
    sensor_vl53l8cx_top.init_sensor();
    sensor_vl53l8cx_top.vl53l8cx_set_ranging_frequency_hz(30);
    sensor_vl53l8cx_top.vl53l8cx_set_resolution(VL53L8CX_RESOLUTION_8X8);
    sensor_vl53l8cx_top.vl53l8cx_start_ranging();
    Wire.setClock(100000); // Sensor has max I2C freq of 1MHz
    // ----------------------------- setup complete -----------------------------
}

static int get_signal_data_587727(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (buffer + offset)[i];
    }
    return EIDSP_OK;
}

static int get_signal_data_587727_2(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (buffer2 + offset)[i];
    }
    return EIDSP_OK;
}

unsigned long startDisTime = millis();
bool DistanceSensor::readSensorData()
{
    Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
    VL53L8CX_ResultsData Results;

    // ------------------- LEFT -------------------

    tcaselect(0);

    uint8_t NewDataReady = 0;
    uint8_t status = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady);

    if ((!status) && (NewDataReady != 0))
    {
        sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
        float overtakingPredictionPercentage = -1.0;
        float distance = -1.0;
        float min = 10000.0;
        for (int j = 0; j < VL53L8CX_RESOLUTION_8X8; j += 8)
        {
            for (int l = 0; l < VL53L8CX_NB_TARGET_PER_ZONE; l++)
            {
                for (int k = (8 - 1); k >= 0; k--)
                {
                    if ((float)(&Results)->target_status[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l] != 255)
                    {
                        if ((float)(&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l] > 2000.0)
                        {
                            save_data[begin_index++] = 0.0;
                        }
                        else
                        {
                            save_data[begin_index++] = (float)(&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l];
                        }
                        float distance = ((&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l]) / 10;
                        if (min > distance)
                        {
                            min = distance;
                        }
                    }
                    else
                    {
                        save_data[begin_index++] = 0.0;
                    }
                }
            }
        }
        distance = (min == 10000.0) ? 0.0 : min;

        // If we reached the end of the circle buffer, reset
        if (begin_index >= (RING_BUFFER_SIZE))
        {
            begin_index = 0;
            // Check if we are ready for prediction or still pending more initial data
            if (pending_initial_data)
            {
                pending_initial_data = false;
            }
        }
        if (!pending_initial_data)
        {
            for (int i = 0; i < input_frame_size; ++i)
            {   
                int ring_array_index = begin_index + i - input_frame_size;
                if (ring_array_index < 0)
                {
                    ring_array_index += (RING_BUFFER_SIZE);
                }
                // normalize
                buffer[i] = save_data[ring_array_index];
            }
            signal_t signal;
            signal.total_length = input_frame_size;
            signal.get_data = &get_signal_data_587727;
            // int err = numpy::signal_from_buffer(buffer, input_frame_size, &signal);
            // if (err != 0)
            // {
            //     ei_printf("Failed to create signal from buffer (%d)\n", err);
            //     buffer[input_frame_size] = {};
            //     delete[] buffer;
            //     return false;
            // }

            // Run the classifier
            ei_impulse_result_t result = {};

            int err = process_impulse(&impulse_handle_587727_0, &signal, &result, false);
            if (err != EI_IMPULSE_OK)
            {
                Serial.printf("ERR: Failed to run classifier (%d)\n", err);
                Serial.printf("buffersize %d\n", input_frame_size);
                ei_printf("ERR: Failed to run classifier (%d)\n", err);
                buffer[input_frame_size] = {};
                return false;
            }
            Serial.printf("Left: %f\n", result.classification[0].value);
        }

        if (measurementCallback)
        {
            measurementCallback({distance, overtakingPredictionPercentage});
        }

        if (sendBLE)
        {
            // Serial.printf("distance: %f, overtaking: %f\n", distance, overtakingPredictionPercentage);
            notifyBLE(distance, overtakingPredictionPercentage);
        }
    }
    // ------------------- RIGHT -------------------
    tcaselect(1);
    uint8_t NewDataReady2 = 0;
    uint8_t status2 = sensor_vl53l8cx_top.vl53l8cx_check_data_ready(&NewDataReady2);

    if ((!status2) && (NewDataReady2 != 0))
    {
        sensor_vl53l8cx_top.vl53l8cx_get_ranging_data(&Results);
        float overtakingPredictionPercentage = -1.0;
        float distanceRight = -1.0;
        float min = 10000.0;
        for (int j = 0; j < VL53L8CX_RESOLUTION_8X8; j += 8)
        {
            for (int l = 0; l < VL53L8CX_NB_TARGET_PER_ZONE; l++)
            {
                for (int k = 0; k < 8; k++)
                {
                    if ((float)(&Results)->target_status[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l] != 255)
                    {
                        if ((float)(&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l] > 2000.0)
                        {
                            save_data2[begin_index2++] = 0.0;
                        }
                        else
                        {
                            save_data2[begin_index2++] = (float)(&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l];
                        }
                        float distance = ((&Results)->distance_mm[(VL53L8CX_NB_TARGET_PER_ZONE * (j + k)) + l]) / 10;
                        if (min > distance)
                        {
                            min = distance;
                        }
                    }
                    else
                    {
                        save_data2[begin_index2++] = 0.0;
                    }
                }
            }
        }
        distanceRight = (min == 10000.0) ? 0.0 : min;
        // If we reached the end of the circle buffer, reset
        if (begin_index2 >= (RING_BUFFER_SIZE))
        {
            begin_index2 = 0;
            // Check if we are ready for prediction or still pending more initial data
            if (pending_initial_data2)
            {
                pending_initial_data2 = false;
            }
        }
        if (!pending_initial_data2)
        {
            for (int i = 0; i < input_frame_size; ++i)
            {   
                int ring_array_index = begin_index2 + i - input_frame_size;
                if (ring_array_index < 0)
                {
                    ring_array_index += (RING_BUFFER_SIZE);
                }
                // normalize
                buffer2[i] = save_data2[ring_array_index];
            }
            signal_t signal2;
            signal2.total_length = input_frame_size;
            signal2.get_data = &get_signal_data_587727_2;
            // if (err != 0)
            // {
            //     ei_printf("Failed to create signal from buffer (%d)\n", err);
            //     buffer2[input_frame_size] = {};
            //     delete[] buffer2;
            //     return false;
            // }

            // Run the classifier
            ei_impulse_result_t result2 = {};

            int err = process_impulse(&impulse_handle_587727_0, &signal2, &result2, false);
            if (err != EI_IMPULSE_OK)
            {
                Serial.printf("ERR: Failed to run classifier (%d)\n", err);
                Serial.printf("buffersize %d\n", input_frame_size);
                ei_printf("ERR: Failed to run classifier (%d)\n", err);
                buffer2[input_frame_size] = {};
                return false;
            }
            Serial.printf("Right: %f\n", result2.classification[0].value);
        }
        if (sendBLE)
        {
            // Serial.printf("distanceRight: %f, overtakingRight: %f\n", distanceRight, overtakingPredictionPercentage);
            notifyBLERight(distanceRight, overtakingPredictionPercentage);
        }
    }
    // -----------------------------------------------------------
    // if ((millis() - prevDistanceTime) < 65)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(65 - (millis() - prevDistanceTime)));
    // }
    // Serial.print("distance: ");
    // Serial.println(millis() - prevDistanceTime);
    prevDistanceTime = millis();
    Wire.setClock(100000); // Sensor has max I2C freq of 1MHz
    return false;
}

void DistanceSensor::notifyBLE(float distance, float overtakingPredictionPercentage)
{
    BLEModule::writeBLE(distanceUUID.c_str(), distance);
    BLEModule::writeBLE(overtakingUUID.c_str(), overtakingPredictionPercentage);
}

void DistanceSensor::notifyBLERight(float distanceRight, float overtakingPredictionPercentage)
{
    BLEModule::writeBLE(distanceUUID.c_str(), distanceRight);
    BLEModule::writeBLE(overtakingUUID.c_str(), overtakingPredictionPercentage);
}