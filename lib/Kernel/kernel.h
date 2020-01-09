/*
 * @kernel.h
 *
 * Header file: protótipos de funções
 *              utilizados pelo kernel
 *
 * Autor: Eduardo Pagotto
 * Data: 2020-01
 * Versão: 0.0.0
 */

#ifndef KERNEL_ZERO_RTOS_H
#define KERNEL_ZERO_RTOS_H

#include <Arduino.h>

// Definições para as tarefas
#define TASKS_SIZE 3
#define RUN_MAX_TIME 100 // 100ms

// Definições gerais
#define SUCCESS 1
#define FAIL 0

// Definição ponteiro de função
typedef void (*ptrFunc)(void);

// Definição da estrutura que contem as informações das tarefas
typedef struct {
    ptrFunc Function;
    unsigned char* taskName;
    uint16_t period;
    bool enableTask;
} TaskHandle;

// Globais
extern volatile uint32_t sysTickCounter;

uint8_t KernelInit(void);
uint8_t KernelAddTask(ptrFunc _function, unsigned char* _nameFunction, uint16_t _period, bool _enableTask,
                      TaskHandle* task);
uint8_t KernelRemoveTask(TaskHandle* task);
void KernelStart(void);
void IsrTimer(void);

#endif