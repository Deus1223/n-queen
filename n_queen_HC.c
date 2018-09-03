/*
 * please execute me follow the form "./hc queens_numbers run_times"
 * The number of queens must equal to the width of the board
 * The width of the board equal to its length.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/*
 * n is the number of queens and the board's width
 * Return the best result.
 */
int Hill_climbing(int n);


/*
 * Generate an array representing what the board looks like.
 * Retturn the base address of the array.
 */
int* generate(int n);


/* board points to the array which represents what the board looks like now.
 * width is the board's width
 * Return the attack counts.
 */
int get_attack(int *board, int width);

/*
 * Display the diagram of result.
 * r points to the array of result
 * w is the board's width
 */
void display(int* r, int w);

int seeds=0;
int main(int argc, char* argv[])
{
  if(argc != 3)
  {
    fprintf(stderr, "Wrong arguments counts\n");
    exit(-1);
  }

  int n = atoi(argv[1]);  //number of queens and the board's width
  int times = atoi(argv[2]);  // run times

  int attack;
  double start, end;
  double total_time=0;
  int total_attack=0;
  int success=0;
  for(int i=0; i<times ;i++)
  {
    start = clock();
    attack = Hill_climbing(n);
    end = clock();
    printf("attack:%d\ttime:%.7fs\n", attack, (end - start)/ CLOCKS_PER_SEC);
    
    total_time += (end - start)/ CLOCKS_PER_SEC;
    total_attack += attack;
    if(attack==0)
      success++;
  }
  printf("Average attack:\t%.3f\n", (double)total_attack/times);
  printf("Average  time:\t%.7f\n", total_time/times);
  printf("Success rate:\t%.3f\n", (double)success/times);

  return 0;
}

int Hill_climbing(int n)
{
  int *board = generate(n);
  int *best = (int*)malloc(sizeof(int)*n);
  memcpy(best, board, sizeof(int)*n);
  int attack_best = get_attack(board, n);
  int attack_tmp;
  bool is_goal;

  int *tmp = (int*)malloc(sizeof(int)*n);
  do{
    is_goal = true;
    memcpy(board, best, sizeof(int)*n);

    // find throgh sussecors
    for(int colume=0; colume<n; colume++)
    {
      memcpy(tmp, board, sizeof(int)*n);

      for(int i=0; i<n; i++)
      {
        tmp[colume] = ((tmp[colume] + 1) % n);
        attack_tmp = get_attack(tmp, n);

        if(attack_best > attack_tmp)
        {
          is_goal = false;
          memcpy(best, tmp, sizeof(int)*n);
          attack_best = attack_tmp;
        }
      }
    }
  }while(!is_goal);

  free(board);
  free(tmp);
  free(best);
 // printf("Hill climbing's result: %d\n", attack_best);
  return attack_best;
}

int* generate(int n)
{
  srand(time(NULL) + (seeds++));
  int *board = (int*)malloc(sizeof(int)*n);
  int i, j, tmp;

  for (i=0; i<n; i++)
    board[i]= rand()%n;


  return board;
}

int get_attack(int *board, int width)
{
  int attack=0;
  int offset;

  for (int now = 0; now < width - 1; now++)
  {
    for (int next = now+1; next < width; next++)
    {
      if(board[next] == board[now])
        attack++;
      else
      {
        offset = next - now;
        if ((board[next] == board[now]+offset) || (board[next] == board[now]-offset))
          attack++;
      }
    }
  }

  return attack;
}

void display(int* r, int w)
{
  int i;

  printf(" ");
  for (i=0; i<w; i++)
    printf(" %d", i);
  printf("\n");

  for (int row = w-1; row >= 0; row--)
  {
    printf("%d", row);
    for (int colume=0; colume<w; colume++)
    {
      if(r[colume] == row)
        printf("|+");
      else
        printf("| ");
    }
    printf("|\n");
  }

  printf(" ");
  for (i=0; i<w; i++)
    printf(" %d", i);
  printf("\n");
}
