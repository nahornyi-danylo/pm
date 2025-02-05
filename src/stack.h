
typedef struct{
  int capacity;
  int top;
  
  int *s;
}stack;

int stackInit(stack *instance);
int pop(stack *instance);
void push(int n, stack *instance);
