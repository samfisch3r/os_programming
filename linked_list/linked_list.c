#include <stdlib.h>
#include <stdio.h>

struct list
{
    int value;
    struct list *next;
};

typedef struct list node;

void main()
{
   node *curr, *head;
   int i;
   head = NULL;

   for (i = 0; i < 10; i++)
   {
      curr = (node *)malloc(sizeof(node));
      curr->value = i;
      curr->next  = head;
      head = curr;
   }

   curr = head;

   while(curr)
   {
      printf("%d\n", curr->value);
      curr = curr->next ;
   }
}