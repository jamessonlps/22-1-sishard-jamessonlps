/*
 PROGRAMA UTIL PARA TESTAR O LAB!

 Funcionalidade: este programa nunca encerra, ou seja, o immortal vai criar um processo para ele,
 e este processo nunca será finalizado (a menos que o immortal seja!)

 Para compilar: gcc infinito.c -o infinito
 */
#include <stdio.h>
#include <unistd.h>

#define TIME_SLEEP 3

int main() {
    printf("[FILHO]: Execução do Infinito iniciada\n");

    while (1) {
        sleep(3);
    }

    return 0;
}
