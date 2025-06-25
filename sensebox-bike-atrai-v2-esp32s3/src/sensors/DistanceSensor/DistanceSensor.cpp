#include "DistanceSensor.h"

#include "model_data.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Wire.h>

#include <vl53l8cx.h>
// #include <TensorFlowLite_ESP32.h>
// #include "tensorflow/lite/micro/kernels/micro_ops.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/schema/schema_generated.h"
// #include <tensorflow/lite/micro/micro_error_reporter.h>
#include "edge-impulse-sdk/tensorflow/lite/micro/kernels/micro_ops.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/micro_interpreter.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/all_ops_resolver.h"
#include "edge-impulse-sdk/tensorflow/lite/schema/schema_generated.h"
#include <edge-impulse-sdk/tensorflow/lite/micro/micro_error_reporter.h>

DistanceSensor::DistanceSensor() : BaseSensor("distanceTask", 
8192, // taskStackSize,
0, // taskDelay,
20, // taskPriority,
0 // core
) {}

// String distanceUUID = "B3491B60C0F34306A30D49C91F37A62B";
String distanceUUID = "b3491b60-c0f3-4306-a30d-49c91f37a62b";
int distanceCharacteristic = 0;

// String overtakingUUID = "FC01C6882C444965AE18373AF9FED18D";
String overtakingUUID = "fc01c688-2c44-4965-ae18-373af9fed18d";
int overtakingCharacteristic = 0;

VL53L8CX sensor_vl53l8cx_top(&Wire, -1, -1);
const int kChannelNumber = 64;
const int kFrameNumber = 20;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *model_input = nullptr;
int input_length;
// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 14 * 1024 + 1008;
uint8_t tensor_arena[kTensorArenaSize];

// A buffer holding the last 20 sets of 8x8 pixels
const int RING_BUFFER_SIZE = 1280;
float save_data[RING_BUFFER_SIZE] = {0.0};
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;

long prevDistanceTime = millis();

void DistanceSensor::initSensor()
{
    // ------------------------------ setup VL53L8CX ------------------------------
    Serial.println("setting up VL53L8CX...");
    Wire.begin();
    // Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
    uint8_t status = sensor_vl53l8cx_top.begin();
    status = sensor_vl53l8cx_top.init();
    sensor_vl53l8cx_top.set_ranging_frequency_hz(15);
    sensor_vl53l8cx_top.set_resolution(VL53L8CX_RESOLUTION_8X8);
    status = sensor_vl53l8cx_top.start_ranging();
    // -------------------------- setup tensorflow model --------------------------
    Serial.println("setting up tensorflow...");
    model = tflite::GetModel(g_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION)
    {
        Serial.printf("Model provided is schema version %d not equal "
                      "to supported version %d.",
                      model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }
    // static tflite::MicroErrorReporter tflErrorReporter;
    // This imports all operations, which is more intensive, than just importing the ones we need.
    // If we ever run out of storage with a model, we can check here to free some space
    static tflite::AllOpsResolver resolver;
    // Build an interpreter to run the model with.
    static tflite::MicroInterpreter static_interpreter(
        (tflite::Model*)(model), (tflite::MicroOpResolver&)(resolver), (uint8_t *)(tensor_arena), (size_t)(kTensorArenaSize));
        // , (tflite::MicroErrorReporter*)(&tflErrorReporter));
    interpreter = &static_interpreter;
    // Allocate memory from the tensor_arena for the model's tensors.
    interpreter->AllocateTensors(true);
    // Obtain pointer to the model's input tensor.
    model_input = interpreter->input(0);
    if ((model_input->dims->size != 3) || (model_input->dims->data[0] != 1) ||
        (model_input->dims->data[1] != kFrameNumber) ||
        (model_input->dims->data[2] != kChannelNumber) ||
        (model_input->type != kTfLiteFloat32))
    {
        Serial.println(model_input->dims->size);
        Serial.println(model_input->dims->data[0]);
        Serial.println(model_input->dims->data[1]);
        Serial.println(model_input->dims->data[2]);
        Serial.println(model_input->type);
        Serial.println("Bad input tensor parameters in model");
        return;
    }
    input_length = model_input->bytes / sizeof(float);
    // ----------------------------- setup complete -----------------------------

    distanceCharacteristic = BLEModule::createCharacteristic(distanceUUID.c_str());
    overtakingCharacteristic = BLEModule::createCharacteristic(overtakingUUID.c_str());
    Wire.setClock(100000); // Sensor has max I2C freq of 1MHz
}

unsigned long startDisTime = millis();
bool DistanceSensor::readSensorData()
{

    VL53L8CX_ResultsData Results;
    uint8_t NewDataReady = 0;
    Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
    uint8_t status = sensor_vl53l8cx_top.check_data_ready(&NewDataReady);
    Wire.setClock(100000); // back to normal frequency

    float distance = -1.0;

    if ((!status) && (NewDataReady != 0))
    {
        // Serial.println("data ready");
        // Serial.println(millis()-prevDistanceTime);
        prevDistanceTime = millis();
        Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz
        sensor_vl53l8cx_top.get_ranging_data(&Results);
        Wire.setClock(100000); // back to normal frequency
        float overtakingPredictionPercentage = -1.0;
        float oldVl53l8cxMin = -1.0;
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
        oldVl53l8cxMin = (min == 10000.0) ? 0.0 : min;

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
            for (int i = 0; i < input_length; ++i)
            {   
                int ring_array_index = begin_index + i - input_length;
                if (ring_array_index < 0)
                {
                    ring_array_index += (RING_BUFFER_SIZE);
                }
                // normalize
                model_input->data.f[i] = save_data[ring_array_index] / 2000.0;
            }
            // Run inference, and report any error.
            TfLiteStatus invoke_status = interpreter->Invoke();
            if (invoke_status == kTfLiteOk)
            {
                const float *prediction_scores = interpreter->output(0)->data.f;
                overtakingPredictionPercentage = prediction_scores[0];
                // Serial.print(overtakingPredictionPercentage);
                // Serial.print(" ");
            }
        }

        float distance = oldVl53l8cxMin;

        if (measurementCallback)
        {
            measurementCallback({distance, overtakingPredictionPercentage});
        }

        unsigned long endDisTime = millis();
        Serial.printf("distance: %lu ms\n", endDisTime - startDisTime);
        startDisTime = millis();

        if (sendBLE)
        {
            notifyBLE(distance, overtakingPredictionPercentage);
        }
    }
    // if ((millis() - prevDistanceTime) < 65)
    // {
    //     vTaskDelay(pdMS_TO_TICKS(65 - (millis() - prevDistanceTime)));
    // }
    // Serial.print("distance: ");
    // Serial.println(millis() - prevDistanceTime);
    return false;
}

void DistanceSensor::notifyBLE(float distance, float overtakingPredictionPercentage)
{
    BLEModule::writeBLE(distanceUUID.c_str(), distance);
    BLEModule::writeBLE(overtakingUUID.c_str(), overtakingPredictionPercentage);
}