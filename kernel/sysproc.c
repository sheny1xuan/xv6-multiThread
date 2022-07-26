#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_clone(void)
{
    uint64 func;
    uint64 arg;
    uint64 stk;
    argaddr(0, &func);
    argaddr(1, &arg);
    argaddr(2, &stk);
    return clone((void(*)(void*))func, (void*)arg, (void*)stk);
//   return fork();
}

uint64
sys_join(void)
{
  int tid;
  argint(0, &tid);
  printf("join tid(%d) for %d\n", tid, myproc()->pid);
  return join(tid);
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_sem_create(void) {
    int n_sem;
    if (argint(0, &n_sem) < 0) {
        return -1;
    }

    for (int i = 0; i < SEM_MAX_NUM; i++) {
        acquire(&sems[i].lock);
        if (sems[i].allocated == 0) {
            sems[i].allocated = 1;
            sems[i].resource_count = n_sem;

            release(&sems[i].lock);
            return i;
        }
        release(&sems[i].lock);
    }

    return -1;
}

uint64
sys_sem_free(void) {
    int id;
    if (argint(0, &id) < 0) {
        return -1;
    }

    acquire(&sems[id].lock);
    if (sems[id].allocated == 1 && sems[id].resource_count > 0) {
        sems[id].allocated = 0;
    }

    release(&sems[id].lock);

    return 0;
}

uint64
sys_sem_p(void) {
    int id;
    if (argint(0, &id) < 0) {
        return -1;
    }

    acquire(&sems[id].lock);
    sems[id].resource_count--;
    // -1, 如果小于0,等待
    if (sems[id].resource_count < 0) {
        sleep(&sems[id], &sems[id].lock);
    }
    release(&sems[id].lock);

    return 0;
}

uint64
sys_sem_v(void) {
    int id;
    if (argint(0, &id) < 0) {
        return -1;
    }

    acquire(&sems[id].lock);
    sems[id].resource_count += 1;
    // +1, 如果为0,唤醒等待的线程
    if (sems[id].resource_count < 1) {
        wakeup(&sems[id]);
    }
    release(&sems[id].lock);
    return 0;
}