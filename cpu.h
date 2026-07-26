#ifndef CPU_H
#define CPU_H

typedef struct {
  char model_name[128];
  int nb_cores;
  _Atomic unsigned long long usage;
} Cpu;

int init_cpu(Cpu *cpu);
int set_cpu_usage(_Atomic unsigned long long *usage, int looping);
void *cpu_thread(void *usage);
void print_cpu(Cpu *cpu);

#endif // CPU_H
