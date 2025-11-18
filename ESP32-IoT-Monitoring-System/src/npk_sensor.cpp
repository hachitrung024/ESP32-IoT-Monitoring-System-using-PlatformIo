#include "npk_sensor.h"

ModbusMaster npkNode;

void npk_sensor_task(void * pvParameter){
    SERIAL_RS485.begin(SERIAL_RS485_BAUD, SERIAL_RS485_CONFIG, SERIAL_RS485_RX_PIN, SERIAL_RS485_TX_PIN);
    npkNode.begin(NPK_SLAVE_ID, SERIAL_RS485);
    for(;;){
        EventBits_t uxBits = xEventGroupWaitBits(
            xSensorEventGroup,
            SENSOR_TRIGGER_READ_ALL_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY
        );

        uint8_t result;
        uint16_t nitrogen = 0;
        uint16_t phosphorus = 0;
        uint16_t potassium = 0;

        result = npkNode.readHoldingRegisters(REG_ADDR_NITROGEN_CONTENT, 1);
        if (result == npkNode.ku8MBSuccess) {
            nitrogen = npkNode.getResponseBuffer(0);
        } else {
            Serial.printf("[NPK] Read N error: %u\n", result);
            nitrogen = 0;
        }

        result = npkNode.readHoldingRegisters(REG_ADDR_PHOSPHORUS_CONTENT, 1);
        if (result == npkNode.ku8MBSuccess) {
            phosphorus = npkNode.getResponseBuffer(0);
        } else {
            Serial.printf("[NPK] Read P error: %u\n", result);
            phosphorus = 0;
        }

        result = npkNode.readHoldingRegisters(REG_ADDR_POTASSIUM_CONTENT, 1);
        if (result == npkNode.ku8MBSuccess) {
            potassium = npkNode.getResponseBuffer(0);
        } else {
            Serial.printf("[NPK] Read K error: %u\n", result);
            potassium = 0;
        }

        char telemetry_buffer[64];
        snprintf(telemetry_buffer, sizeof(telemetry_buffer),
                 "{\"NPK-N\":%u,\"NPK-P\":%u,\"NPK-K\":%u}",
                 nitrogen, phosphorus, potassium);

        if (xQueueSend(xSensorDataQueue, &telemetry_buffer, 0) != pdPASS) {
            Serial.println("[NPK] Failed to send telemetry data to queue");
        }
    }
}