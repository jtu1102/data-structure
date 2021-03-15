#include <stdio.h>
#include <stdlib.h>

FILE *input;
FILE *output;

typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;
typedef int ElementType;

struct Node
{
	ElementType element;
	Position next;
};

List		MakeEmpty(List L);
int			IsEmpty(List L);
int			IsLast(Position P, List L);
void		Insert(ElementType X, List L, Position P);
void		PrintList(List L);
void		Delete(ElementType X, List L);
Position	Find(ElementType X, List L);
Position	FindPrevious(ElementType X, List L);
void		DeleteList(List L);

int main(int argc, char *argv[])
{
	char command;
	int key1, key2;
	FILE *input;
	Position header = NULL;
	Position tmp = NULL;
	if (argc == 1)
	{
		input = fopen("input.txt", "r");
		output = fopen("output.txt", "w");
	}
	else
	{
		input = fopen(argv[1], "r");
		output = fopen(argv[2], "w");
	}
	header = MakeEmpty(header);
	while (1)
	{
		command = fgetc(input);
		if (feof(input))
			break;
		switch (command)
		{
			case 'i':
					fscanf(input, "%d %d", &key1, &key2);
					tmp = Find(key1, header);
					if (tmp != NULL)
					{
						fprintf(output, "There already is an element with key %d. Insertion failed\n", key1);
						break;
					}
					tmp = Find(key2, header);
					if (tmp == NULL) // This means insertion error. 'tmp' cannot be the argument of 'Insert' function
					{
						fprintf(output, "Insertion(%d) Failed: cannot find the location to be inserted\n", key1);
						break;
					}
					Insert(key1, header, tmp);
					break;
			case 'd':
					fscanf(input, "%d", &key1);
					Delete(key1, header);			// delete error message is in Delete function
					break;
			case 'f':
					fscanf(input, "%d", &key1);
					tmp = FindPrevious(key1, header);
					if (IsLast(tmp, header))
						fprintf(output, "Finding(%d) Failed: Element %d is not in the list\n", key1, key1);
					else
					{
						if (tmp->element > 0)
							fprintf(output, "Key of the previous node of %d is %d\n", key1, tmp->element);
						else
							fprintf(output, "Key of the previous node of %d is head\n", key1);
					}
					break;
			case 'p':
					if (IsEmpty(header))
					{
						fprintf(output, "Empty List!\n");
						break;
					}
					PrintList(header);
					break;
			default:
					break;
		}
	}

	DeleteList(header);
	fclose(input);
	fclose(output);
	return 0;
}

/* Create a new List == Create header Node */

List	MakeEmpty(List L)
{
	List list;
	
	list = (List)malloc(sizeof(struct Node)); // allocate memory for new list
	list->element = -1;
	list->next = NULL;
	return list;
}

/* return 1 if the list is empty */

int		IsEmpty(List L)
{
	return (L->next == NULL);
}

/* return 1 if the 'P' is the last node of the list */

int		IsLast(Position P, List L)			 // do not use argument 'L'
{
	return (P->next == NULL);
}

/* Create new Node with elemnet X, and insert it in the list after the position 'P' */

void	Insert(ElementType X, List L, Position P)
{
	Position new;
	new = (Position)malloc(sizeof(struct Node)); // create new Node
	new->element = X;							 // put data in the new Node
	new->next = P->next;						 // link 
	P->next = new;								 // link 
}

/* print all the element of the list */

void	PrintList(List L)
{
	Position now;
	now = L->next;							// do not print header element
	while (now != NULL)						// print all elements of the list
	{
		fprintf(output, "key: %d\t", now->element);
		now = now->next;					// move to the next Node
	}
	fprintf(output, "\n");
}

/* Delete Node that have element X */

void	Delete(ElementType X, List L)
{
	Position p;
	Position tmp;
	p = L;									// find the Node with element 'X' from the first Node (right next the header)
	while(p->next != NULL && p->next->element != X)
		p = p->next;
	if (p->next == NULL)					// This is the case that a list doesn't have any Node with element X
		fprintf(output, "Deletion(%d) Failed: Element %d is not in the list\n", X, X);
	else
	{
		tmp = p->next;
		p->next = tmp->next;
		tmp->next = NULL;
		free(tmp);
	}
}

/* Find Node with element X */

Position	Find(ElementType X, List L)
{
	Position now;
	now = L;							    // Check from the header Node
	while (now != NULL && now->element != X)
		now = now->next;
	return (now);							// if the list doesn't have any Node with element X, 'Find' return NULL
}

/* Find Previous Node with element X */

Position	FindPrevious(ElementType X, List L)
{
	Position now;
	now = L;								// if first Node has element 'X', the previous Node of this Node is header
	while (now->next != NULL && now->next->element != X)
		now = now->next;
	return (now);							// if a list doesn't have any Node with element X, 'FindPrevious' return NULL
}

/* Delete List. Free all allocated memory of Nodes in the list */

void	DeleteList(List L)
{
	Position now;
	Position tmp;

	now = L->next;
	L->next = NULL;							// make list empty
	while (now != NULL)						// delete all Nodes
	{
		tmp = now->next;					// backup, after freeing 'now', cannot access 'now->next'
		free(now);
		now = tmp;
	}
}
