// gcc -Wall -Wno-unused-result -g classifier.c knn.o -o classifier -lm -pthread

#include "knn.h"
#include "raylib.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_PATH_FILE 128
#define MAX_NUM_FLAGS 4

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define FPS_RATE 60

#define STATE_INIT 0
#define STATE_TRAIN_DATA 1
#define STATE_SHOW_RESULTS 2
#define STATE_FINISHED 3

/**************************************************************/
/*                         STRUCTS                            */
/**************************************************************/

typedef struct _flags_t {
  char flag;
  int index;
} flags_t;

typedef struct _th_pred_data_t {
  char id;
  t_knn_classifier *model;
  sem_t *semaphore;
  double *data_row;
  long *pred_v;
} th_pred_data_t;

long counter_train = 0; // threads from train executed
long counter_test = 0;  // threads from test executed

/**************************************************************/
/*                        FUNCTIONS                           */
/**************************************************************/

/**
 * @brief Get the flags object
 *
 * @param n number of argsuments inside args
 * @param args pointer to args passed by command line
 * @param flags pointer to store flags passed by command line
 * @param n_flags number os flags received from command line
 */
void get_flags(int n, char *args[], flags_t *flags, int *n_flags) {
  int j = 0;
  for (int i = 0; i < n; i++) {
    if ((strlen(args[i]) == 2) && (args[i][0] == '-')) {
      flags[j].flag = args[i][1];
      flags[j].index = i;
      j++;
    }
  }
  *n_flags = j;
}

void set_flags_actions(flags_t *flags, char *argv[], int n_flags, int *k, char p_train[], char p_test[], int *n_th, sem_t *sem) {
  for (int i = 0; i < n_flags; i++) {
    switch (flags[i].flag) {
    case 'k':
      *k = atoi(argv[flags[i].index + 1]);
      break;
    case 'd':
      strcpy(p_train, argv[flags[i].index + 1]);
      break;
    case 't':
      strcpy(p_test, argv[flags[i].index + 1]);
      break;
    case 'n':
      *n_th = atoi(argv[flags[i].index + 1]);
      sem = malloc(sizeof(sem_t));
      sem_init(sem, 0, *n_th);
      break;
    default:
      break;
    }
  }
}

long get_max(long a, long b) {
  return (a > b ? a : b);
}

/**************************************************************/
/*                       THREADS CONTROL                      */
/**************************************************************/

void *th_predict(void *arg) {
  th_pred_data_t *p = (th_pred_data_t *)arg;

  if (p->semaphore != NULL) {
    sem_wait(p->semaphore);
    *(p->pred_v) = knn_predict(p->model, p->data_row);
    sem_post(p->semaphore);
  } else {
    *(p->pred_v) = knn_predict(p->model, p->data_row);
  }

  if (p->id == 'a') {
    counter_train++;
  } else if (p->id == 'b') {
    counter_test++;
  }

  return NULL;
}

/**************************************************************/
/*                       DISPLAY CONTROL                      */
/**************************************************************/
void draw_infos_from_model(int k, int n) {
  BeginDrawing();

  char k_neights_label[32];
  char n_threads_label[32];
  sprintf(k_neights_label, "k value: %d", k);
  if (n == -1) {
    sprintf(n_threads_label, "n value: not passed");
  } else {
    sprintf(n_threads_label, "n value: %d", n);
  }
  DrawText(k_neights_label, SCREEN_WIDTH - 150, 5, 15, DARKBLUE);
  DrawText(n_threads_label, SCREEN_WIDTH - 150, 25, 15, DARKBLUE);

  EndDrawing();
}

void draw_progress_bars(char label_train[], char label_test[], int progress_train, int progress_test, Color c_label, Color c_bar) {
  int x = 150, y = 150, w = 500, h = 60;
  BeginDrawing();
  ClearBackground(RAYWHITE);

  /* Train progress bar */
  DrawText(label_train, x, y - 30, 20, c_label);
  DrawRectangleLines(x, y, w, h, DARKGRAY);
  DrawRectangle(x, y, progress_train, h, c_bar);
  if (progress_train == w) {
    DrawRectangle(x, y, progress_train, h, LIME);
    DrawText("COMPLETED!", x + 120, y + 10, 40, GREEN);
  }

  /* Test progress bar */
  DrawText(label_test, x, y + h + 30, 20, c_label);
  DrawRectangleLines(x, y + h + 60, w, h, DARKGRAY);
  DrawRectangle(x, y + h + 60, progress_test, h, c_bar);
  if (progress_test == w) {
    DrawRectangle(x, y + h + 60, progress_test, h, LIME);
    DrawText("COMPLETED!", x + 120, y + h + 70, 40, GREEN);
  }

  EndDrawing();
}

/**************************************************************/
/*                        MAIN LOOP                           */
/**************************************************************/

int main(int argc, char *argv[]) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "KNN TRAINING WITH THREADS");
  SetTargetFPS(FPS_RATE);

  int state = STATE_INIT;

  char path_train_csv[SIZE_PATH_FILE];
  char path_test_csv[SIZE_PATH_FILE];
  flags_t *flags = malloc(sizeof(flags_t) * MAX_NUM_FLAGS);
  int num_flags;

  // long data_progress = 0;  // indicates progress from 0 to 500
  long train_progress = 0; // indicates train progress from 0 to 500
  long test_progress = 0;  // indicates test progress from 0 to 500
  // long data_size;          // number os threads executing
  long total_ite; // total of iterations (train + test)

  sem_t *sem_limit = NULL;

  get_flags(argc, argv, flags, &num_flags);

  int k_neights;
  int n_threads = -1; // if flag -n is not passed

  /* Take flags arguments and fill the respective variables */
  set_flags_actions(flags, argv, num_flags, &k_neights, path_train_csv, path_test_csv, &n_threads, sem_limit);

  /* Open trainig dataset and train the model */
  t_data *data_train = read_csv(path_train_csv);
  t_knn_classifier *knn_model = knn_fit(data_train, k_neights);

  /* Allocate vector to store prediction */
  long *pred_train = malloc(sizeof(long) * data_train->n_rows);
  pthread_t *threads = malloc(sizeof(pthread_t) * data_train->n_rows);
  th_pred_data_t *th_data = malloc(sizeof(th_pred_data_t) * data_train->n_rows);

  /* Open test dataset */
  t_data *data_test = read_csv(path_test_csv);
  long *pred_test = malloc(sizeof(long) * data_test->n_rows);
  pthread_t *threads_test = malloc(sizeof(pthread_t) * data_test->n_rows);
  th_pred_data_t *th_data_test = malloc(sizeof(th_pred_data_t) * data_test->n_rows);

  // data_size = data_train->n_rows;
  total_ite = get_max(data_train->n_rows, data_test->n_rows);

  while (!WindowShouldClose()) {
    switch (state) {
    /*----------------- INITIAL EXECUTION ------------------*/
    case STATE_INIT: {
      draw_infos_from_model(k_neights, n_threads);
      BeginDrawing();
      DrawText("PRESS ENTER TO START MODEL PREDICTION", 150, 170, 20, DARKGRAY);
      EndDrawing();

      if (IsKeyPressed(KEY_ENTER)) {
        state = STATE_TRAIN_DATA;
      }
    } break;
    /*----------------- TRAIN DATA EXECUTION ------------------*/
    case STATE_TRAIN_DATA: {

      for (long i = 0; i < total_ite; i++) {
        if (i < data_train->n_rows) {
          th_data[i].id = 'a';
          th_data[i].model = knn_model;
          th_data[i].data_row = data_train->mat[i];
          th_data[i].pred_v = &pred_train[i];
          th_data[i].semaphore = sem_limit;

          pthread_create(&threads[i], NULL, th_predict, &th_data[i]);
        }

        if (i < data_test->n_rows) {
          th_data_test[i].id = 'b';
          th_data_test[i].model = knn_model;
          th_data_test[i].data_row = data_test->mat[i];
          th_data_test[i].pred_v = &pred_test[i];
          th_data_test[i].semaphore = sem_limit;

          pthread_create(&threads_test[i], NULL, th_predict, &th_data_test[i]);
        }

        train_progress = (500 * counter_train) / (data_train->n_rows);
        test_progress = (500 * counter_test) / (data_test->n_rows);

        char train_completed[32];
        char test_completed[32];
        sprintf(train_completed, "TRAIN DATA COMPLETED: %ld%%", train_progress / 5);
        sprintf(test_completed, "TEST DATA COMPLETED: %ld%%", test_progress / 5);

        draw_progress_bars(train_completed, test_completed, train_progress, test_progress, DARKGRAY, SKYBLUE);
      }

      for (long i = 0; i < total_ite; i++) {
        pthread_join(threads[i], NULL); // train data finishes later
      }

      train_progress = (500 * counter_train) / (data_train->n_rows);
      test_progress = (500 * counter_test) / (data_test->n_rows);
      char train_completed[32];
      char test_completed[32];
      sprintf(train_completed, "TRAIN DATA COMPLETED: %ld%%", train_progress / 5);
      sprintf(test_completed, "TEST DATA COMPLETED: %ld%%", test_progress / 5);
      draw_progress_bars(train_completed, test_completed, train_progress, test_progress, DARKGRAY, SKYBLUE);

      state = STATE_SHOW_RESULTS;
    } break;
    /*----------------- SHOWING RESULTS FROM MODEL ------------------*/
    case STATE_SHOW_RESULTS: {
      draw_infos_from_model(k_neights, n_threads);
      BeginDrawing();
      char train_results[64];
      char test_results[64];
      sprintf(train_results, "Accuracy score train: %.3f", accuracy_score(get_target(data_train), pred_train, data_train->n_rows));
      sprintf(test_results, "Accuracy score test: %.3f", accuracy_score(get_target(data_test), pred_test, data_test->n_rows));
      DrawText(train_results, 5, 380, 20, DARKGREEN);
      DrawText(test_results, 5, 420, 20, DARKGREEN);
      DrawText("PRESS Q TO CLOSE THE PROGRAM", 5, 5, 20, RED);
      EndDrawing();

      if (IsKeyPressed(KEY_Q)) {
        state = STATE_FINISHED;
      }
    } break;
    /*----------------- ENDS THE VIEW AND PROGRAM ------------------*/
    case STATE_FINISHED: {
      sem_destroy(sem_limit);

      /* Frees dynamically allocated arrays */
      free(flags);
      free(threads);
      free(th_data);
      free(th_data_test);
      free(threads_test);
      free(pred_train);
      free(pred_test);

      CloseWindow();
    }
    default:
      break;
    }
  }

  return 0;
}
