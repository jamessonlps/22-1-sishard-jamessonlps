/*
 PROGRAMA UTIL PARA TESTAR O LAB!

 Funcionalidade: este programa espera TIME_SLEEP segundos e finaliza com signal

Para compilar: gcc timecrash.c -o timecrash
 */

#include <stdio.h>
#include <unistd.h>

#define TIME_SLEEP 3

int main() {

    printf("[FILHO]: Execução do TimeCrash iniciada\n");

    sleep(TIME_SLEEP);

    double x = 1 / 0; // DIVISAO POR ZERO!

    return 0;
}
