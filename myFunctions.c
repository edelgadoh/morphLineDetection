#include<stdlib.h>
#include<stdio.h>
#include <malloc.h>

/*
gcc -c -fPIC libtest.c
gcc -shared libtest.o -o libtest.so

typedef struct lista {
        int x;
        struct lista *prox;
} Lista;

Lista *list = NULL;
*/
/* LISTA: Insere na lista os pontos das folhas por cada no da arvore. */
/*
Lista *insereLista(int x){
    Lista *novo;
    novo = (Lista*)malloc(sizeof(Lista));
    if (!novo) {
      printf("Falta de memória");
      exit(0);
    }
    novo->x = x;
    novo->prox=NULL;
    if(list == NULL)
      list=novo;
    else
    {
      novo->prox = list;
      list=novo;
    }
    return list;
}
*/
void swap(int* a, int* b) {
  int tmp;
  tmp = *a;
  *a = *b;
  *b = tmp;
}
 
int partition(int vec[], int left, int right) {
  int i, j;
 
  i = left;
  for (j = left + 1; j <= right; ++j) {
    if (vec[j] < vec[left]) {
      ++i;
      swap(&vec[i], &vec[j]);
    }
  }
  swap(&vec[left], &vec[i]);
 
  return i;
}
 
void quickSort(int vec[], int left, int right) {
  int r;
 
  if (right > left) {
    r = partition(vec, left, right);
    quickSort(vec, left, r - 1);
    quickSort(vec, r + 1, right);
  }
}

int* rankFilter(int *data, int x, int y, int xsize, int ysize, int pRank, int *position, int psize)
{
    int i=0;
    int j=0;
    int k=0;
    int dx = x/2;
    int dy = y/2;
    int a,b;
    int rank=0;
    //Lista *lis=NULL;
    int val = 0;
    int *L = (int *)malloc(x * y * sizeof(int));
    int *B = (int *)malloc(psize * sizeof(int));
    int *data2 = (int *)malloc(xsize * ysize * sizeof(int));
    for(i=0;i<xsize;i++){
      for(j=0;j<ysize;j++){
         data2[i*ysize+j] = data[i*ysize+j];
      }
    }
    for(i=dx; i < xsize-dx; i++)
    {
        for(j=dy; j < ysize-dy; j++)
        {
                  //listaPontosPorNo=NULL;
                  k=0;
                  for(a=i-dx; a < i+dx+1; a++)
                  {
                            for(b=j-dy; b < j+dy+1; b++)
                            {
                                      //list = insereLista(data[a][b]);
                                      L[k] = data[a*ysize+b];
                                      k++;
                            }
                  }
                  for(k=0; k < psize; k++)
                  {
                    B[k] = L[position[k]];
                  }
                  //ordenar
                  quickSort(B, 0, psize-1);
                  rank = (psize-1)*pRank/100;//2
                  val = B[rank];
                  data2[i*ysize+j] = val;
        }
    }
    free(B);
    free(L);
    return data2;//data2;
}

//http://www.dee.feis.unesp.br/graduacao/disciplinas/langc/modulo_linguagemc/modulo6.htm
//http://shakaran.net/blog/2010/10/como-usar-bibliotecas-de-c-en-python/
