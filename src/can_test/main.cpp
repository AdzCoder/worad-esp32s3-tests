/**
 * @file main.cpp
 * @brief ESP32-S3 CAN Bus Communication Test using TWAI driver
 * @author Adil Wahab Bhatti
 * @date 2025
 *
 * This program demonstrates basic CAN bus communication on an ESP32-S3
 * using the Two-Wire Automotive Interface (TWAI) driver. It periodically
 * transmits test messages and listens for incoming CAN frames.
 */

#include <Arduino.h>

#include "driver/twai.h"

/// GPIO pin for CAN TX
#define CAN_TX 11
/// GPIO pin for CAN RX
#define CAN_RX 10

/// CAN message transmission interval in milliseconds
#define TX_INTERVAL_MS 1000

/// CAN message identifier for transmitted messages
static const uint32_t TX_MESSAGE_ID = 0x123;

/**
 * @brief Initialize TWAI driver and CAN bus communication
 *
 * Configures the TWAI peripheral with 1Mbps speed in NO_ACK mode
 * for testing without requiring other devices on the bus.
 */
void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("ESP32-S3 CAN Test Starting...");

  // Configure TWAI general settings (NO_ACK mode for solo testing)
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(
      (gpio_num_t)CAN_TX, (gpio_num_t)CAN_RX, TWAI_MODE_NO_ACK);

  // Configure TWAI timing for 1Mbps
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();

  // Configure TWAI filter to accept all messages
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install and start TWAI driver
  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    Serial.println("Failed to install TWAI driver");
    while (1) delay(1000);
  }
  Serial.println("TWAI driver installed");

  if (twai_start() != ESP_OK) {
    Serial.println("Failed to start TWAI driver");
    while (1) delay(1000);
  }
  Serial.println("TWAI driver started - CAN bus ready!");
}

/**
 * @brief Main program loop - transmit and receive CAN messages
 *
 * Transmits a test message with ID 0x123 containing "TEST" followed
 * by four bytes (0x01-0x04). Attempts to receive any incoming messages.
 */
void loop() {
  // Prepare CAN message
  twai_message_t tx_message;
  tx_message.identifier = TX_MESSAGE_ID;
  tx_message.data_length_code = 8;
  tx_message.extd = 0;  // Standard frame
  tx_message.rtr = 0;   // Data frame
  tx_message.data[0] = 'T';
  tx_message.data[1] = 'E';
  tx_message.data[2] = 'S';
  tx_message.data[3] = 'T';
  tx_message.data[4] = 0x01;
  tx_message.data[5] = 0x02;
  tx_message.data[6] = 0x03;
  tx_message.data[7] = 0x04;

  // Transmit message
  if (twai_transmit(&tx_message, pdMS_TO_TICKS(1000)) == ESP_OK) {
    Serial.println("Message transmitted successfully");
  } else {
    Serial.println("Failed to transmit message");
  }

  // Attempt to receive message
  twai_message_t rx_message;
  if (twai_receive(&rx_message, pdMS_TO_TICKS(100)) == ESP_OK) {
    Serial.print("Received message ID: 0x");
    Serial.print(rx_message.identifier, HEX);
    Serial.print(" Data: ");
    for (int i = 0; i < rx_message.data_length_code; i++) {
      Serial.printf("%02X ", rx_message.data[i]);
    }
    Serial.println();
  }

  delay(TX_INTERVAL_MS);
}
