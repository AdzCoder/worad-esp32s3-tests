/**
 * @file main.cpp
 * @brief CubeMars AK45-36 Motor Test
 * @author Adil
 * @date 2026
 *
 * Basic test to verify motor connection and movement commands over CAN bus.
 */

#include <Arduino.h>

#include "driver/twai.h"

// GPIO Configuration
#define CAN_TX 11
#define CAN_RX 10

// Motor Configuration
#define MOTOR_ID 111
#define MOTOR_KT 0.11f  // Torque constant (Nm/A)

float TARGET_ANGLE = 0.0f;  // degrees

unsigned long lastPositionUpdate = 0;
unsigned long lastOutputTime = 0;
const unsigned long POSITION_UPDATE_INTERVAL = 100;  // ms
const unsigned long OUTPUT_INTERVAL = 100;           // ms

// Motor state variables
float motor_position = 0.0f;
float motor_speed = 0.0f;
float motor_current = 0.0f;
float motor_torque = 0.0f;
int8_t motor_temp = 0;
uint8_t motor_error = 0;
bool data_received = false;

// Send position command
void motor_set_position(uint8_t motor_id, float degrees) {
  uint8_t buffer[4];
  int32_t pos_value = (int32_t)(degrees * 10000.0f);

  buffer[0] = (pos_value >> 24) & 0xFF;
  buffer[1] = (pos_value >> 16) & 0xFF;
  buffer[2] = (pos_value >> 8) & 0xFF;
  buffer[3] = pos_value & 0xFF;

  // Extended CAN ID: motor_id + (command_id << 8)
  // Command 4 = Position mode
  uint32_t can_id = motor_id | (4 << 8);

  twai_message_t msg;
  msg.identifier = can_id;
  msg.data_length_code = 4;
  msg.extd = 1;
  msg.rtr = 0;

  for (int i = 0; i < 4; i++) {
    msg.data[i] = buffer[i];
  }

  twai_transmit(&msg, pdMS_TO_TICKS(100));
}

// Receive and parse motor data (stores in global variables)
void receive_motor_data() {
  twai_message_t rx_msg;

  if (twai_receive(&rx_msg, pdMS_TO_TICKS(100)) == ESP_OK) {
    if (rx_msg.data_length_code == 8) {
      // Parse data according to manual section 4.3.1
      int16_t pos_raw = (rx_msg.data[0] << 8) | rx_msg.data[1];
      int16_t spd_raw = (rx_msg.data[2] << 8) | rx_msg.data[3];
      int16_t cur_raw = (rx_msg.data[4] << 8) | rx_msg.data[5];

      // Convert to actual values and store
      motor_position = pos_raw * 0.1f;          // degrees
      motor_speed = spd_raw * 10.0f;            // ERPM
      motor_current = cur_raw * 0.01f;          // Amps
      motor_torque = motor_current * MOTOR_KT;  // Nm (Torque = Current × KT)
      motor_temp = rx_msg.data[6];
      motor_error = rx_msg.data[7];

      data_received = true;
    }
  }
}

// Output motor data to serial
void output_motor_data() {
  if (data_received) {
    Serial.printf("%.2f,%.1f,%.2f,%.4f,%d,%d\n", motor_position, motor_speed,
                  motor_current, motor_torque, motor_temp, motor_error);
  }
}

// Check for serial input and update target angle
void check_serial_input() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() > 0) {
      float newAngle = input.toFloat();
      TARGET_ANGLE = newAngle;
      Serial.printf("New target angle: %.1f degrees\n", TARGET_ANGLE);

      // Immediately send the new position command
      motor_set_position(MOTOR_ID, TARGET_ANGLE);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Wait for serial monitor to be opened
  while (!Serial) {
    delay(10);
  }
  delay(500);  // Additional delay to ensure serial is ready

  Serial.println("Waiting for CAN bus setup...");

  // Setup CAN bus
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CAN_TX, (gpio_num_t)CAN_RX, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK &&
      twai_start() == ESP_OK) {
    Serial.println("CAN bus ready");
  } else {
    Serial.println("CAN setup failed!");
    while (1) delay(1000);
  }

  delay(500);

  // Set motor to target angle
  Serial.printf("Setting motor to %.1f degrees\n", TARGET_ANGLE);
  motor_set_position(MOTOR_ID, TARGET_ANGLE);
  delay(100);

  Serial.println("Enter a number to set a new target angle\n");
  // Print header
  Serial.println(
      "\nPosition(deg),Speed(ERPM),Current(A),Torque(Nm),Temp(C),Error");
}

void loop() {
  unsigned long currentMillis = millis();

  // Check for new target angle from serial input
  check_serial_input();

  // Periodically send position command to hold the angle
  if (currentMillis - lastPositionUpdate >= POSITION_UPDATE_INTERVAL) {
    motor_set_position(MOTOR_ID, TARGET_ANGLE);
    lastPositionUpdate = currentMillis;
  }

  // Receive motor data every loop iteration
  receive_motor_data();

  // Periodically send output data to serial
  if (currentMillis - lastOutputTime >= OUTPUT_INTERVAL) {
    output_motor_data();
    lastOutputTime = currentMillis;
  }

  delay(20);  // 50Hz update rate
}
