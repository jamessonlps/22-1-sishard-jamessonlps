#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_SIZE_PATH 1024
#define MAX_SIZE_LOG 2048
#define MAX_SIZE_BUFFER 2048 * 200

pid_t cpid;
int num_flags = 0;

typedef struct _proc {
    char path[MAX_SIZE_PATH];
    pid_t cpid;
} proc;

typedef struct _procs {
    int capacity;
    int size;
    proc *data;
} procs;

/**
 * @brief Cria uma struct que armazena processos
 *
 * @param capacity Capacidade inicial de processos
 * @return procs* - Um ponteiro para processos
 */
procs *procs_pid_new(int capacity) {
    procs *prcs = malloc(sizeof(procs));
    prcs->size = 0;
    prcs->capacity = capacity;
    prcs->data = malloc(capacity * sizeof(proc));
    return prcs;
}

/**
 * @brief Insere um processo na struct que armazena processos
 *
 * @param prcs Struct que armazena os processos a serem executados
 * @param cpid pid do processo a ser armazenado
 * @param path path do programa a ser executado
 */
void procs_put(procs *prcs, pid_t cpid, char path[]) {
    if (prcs->capacity == prcs->size) {
        prcs->data = realloc(prcs->data, 2 * prcs->capacity * sizeof(proc));
        prcs->capacity *= 2;
    }
    proc new_proc;
    new_proc.cpid = cpid;
    for (int i = 0; i < strlen(path); i++) {
        new_proc.path[i] = path[i];
    }
    new_proc.path[strlen(path)] = '\0';
    prcs->data[prcs->size] = new_proc;
    prcs->size++;
}

/**
 * @brief Atualiza o pid de um determinado processo
 *
 * @param prcs Struct que armazena os processos a serem executados
 * @param prev_pid pid a ser substituído do processo
 * @param new_pid novo pid do processo
 */
void procs_sub(procs *prcs, pid_t prev_pid, pid_t new_pid) {
    for (int i = 0; i < prcs->size; i++) {
        if (prcs->data[i].cpid == prev_pid) {
            prcs->data[i].cpid = new_pid;
        }
    }
}

/**
 * @brief Retorna o processo dado um determinado pid
 *
 * @param prcs Struct que armazena os processos a serem executados
 * @param cpid pid do processo que deve ser retornado
 * @return proc - processo referente ao pid solicitado
 */
proc procs_get_from_pid(procs *prcs, pid_t cpid) {
    proc ret_proc;
    for (int i = 0; i < prcs->size; i++) {
        if (prcs->data[i].cpid == cpid) {
            ret_proc = prcs->data[i];
        }
    }
    return ret_proc;
}

/**
 * @brief Cria um novo processo e retorna o respectivo pid.
 *
 * @param path Caminho do programa a ser executado no processo
 * @param fd Endereço do arquivo de log
 * @return pid_t - pid do processo criado
 */
pid_t create_process(char path[MAX_SIZE_PATH], int *fd) {
    pid_t c_pid = fork();
    if (c_pid == 0) {
        printf("[FILHO]: Processo filho iniciado com pid = %d\n", getpid());
        char *args[] = {path, NULL};
        execvp(path, args);
        printf("[FILHO]: ***Erro*** Execução do execvp falhou com pid=%d e path='%s'\n", getpid(), path);
        return -1;
    } else {
        /* Escreve log de criação de processo filho */
        if (num_flags > 1) {
            char msg[MAX_SIZE_LOG];
            sprintf(msg, "starting %s (pid=%d)\n", path, c_pid);
            write(*fd, msg, strlen(msg));
        }
        printf("[PAI]:   Processo filho iniciado: cpid=%d, path='%s'\n", c_pid, path);
        return c_pid;
    }
}

/**
 * @brief Faz a leitura do arquivo e inicia a execução de cada
 * um dos processos listados e aramazena em processes
 *
 * @param file_path Caminho do arquivo a ser lido
 * @param processes Struct que armazena os processos em andamento
 * @param fd Endereço do filedescriptor
 */
void read_input(char file_path[MAX_SIZE_PATH], procs *processes, int *fd) {
    int file_desc = open(file_path, O_RDONLY);
    char buffer = '0';        // buffer para leitura do path
    int read_bytes = -1;      // número de bytes lidos do arquivo
    char path[MAX_SIZE_PATH]; // caminho do programa a ser executado
    int size;                 // tamanho do path

    while (read_bytes != 0) {
        buffer = '0';
        size = 0;
        while (buffer != '\n' && read_bytes != 0) {
            read_bytes = read(file_desc, &buffer, 1);
            if (buffer != '\n' && read_bytes != 0) {
                path[size] = buffer;
                size++;
            }
        }
        path[size] = '\0';

        pid_t pid = create_process(path, fd);
        if (pid != -1) {
            procs_put(processes, pid, path);
        }
    }

    close(file_desc);
}

procs *process_list;

void sig_handler(int num) {
    printf("Você deseja cancelar a operação? [Y/y]Sim\n");
    char user_resp[2];
    scanf("%s", user_resp);

    if (user_resp[0] == 'Y' || user_resp[0] == 'y') {
        if (num_flags > 1) {
            for (int i = 0; i < process_list->size; i++) {
                printf("[PAI]: Assassinando filho %d - '%s' para encerrar tudo...\n", process_list->data[i].cpid, process_list->data[i].path);
                kill(process_list->data[i].cpid, SIGKILL);
            }
        } else {
            printf("[PAI]: Assassinando filho único...\n");
            kill(cpid, SIGKILL);
        }
        kill(getpid(), SIGKILL);
    }
}

int main(int argc, char *argv[]) {
    int wstatus;
    pid_t w;

    struct sigaction s;
    s.sa_handler = sig_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, NULL);

    int idx_cmd = -1;
    int idx_log = -1;
    int idx_file_in = -1;

    /* Guarda índices das flags de comando e log e o índice do path */
    for (int i = 0; i < argc; i++) {
        if (strlen(argv[i]) == 2) {
            if (argv[i][1] == 'p' || argv[i][1] == 'f') {
                idx_cmd = i;
                idx_file_in = i + 1;
            } else {
                idx_log = i;
            }
            num_flags++;
        }
    }

    /* Cria o path do arquivo de log caso ele seja passado */
    char path_log[MAX_SIZE_PATH] = "./l.log";

    /* Guarda path do arquivo log quando ele é passado */
    if (num_flags > 1 && argc == 5) {
        strcpy(path_log, argv[idx_log + 1]);
        // Será que aqui preciso colocar o '\0' no final?
        // path_log[strlen(argv[idx_log + 1])] = '\0';
    }

    int file_desc_out;
    if (num_flags > 1) {
        /* Abre arquivo para escrita dos logs */
        file_desc_out = open(path_log, O_RDONLY | O_WRONLY | O_CREAT | O_APPEND, 0700);
        printf("\n[LOG]: Arquivo para gravação do log aberto.\n");
    }

    if (argv[idx_cmd][1] == 'f') {
        process_list = procs_pid_new(4);

        /* Lê arquivo e executa cada um dos processos listados */
        read_input(argv[idx_file_in], process_list, &file_desc_out);

        /* Printa no terminal os processos iniciados */
        printf("\n\tEXIBINDO LISTA DE PROCESSOS EM EXECUÇÃO\n");
        for (int i = 0; i < process_list->size; i++) {
            printf("\tPID %d \tPROCESS %s\n\n", process_list->data[i].cpid, process_list->data[i].path);
        }

        while (1) {
            // sleep(1);
            printf("[PAI]:   Agora eu tô só de boas observando...\n");
            w = wait(&wstatus);
            if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus) || WIFSTOPPED(wstatus)) {
                /* Recupera path do programa encerrado */
                proc new_proc = procs_get_from_pid(process_list, w);
                if (strlen(new_proc.path) > 3) {
                    // sleep(1);
                    if (num_flags > 1) {
                        char msg1[MAX_SIZE_LOG];
                        sprintf(msg1, "program %s (pid=%d) finished (EXITED=%d, EXITSTATUS=%d, SIGNALED=%d, SIGNAL=%d, SIGNALSTR=%s)\n", new_proc.path, w, WIFEXITED(wstatus), WEXITSTATUS(wstatus), WIFSIGNALED(wstatus), WTERMSIG(wstatus), strsignal(WTERMSIG(wstatus)));
                        write(file_desc_out, msg1, strlen(msg1));
                        char msg2[MAX_SIZE_LOG];
                        sprintf(msg2, "restarting %s (oldpid=%d, newpid=%d)\n", new_proc.path, w, new_proc.cpid);
                        write(file_desc_out, msg2, strlen(msg2));
                    }
                    printf("[PAI]:   Processo '%s' finalizado. Reiniciando...\n", new_proc.path);
                    /* Reinicia programa encerrado no processo filho */
                    cpid = create_process(new_proc.path, &file_desc_out);
                    procs_sub(process_list, w, cpid);
                }
            }
            if (w == -1) {
                printf("[PAI]:   Alguma falha inesperada ocorreu no pai. HELP!!\n");
                // sleep(1);
            }
        }

    } else if (argv[idx_cmd][1] == 'p') {
        while (1) {
            // sleep(1);
            cpid = create_process(argv[idx_file_in], &file_desc_out);
            printf("[PAI]:   Processo PAI executando com pid = %d\n", getpid());
            w = wait(&wstatus);
            if (WIFEXITED(wstatus) || WIFSIGNALED(wstatus)) {
                kill(cpid, SIGKILL); // Filho retornando erro depois do handler
                printf("[PAI]:   Filho com cpid = %d encerrado, w = %d\n", cpid, w);
            }
        }
    }
}