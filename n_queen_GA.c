/*
 * please execute me follow the form: /ga queens_counts population_size generation_limit run_times
 * The number of queens must equal to the width of the board
 * The width of the board equal to its length.
 * The population size must be even.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

typedef struct board{
  int* position; // what the board looks like now
  int attack;
}board;

void board_copy(board* target, board* source, int n_size);  // copy struct board; n_size'unit is byte
int cmp(const void *a, const void *b);  // for qsort of struct board


/*
 * n is the number of queens and the board's width
 * p is the population size
 * Return the best result.
 */
int Genetic_Algorithm(int n, int p, int g_limit);


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
 * width is the board's width
 */
void display(int* r, int width);

/*
 * seeder for srand()
 * g_limit is generation_limit
 * not_goal to detect if it get the optimal
 */
int seeds = 0;
int main(int argc, char* argv[])
{
  if(argc != 5)
  {
    fprintf(stderr, "Wrong arguments counts.\n");
    fprintf(stderr, "please input likes the form:\n./ga queens_counts population_size generation_limit run_times\n");
    exit(-1);
  }

  int n = atoi(argv[1]);  // number of queens and the board's width
  int p = atoi(argv[2]);  // population size in GA
  int g_limit = atoi(argv[3]);
  int times = atoi(argv[4]);


  if(p%2)
  {
    fprintf(stderr, "Population size must be even.\n");
    fprintf(stderr, "please input likes the form:\n./ga queens_counts population_size generation_limit run_times\n");
    exit(-1);
  }

  int attack;
  double start, end;
  double total_time=0;
  int total_attack=0;
  int success=0;
  for(int i=0; i<times ;i++)
  {
    start = clock();
    attack = Genetic_Algorithm(n, p, g_limit);
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

void board_copy(board* target, board* source, int n)
{
  target->attack = source->attack;
  memcpy(target->position, source->position, n);
}

int cmp(const void *a, const void *b)
{
  int tmp1 = ((board*) a) -> attack;
  int tmp2 = ((board*) b) -> attack;

  return tmp1 - tmp2;
}

int Genetic_Algorithm(int n, int p_size, int g_limit)
{
  srand(time(NULL) + (seeds++));
  board *population = (board*)malloc(sizeof(board)*p_size);
  board *child1 = (board*)malloc(sizeof(board));
    child1->position = generate(n);
  board *child2 = (board*)malloc(sizeof(board));
    child2->position = generate(n);

  // initialize the population set
  for(int i=0; i<p_size; i++)
  {
    population[i].position = generate(n);
    population[i].attack = get_attack(population[i].position, n);
  }

  // initialize the children set
  int half_p = p_size/2;
  int child_size = ((half_p % 2) == 0) ? half_p : half_p-1;
  board *children = (board*)malloc(sizeof(board) * child_size);
  for(int i=0; i<child_size; i++)
    children[i].position = (int*)malloc(sizeof(int)*n);

  int half_n = n/2;
  int chromosome, crossover, child_idx;
  int parent1, parent2, rand1, rand2;
  int times=0;
  bool not_goal = true;
  int n_size = sizeof(int)*n;

  do{
    times++;
    child_idx = 0;

    for(int now=0; now < half_p-1; now+=2)
    {
      // find parent1
      rand1 = rand()%p_size + 0;
      rand2 = rand()%p_size + 0;
      parent1 = rand1;
      if(population[rand1].attack > population[rand2].attack)
        parent1 = rand2;
      board_copy(child1, &population[parent1], n_size);

      // find parent2
      rand1 = rand()%p_size + 0;
      rand2 = rand()%p_size + 0;
      parent2 = rand1;
      if(population[rand1].attack > population[rand2].attack)
        parent2 = rand2;
      board_copy(child2, &population[parent2], n_size);

      // crossover
      crossover = rand()%n + 1;
      for(chromosome = crossover; chromosome < n; chromosome++)
      {
        (child1 -> position)[chromosome] = (population[parent2].position)[chromosome];
        (child2 -> position)[chromosome] = (population[parent1].position)[chromosome];
      }

      // mutation
      (child1 -> position)[rand() % n] = rand()%n;
      (child2 -> position)[rand() % n] = rand()%n;

      // calculate the attack
      child1->attack = get_attack(child1 -> position, n);
      child2->attack = get_attack(child2 -> position, n);

      // put into children set
      board_copy(&children[child_idx], child1, n_size);  child_idx++;
      board_copy(&children[child_idx], child2, n_size);  child_idx++;
    }

    // survivor is the fittest
    qsort(children, child_size, sizeof(board), cmp);
    child_idx = 0;
    int p_idx = p_size -1;
    while((children[child_idx].attack < population[p_idx].attack) && (p_idx > half_p))
    {
      board_copy(&population[p_idx], &children[child_idx], n_size);
      p_idx--;  child_idx++;
    }

    qsort(population, p_size, sizeof(board), cmp);
    if((population[0].attack) == 0)
      not_goal = false;
  }while(not_goal && g_limit--);

/*
  if(not_goal)
    printf("Failed\n");
  else
    printf("Times:%d Succeed\n", times);
*/

  int attack = population[0].attack;

  free(child1);
  free(child2);
  for(int i=0; i<p_size; i++)
    free(population[i].position);
  free(population);
  for(int i=0; i<child_size; i++)
    free(children[i].position);
  free(children);

  return attack;
}

int* generate(int n)
{
  srand(time(NULL) + (seeds++));
  int *board = (int*)malloc(sizeof(int)*n);
  int i, j, tmp;

  for(i = 0; i < n; i++)
    board[i] = i;

  for (i=0; i<n; i++)
  {
    j = rand()%n + 0;
    tmp = board[i];
    board[i] = board[j];
    board[j] = tmp;
  }

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
