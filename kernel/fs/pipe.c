#include "../inc/defs.h"
#include "../inc/types.h"
#include "../inc/file.h"
#include "../inc/printf.h"
#include "../inc/string.h"

extern struct proc *curproc;

int
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *p;

  p = 0;
  *f0 = *f1 = 0;
  if((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  if((p = (struct pipe*)kalloc()) == 0)
    goto bad;

  p->readopen = 1;
  p->writeopen = 1;
  p->nwrite = 0;
  p->nread = 0;
  initlock(&p->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = p;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = p;

  p->pw = 0;
  p->pr = 0;

  // printf("[pipealloc] :%x\n", p);

  return 0;

//PAGEBREAK: 20
 bad:
  //printf("pipe bad\n");
  if(p)
    kfree((char*)p);
  if(*f0)
    fileclose(*f0);
  if(*f1)
    fileclose(*f1);
  return -1;
}

void
pipeclose(struct pipe *p, int writable)
{
  // printf("pipe close writable: %d\n", writable);
  acquire(&p->lock);
  if(writable){
    p->writeopen = 0;
    wakeup(p->pr);
  } else {
    p->readopen = 0; 
    wakeup(p->pw);
  }

  if(p->readopen == 0 && p->writeopen == 0){
    release(&p->lock);
    kfree((char*)p);
  } else
    release(&p->lock);
}

extern struct proc *curproc;   
int
pipewrite(struct pipe *p, char *addr, int n)
{
  p->pw = curproc;
  // printf("pipe write: %s  pipe-addr:%x\n", addr, p);
  int i;

  acquire(&p->lock);
  for(i = 0; i < n; i++){
    while(p->nwrite == p->nread + PIPESIZE){  //DOC: pipewrite-full
      if(p->readopen == 0 || curproc->killed){
        release(&p->lock);
        return -1;
      }
      lock_ptable();
      if (p->pr) wakeup(p->pr);
      release(&p->lock);
      
      sleep(curproc);  //DOC: pipewrite-sleep
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
    // safebytecpy(&p->data[p->nwrite++ % PIPESIZE], addr[i]);
  }
  if (p->pr) wakeup(p->pr);  //DOC: pipewrite-wakeup1
  release(&p->lock);
  return n;
}

int
piperead(struct pipe *p, char *addr, int n)
{
  int i;
  // printf("pipe read:  dst: %x  pipe-addr:%x\n", addr, p);

  acquire(&p->lock);
  while(p->nread == p->nwrite && p->writeopen){  //DOC: pipe-empty
    if(curproc->killed){
      release(&p->lock);
      return -1;
    }
    lock_ptable();
    release(&p->lock);
    sleep(curproc); //DOC: piperead-sleep
  }
  for(i = 0; i < n; i++){  //DOC: piperead-copy
    if(p->nread == p->nwrite)
      break;
    // safebytecpy(&addr[i], p->data[p->nread++ % PIPESIZE]);
    addr[i] = p->data[p->nread++ % PIPESIZE];
    // printf("[piperead]: %c\n", addr[i]);
  }
  if (p->pw) wakeup(p->pw);  //DOC: piperead-wakeup
  release(&p->lock);
  return i;
}


