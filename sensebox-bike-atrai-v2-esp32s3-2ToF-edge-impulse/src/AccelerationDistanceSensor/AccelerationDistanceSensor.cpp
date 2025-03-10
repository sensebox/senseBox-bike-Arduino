#include "AccelerationDistanceSensor.h"
#include <edge-impulse-sdk/classifier/ei_run_classifier.h>

AccelerationDistanceSensor::AccelerationDistanceSensor() {}

static SemaphoreHandle_t i2c_mutex;

void AccelerationDistanceSensor::begin()
{
    i2c_mutex = xSemaphoreCreateMutex();
    accInitSensor();
    delay(500);
    distInitSensor();
    delay(500);
    // xTaskCreate(sensorTask, taskName, taskStackSize, this, 1, NULL);
}

void AccelerationDistanceSensor::startSubscription()
{
    if (this->accHandle == NULL || this->distHandle == NULL)
    {
        activeSubscription = true;
        // https://docs.espressif.com/projects/esp-idf/en/v5.2.3/esp32s3/api-guides/performance/speed.html#choosing-task-priorities-of-the-application
        xTaskCreatePinnedToCore(accTask, "accelerationSensorTask", 4096, this, 17, &this->accHandle, 1);
        xTaskCreatePinnedToCore(distTask, "distanceSensorTask", 8192, this, 20, &this->distHandle, 0);
        // very important tasks could run on priority 20 on core 0
    }
}

void AccelerationDistanceSensor::stopSubscription()
{
    if (this->accHandle != NULL)
    {
        vTaskDelete(this->accHandle);
    }
    if (this->distHandle != NULL)
    {
        vTaskDelete(this->distHandle);
    }
    activeSubscription = false;
}

void AccelerationDistanceSensor::startBLE()
{
    sendBLE = true;
}

void AccelerationDistanceSensor::stopBLE()
{
    sendBLE = false;
}


void AccelerationDistanceSensor::accTask(void* pvParameters)
{
    AccelerationDistanceSensor* sensor = static_cast<AccelerationDistanceSensor*>(pvParameters);
    while (true)
    {
        unsigned long prevTime = millis();
        if (sensor->activeSubscription)
        {
            if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE)
            {
                sensor->accReadSensorData();
                xSemaphoreGive(i2c_mutex);
            }
        }

        // if (sensor->taskDelay > 0 && (millis() - prevTime) < sensor->taskDelay)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay - (millis() - prevTime)));
        // }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void AccelerationDistanceSensor::distTask(void* pvParameters)
{
    AccelerationDistanceSensor* sensor = static_cast<AccelerationDistanceSensor*>(pvParameters);
    while (true)
    {
        unsigned long prevTime = millis();
        if (sensor->activeSubscription)
        {
            if (xSemaphoreTake(i2c_mutex, portMAX_DELAY) == pdTRUE)
            {
                sensor->distReadSensorData();
                xSemaphoreGive(i2c_mutex);
            }
        }

        // if (sensor->taskDelay > 0 && (millis() - prevTime) < sensor->taskDelay)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(sensor->taskDelay - (millis() - prevTime)));
        // }
        vTaskDelay(pdMS_TO_TICKS(0));
    }
}


String surfaceClassificationUUID = "b944af10-f495-4560-968f-2f0d18cab521";
// String accUUID = "B944AF10F4954560968F2F0D18CAB522";
String anomalyUUID = "b944af10-f495-4560-968f-2f0d18cab523";
int surfaceClassificationCharacteristic = 0;
int anomalyCharacteristic = 0;

Adafruit_MPU6050 mpu;

void AccelerationDistanceSensor::accInitSensor()
{
    Serial.println("setting up MPU6050...");

  if (!mpu.begin(0x68, &Wire))
  {
    Serial.println("MPU6050 Chip wurde nicht gefunden");
    delay(500);
    return;
  }
  else
  {
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    delay(100);
  };

  surfaceClassificationCharacteristic = BLEModule::createCharacteristic(surfaceClassificationUUID.c_str());
  anomalyCharacteristic = BLEModule::createCharacteristic(anomalyUUID.c_str());
}

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

void AccelerationDistanceSensor::tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void AccelerationDistanceSensor::distInitSensor()
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

int acc_input_frame_size = impulse_440960_0.dsp_input_frame_size;
float* accBuffer = new float[acc_input_frame_size]();
size_t ix = 0;
float probAsphalt = 0.0;
float probCompact = 0.0;
float probPaving = 0.0;
float probSett = 0.0;
float probStanding = 0.0;
float anomaly = 0.0;

static int get_signal_data_440960(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (accBuffer + offset)[i];
    }
    return EIDSP_OK;
}

unsigned long startAccTime = millis();
bool AccelerationDistanceSensor::accReadSensorData()
{
  bool classified = false;
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  accBuffer[ix++] = 1.0;
  accBuffer[ix++] = 1.0;
  accBuffer[ix++] = 1.0;
  accBuffer[ix++] = 1.0;
  accBuffer[ix++] = 1.0;
  accBuffer[ix++] = 1.0;

  unsigned long endAccTime = millis();
  if ((endAccTime - startAccTime) < 30)
  {
    vTaskDelay(pdMS_TO_TICKS(30 - (endAccTime - startAccTime)));
  }
  Serial.printf("acceleration: %lu ms\n", endAccTime - startAccTime);
  startAccTime = millis();

  // one second inverval
  if (acc_input_frame_size <= ix)
  {
    classified = true;
    // Turn the raw buffer in a signal which we can the classify
    signal_t signal;
    signal.total_length = acc_input_frame_size;
    signal.get_data = &get_signal_data_440960;
    // int err = numpy::signal_from_buffer(accBuffer, acc_input_frame_size, &signal);
    // if (err != 0)
    // {
    //   ei_printf("Failed to create signal from buffer (%d)\n", err);
    //   accBuffer[acc_input_frame_size] = {};
    //   return classified;
    // }

    // Run the classifier
    ei_impulse_result_t result = {};

    int err = process_impulse(&impulse_handle_440960_0, &signal, &result, false);
    if (err != EI_IMPULSE_OK)
    {
      ei_printf("ERR: Failed to run classifier (%d)\n", err);
      accBuffer[acc_input_frame_size] = {};
      return classified;
    }

    probAsphalt = result.classification[0].value;
    probCompact = result.classification[1].value;
    probPaving = result.classification[2].value;
    probSett = result.classification[3].value;
    probStanding = result.classification[4].value;

    anomaly = result.anomaly;
    Serial.printf("Asphalt: %f, Compact: %f, Paving: %f, Sett: %f, Standing: %f, Anomaly: %f\n", probAsphalt, probCompact, probPaving, probSett, probStanding, anomaly);

    if (sendBLE)
    {
      accNotifyBLE(probAsphalt, probCompact, probPaving, probSett, probStanding, anomaly);
    }

    ix = 0;

    accBuffer[acc_input_frame_size] = {};

    startAccTime = millis();
  }

  if (measurementCallback)
  {
    measurementCallback({probAsphalt, probCompact, probPaving, probSett, probStanding});
  }

  return classified;
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
bool AccelerationDistanceSensor::distReadSensorData()
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
            distNotifyBLE(distance, overtakingPredictionPercentage);
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
            distNotifyBLERight(distanceRight, overtakingPredictionPercentage);
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

void AccelerationDistanceSensor::accNotifyBLE(float probAsphalt, float probCompact, float probPaving, float probSett, float probStanding, float anomaly)
{
  BLEModule::writeBLE(surfaceClassificationUUID.c_str(), probAsphalt*100, probCompact*100, probPaving*100, probSett*100, probStanding*100);
  BLEModule::writeBLE(anomalyUUID.c_str(), anomaly);
}

void AccelerationDistanceSensor::distNotifyBLE(float distance, float overtakingPredictionPercentage)
{
    BLEModule::writeBLE(distanceUUID.c_str(), distance);
    BLEModule::writeBLE(overtakingUUID.c_str(), overtakingPredictionPercentage);
}

void AccelerationDistanceSensor::distNotifyBLERight(float distanceRight, float overtakingPredictionPercentage)
{
    BLEModule::writeBLE(distanceUUID.c_str(), distanceRight);
    BLEModule::writeBLE(overtakingUUID.c_str(), overtakingPredictionPercentage);
}