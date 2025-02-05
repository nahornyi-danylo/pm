#include <stdlib.h>
#include <assert.h>
#include "stack.h"

static void resize(stack *instance){
  instance->capacity*=2;
  instance->s = realloc(instance->s, sizeof(int)*instance->capacity);
}

void push(int n, stack *instance){
  instance->s[instance->top] = n;
  instance->top++;
  if(instance->top == instance->capacity)resize(instance);
}

int pop(stack *instance){
  assert(instance->top);
  instance->top--;
  return instance->s[instance->top];
}

int stackInit(stack *instance){
  instance->capacity = 1024;
  instance->top = 0;
  instance->s = malloc(sizeof(int)*instance->capacity);
  if(!instance->s){
    return -1;
  }
  return 0;
}

