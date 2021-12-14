#include "types.h"
#include "user.h"

// 堆区内存管理

//校准  有何用处?
typedef long Align;

union header {
  struct {
    union header *ptr; //4B
    u_int size;        //4B 不会受影响
  } s;                 //8B
  Align x;             //4B
};

typedef union header Header;

//注意：这里有雷！freep是一定要初始化为0的，但是操作系统可能装载失败
//解决方案：kalloc时清零

static Header base;     //环链起点
// Header base;
static Header *freep;   //空闲内存块链头 
// Header *freep = 0;


//初始化时 freep指向base
void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  //从空闲链头开始查询 准备合并
  //保证其按顺序串联

  //筛选 直到ap在p和下一个块中间
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    //p往后指 在两个空闲块外 (向上扩展链)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  
  //相邻块 合并  释放块在目标块下方
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr; //插入到p的下一个

  //初始化后bp->s.ptr指向base
  
  //相邻块 合并  释放块在目标块上方
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;  //插入到p的下一个

  //初始化后base.ptr指向新块

  freep = p;
}

// static类型貌似会出问题?
// static 
Header*
morecore(u_int nu)
{
  //printf("morecore: %u\n", nu);
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  //申请8页
  //pause();
  p = sbrk(nu * sizeof(Header));
  
  if(p == (char*)-1)
    return 0;
  //p:可用的新起始地址
  hp = (Header*)p;
  hp->s.size = nu;
  //挂到链上
  free((void*)(hp + 1));
  // printf("end morecore\n");
  return freep;
}

void*
malloc(u_int nbytes)
{
  // printf("malloc: freep: %x\n", freep);
  Header *p, *prevp;
  u_int nunits;

  //size : 8
  //单元数(向上取整) + 1
  //即所有内存块都是8的倍数（第一个块存储大小，下一块的位置）
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    //初始化 base ptr指向自己  初始化使用
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  //首次适应算法，从空闲链表上寻找合适大小的内存块
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    //匹配
    if(p->s.size >= nunits){
      //大小相等 摘除此块
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      //内存块大于申请大小 分割（去尾）
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      //链头停留在上次分配的位置
      freep = prevp;
      //摘除适配块的尾部分配给用户
      // printf("malloc match: %x\n", (void*)(p + 1));
      return (void*)(p + 1);
    }
    //回到链头 无合适大小的块  新申请
    //初始化时 p freep均指向&base
    if(p == freep) {
      if((p = morecore(nunits)) == 0)
        return 0;
    }
      
  }
}

void disheap() {
  printf("Header *freep : %x\n", freep);
}