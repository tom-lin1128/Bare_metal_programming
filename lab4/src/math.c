#include "math.h"

int pow(int n,int p){
    int tmp = n;
    for(int i = 1; i < p; i++){
        n *= tmp;
    }
    return n;
}

int log2(int n){ 
  int i=0;
  while (n>>=1) ++i;
  return i;
}