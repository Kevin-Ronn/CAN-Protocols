#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdio.h>
#include <stdbool.h>

#define CHECK_INTERVAL pdMS_TO_TICKS(1000) // 1-second interval
#define QUEUE_LENGTH 10
#define MESSAGE_SIZE sizeof(Message)

typedef struct {
    char sender[10];
    char receiver[10];
    char message[50];
} Message;

// Queue Handles
QueueSetHandle_t queueSet;
QueueHandle_t motorQueue, ventQueue, fuelQueue;

// Status Check
bool motorStatus = true;  // Motor status: t = OK, f = Error
bool gearboxStatus = true; // Gearbox status: t = OK, f = Error
bool ventStatus = true;   // Ventilation: t = OK, f = Error
float fuelLevel = 15.0;   // Fuel percentage

void MotorControllerTask(void *pvParameters) {
    Message msg;
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Check motor && gearbox
        printf("Checking motor\n");
        if (motorStatus && gearboxStatus) {
            printf("M.G is ok, speed xx and rpm is yyyy\n");
        } else {
            printf("x01:Error: M.||Gb.\n");
        }

        // Queries to ventilation and fuel systems
        snprintf(msg.sender, 10, "Motor");
        snprintf(msg.receiver, 10, "Vent");
        snprintf(msg.message, 50, "Checking vent.");
        xQueueSend(motorQueue, &msg, 0);

        snprintf(msg.receiver, 10, "Fuel");
        snprintf(msg.message, 50, "Checking fuel.");
        xQueueSend(motorQueue, &msg, 0);

        // Read from the queue
        QueueHandle_t activeQueue = xQueueSelectFromSet(queueSet, portMAX_DELAY);
        if (activeQueue != NULL) {
            xQueueReceive(activeQueue, &msg, 0);
            printf("%s -> %s: %s\n", msg.sender, msg.receiver, msg.message);
        }

        // Wait until next cycle
        vTaskDelayUntil(&lastWakeTime, CHECK_INTERVAL);
    }
}

void VentilationTask(void *pvParameters) {
    Message msg;
    for (;;) {
        // Check for messages from motor controller
        if (xQueueReceive(ventQueue, &msg, portMAX_DELAY)) {
            if (ventStatus) {
                snprintf(msg.message, 50, "Vent. Is ok");
                printf("Y*Y*\n");
            } else {
                snprintf(msg.message, 50, "x02:Error: Vent");
                printf("N*N*\n");
            }
            snprintf(msg.sender, 10, "Vent");
            xQueueSend(motorQueue, &msg, 0);
        }
    }
}

void FuelTask(void *pvParameters) {
    Message msg;
    for (;;) {
        // Check for messages from motor controller
        if (xQueueReceive(fuelQueue, &msg, portMAX_DELAY)) {
            if (fuelLevel < 10.0) {
                snprintf(msg.message, 50, "0x3: Low fuel");
                printf("U$U$\n");
            } else {
                snprintf(msg.message, 50, "0x4: Good fuel");
                printf("h$h$\n");
            }
            snprintf(msg.sender, 10, "Fuel");
            xQueueSend(motorQueue, &msg, 0);
        }
    }
}

void main() {
    // Create Queues
    motorQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE);
    ventQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE);
    fuelQueue = xQueueCreate(QUEUE_LENGTH, MESSAGE_SIZE);

    // Create Queue Set
    queueSet = xQueueCreateSet(QUEUE_LENGTH * 3);
    xQueueAddToSet(motorQueue, queueSet);
    xQueueAddToSet(ventQueue, queueSet);
    xQueueAddToSet(fuelQueue, queueSet);

    // Create Tasks
    xTaskCreate(MotorControllerTask, "MotorController", 1000, NULL, 1, NULL);
    xTaskCreate(VentilationTask, "Ventilation", 1000, NULL, 1, NULL);
    xTaskCreate(FuelTask, "Fuel", 1000, NULL, 1, NULL);

    // Start Scheduler
    vTaskStartScheduler();
}
