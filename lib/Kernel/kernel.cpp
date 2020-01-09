/**
 * @file kernel.cpp
 * @author Eduardo Pagotto (edupagotto@gmail.com)
 * @brief   Gerenciador de tarefas baseado num kernel cooperativo (não preemptivo)
            Para isso, utilizou-se um buffer estático para armazenar as tarefas;
            As tarefas são escalonadas de acordo com a interrupção do Timer 1. Este verifica o tempo restante para
            cada tarefa ser executada. A tarefa que atingir o tempo primeiro, será executada.
            As "prioridades" das tarefas é por ordem de adição no buffer.
 *
 * @version 0.1
 * @date 2020-01-09
 * @copyright Copyright (c) 2020
 */

#include "kernel.h"
#include "avr/wdt.h"
#include <TimerOne.h> // Inclui a biblioteca do timer 1

// Definição do buffer para armazenar as tarefas
TaskHandle* buffer[NUMBER_OF_TASKS];

// Variáveis do kernel
volatile uint32_t taskCounter[NUMBER_OF_TASKS];
volatile int16_t TempoEmExecucao;
volatile uint32_t sysTickCounter;
volatile bool TemporizadorEstourou;
volatile bool TarefaSendoExecutada;

/**
 * @brief Inicializa as variáveis utilizadas pelo kernel, e o temporizador resposável pelo tick
 * @return char
 */
char KernelInit() {
    memset(buffer, 0, sizeof(buffer)); // Inicializa o buffer para funções
    memset((void*)taskCounter, 0, sizeof(taskCounter));

    sysTickCounter = 0;
    TemporizadorEstourou = NAO;
    TarefaSendoExecutada = NAO;

    // Base de tempo para o escalonador
    Timer1.initialize(1000);          // 1ms
    Timer1.attachInterrupt(IsrTimer); // chama vIsrTimer() quando o timer estoura

    return SUCCESS;
}

/**
 * @brief Adiciona uma nova Tarefa ao pool
 *
 * @param _function ponteiro da funcao
 * @param _nameFunction nome da funcao para debug (opcional NULL)
 * @param _period tempo maximo
 * @param _enableTask inicia
 * @param task handle da task
 * @return char
 */
char KernelAddTask(ptrFunc _function, unsigned char* _nameFunction, uint16_t _period, char _enableTask,
                   TaskHandle* task) {
    int i;

    task->Function = _function;
    task->taskName = _nameFunction;
    task->period = _period;
    task->enableTask = _enableTask;

    // Verifica se já existe a tarefa no buffer
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        if ((buffer[i] != NULL) && (buffer[i] == task))
            return SUCCESS;
    }

    // Adiciona a tarefa em um slot vazio
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        if (!buffer[i]) {
            buffer[i] = task;
            return SUCCESS;
        }
    }
    return FAIL;
}

/**
 * @brief de forma contrária a função KernelAddTask, esta função remove uma Tarefa do buffer circular
 *
 * @param task handle da task
 * @return char
 */
char KernelRemoveTask(TaskHandle* task) {
    int i;
    for (i = 0; i < NUMBER_OF_TASKS; i++) {
        if (buffer[i] == task) {
            buffer[i] = NULL;
            return SUCCESS;
        }
    }
    return FAIL;
}

/**
 * @brief função responsável por escalonar as tarefas de acordo com a resposta da interrupção do Timer 1
 *
 */
void KernelStart() {
    int i;

    for (;;) {
        if (TemporizadorEstourou == SIM) {
            for (i = 0; i < NUMBER_OF_TASKS; i++) {
                if (buffer[i] != 0) {
                    if (((sysTickCounter - taskCounter[i]) > buffer[i]->period) && (buffer[i]->enableTask == SIM)) {
                        TarefaSendoExecutada = SIM;
                        TempoEmExecucao = TEMPO_MAXIMO_EXECUCAO;
                        buffer[i]->Function();
                        TarefaSendoExecutada = NAO;
                        taskCounter[i] = sysTickCounter;
                    }
                }
            }
            TemporizadorEstourou = NAO;
        }
    }
}

/**
 * @brief   Trata a Interrupção do timer 1
 *          Decrementa o tempo para executar de cada tarefa
 *          Se uma tafera estiver em execução, decrementa o tempo máximo de execução para reiniciar o MCU caso ocorra
 *          algum travamento
 *
 */
void IsrTimer() {
    TemporizadorEstourou = SIM;
    sysTickCounter++;

    // Conta o tempo em que uma tarefa está em execução
    if (TarefaSendoExecutada == SIM) {
        TempoEmExecucao--;
        if (!TempoEmExecucao) {
            // possivelmente a tarefa travou, então, ativa o WDT para reiniciar o micro
            wdt_enable(WDTO_15MS);
            while (1)
                ;
        }
    }
}
