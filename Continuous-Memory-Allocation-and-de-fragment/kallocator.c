#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "kallocator.h"
#include "list_sol.h"

int compacted_size = 0;
Node* mem_ref = NULL;

struct KAllocator {
    enum allocation_algorithm aalgorithm; 
    int size;
    void* memory;
    // Some other data members you want, 
    // such as lists to record allocated/free memory
    Node* allocated_mem;
    Node* free_mem;
};

struct KAllocator kallocator;

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);

    // Add some other initialization 

    kallocator.allocated_mem = NULL;
    kallocator.free_mem = List_createNode(_size, 'f', kallocator.memory);

}

void destroy_allocator() {
    free(kallocator.memory);

    // free other dynamic allocated memory to avoid memory leak
    while (kallocator.free_mem != NULL)
    	List_deleteNode (&kallocator.free_mem, kallocator.free_mem);

    while (kallocator.allocated_mem != NULL)
    	List_deleteNode (&kallocator.allocated_mem, kallocator.allocated_mem);

    while (mem_ref != NULL)
        List_deleteNode (&mem_ref, mem_ref);
}

void allocate_memory (int _size, Node* free_node)
{
	Node* new_node = List_createNode(_size, 'a', free_node->ptr);
	List_insertHead (&kallocator.allocated_mem, new_node);

	if (free_node->size == _size)
		List_deleteNode (&kallocator.free_mem, free_node);
	else if (free_node->size > _size)
	{
		free_node->ptr = free_node->ptr + _size; //not so sure
		free_node->size = free_node->size - _size; //not so sure
	}
}

Node* find_best_hole(int _size)
{
	Node* pointer = kallocator.free_mem;
	int min = 1000;
	Node* ret = NULL;
	while (pointer != NULL)
	{
		if (pointer->size == _size)
			return pointer;
		else if (pointer->size > _size)
		{
			if (min > (pointer->size - _size))
			{
				min = pointer->size - _size;
				ret = pointer;
			}
		}
		pointer = pointer->next;
	}
	return ret;
}

Node* find_worst_hole(int _size)
{
	Node* pointer = kallocator.free_mem;
	int max = pointer->size;
	Node* ret = pointer;
	while (pointer != NULL)
	{
		if (pointer->size > max)
		{
			max = pointer->size;
			ret = pointer;
		}
		pointer = pointer->next;
	}

	if (max >= _size)
		return ret;
	else
		return NULL;
}

void* kalloc(int _size) 
{
    void* ptr = NULL;

    if (kallocator.aalgorithm == 0) // FIRST-FIT
    {
        List_sort (&kallocator.free_mem);
    	Node* free_mem_traverser = kallocator.free_mem;
        while (free_mem_traverser != NULL)
        {
            if (free_mem_traverser->size >= _size)
            	break;
            free_mem_traverser = free_mem_traverser->next;
        }

        ptr = free_mem_traverser->ptr;
        allocate_memory(_size, free_mem_traverser);
    }
    else if (kallocator.aalgorithm == 1) //BEST-FIT
    {
		Node* best_hole = find_best_hole(_size);
		ptr = best_hole->ptr;
		allocate_memory(_size, best_hole);
    }
    else if (kallocator.aalgorithm == 2) //WORST-FIT
    {
        Node* worst_hole = find_worst_hole(_size);
		ptr = worst_hole->ptr;
		allocate_memory(_size, worst_hole);
    }
    // Allocate memory from kallocator.memory 
    // ptr = address of allocated memory

    return ptr;
}

void kfree(void* _ptr) 
{
    assert(_ptr != NULL);

    Node* allocated_node = List_findNode(kallocator.allocated_mem, _ptr);
    //piche khali thakle
    Node* traverser1 = kallocator.free_mem;
    Node* free_node1 = NULL;
    while (traverser1 != NULL)
    {
    	if ((traverser1->ptr + traverser1->size) == _ptr)
    	{
    		traverser1->size = traverser1->size + allocated_node->size;
    		free_node1 = traverser1;
    		break;
    	}
    	traverser1 = traverser1->next;
    }
    //shamne khali thakle + shamne piche dui jaigai khali thakle
    Node* traverser2 = kallocator.free_mem;
    while (traverser2 != NULL)
    {
    	//shamne khali thakle ar piche bhora khatle
    	if (((allocated_node->ptr + allocated_node->size) == traverser2->ptr) && free_node1 == NULL)
    	{
    		traverser2->ptr = allocated_node->ptr; //traverser->ptr - allocated_node->size;
    		traverser2->size = traverser2->size + allocated_node->size;
    		break;
    	}
    	//shamneo khali picheo khali
    	else if ((allocated_node->ptr + allocated_node->size) == traverser2->ptr && free_node1 != NULL)
    	{
    		free_node1->size = free_node1->size + traverser2->size;
    		List_deleteNode (&kallocator.free_mem, traverser2);
    		break;
    	}
    	traverser2 = traverser2->next;
    }

    if(traverser1 == NULL && traverser2 == NULL)
    {
    	Node* new_free_node = List_createNode(allocated_node->size, 'f', allocated_node->ptr);
    	List_insertHead (&kallocator.free_mem, new_free_node);
    }

    List_deleteNode (&kallocator.allocated_mem, allocated_node);
}

void update_metaData_lists()
{
    while (kallocator.free_mem != NULL)
        List_deleteNode (&kallocator.free_mem, kallocator.free_mem);

    while (kallocator.allocated_mem != NULL)
        List_deleteNode (&kallocator.allocated_mem, kallocator.allocated_mem);

    Node* traverser = mem_ref;
    while (traverser != NULL)
    {
        if (traverser->type == 'f')
        {
            Node* new_node = List_createNode(traverser->size, traverser->type, traverser->ptr);
            List_insertHead (&kallocator.free_mem, new_node);
        }
        else if (traverser->type == 'a')
        {
            Node* new_node = List_createNode(traverser->size, traverser->type, traverser->ptr);
            List_insertHead (&kallocator.allocated_mem, new_node);
        }
        traverser = traverser->next;
    }
}

void move_mem_swap_nodes (Node* left, Node* right)
{
    void* start = left->ptr;
    void* middle = right->ptr;
    void* end = right->ptr + right->size;

    /*size_t* start = left->ptr;
    size_t* middle = right->ptr;
    size_t* end = right->ptr + right->size;*/

    /*int* start = left->ptr;
    int* middle = right->ptr;
    int* end = right->ptr + right->size;*/

    while (middle != end)
    {
        *((size_t*)start) = *((size_t*)middle);
        //*start = *middle;
        start++;
        middle++;
    }

    //memmove(left, right, right->size);

    right->ptr = left->ptr;
    left->ptr = start;

    Node* previous = mem_ref;
    while (previous->next != left)
    	previous = previous->next;
    left->next = right->next;
    right->next = left;
    previous->next = right;
}

void move_and_compact(Node* left, Node* right)
{
        if (right == NULL)
            return;
        else if ((left->type == 'a' && right->type == 'f') || (left->type == 'a' && right->type == 'a'))
        {
            //move
            //printf("a f -> ");
            move_and_compact(right, right->next);
        }
        // else if ((left->type == 'a' && right->type == 'a'))
        // {
        //     //move
        //     //printf("a a -> ");
        //     move_and_compact(right, right->next);
        // }
        else if (left->type == 'f' && right->type == 'a')
        {
            //swap then move
            //printf("f a -> ");
            move_mem_swap_nodes(left, right);
            move_and_compact(left, left->next);
            compacted_size++;
        }
        else if (left->type == 'f' && right->type == 'f')
        {
        	//printf("f f -> ");
            left->size = left->size + right->size;
            //left->next = right->next;
            //free(right);
            List_deleteNode (&mem_ref, right);
            move_and_compact(left, left->next);
        }
}

void fix_before_after(void** _before, void** _after)
{
	int arr_size = List_countNodes (kallocator.allocated_mem);
	for (int i=0; i<arr_size; i++)
	{
		if (_before[i] == _after[i])
		{
			for (int j=i; j < arr_size-1; j++)
			{
				_before[j] = _before[j+1];
				_after[j] = _after[j+1];
			}
			arr_size--;
			i--;
		}
	}
}

int compact_allocation(void** _before, void** _after) 
{
    
	//printf("compact\n");
    // compact allocated memory
    // update _before, _after and compacted_size
    
    Node* traverser = kallocator.free_mem;
    while (traverser != NULL)
    {
        Node* new_node = List_createNode(traverser->size, traverser->type, traverser->ptr);
        List_insertHead (&mem_ref, new_node);
        traverser = traverser->next;
    }
    traverser = kallocator.allocated_mem;
    while (traverser != NULL)
    {
        Node* new_node = List_createNode(traverser->size, traverser->type, traverser->ptr);
        List_insertHead (&mem_ref, new_node);
        traverser = traverser->next;
    }
    List_sort (&mem_ref);

    //List_print(mem_ref);

    traverser = mem_ref;
    int i = 0;
    while (traverser != NULL)
    {
        if (traverser->type == 'a')
        {
            _before[i] = traverser->ptr; //syntax
            i++;
        }
        traverser = traverser->next;
    }

    move_and_compact(mem_ref, mem_ref->next);
    //printf("NULL\n");
    update_metaData_lists();

    //List_print(mem_ref);

    traverser = mem_ref;
    i = 0;
    while (traverser != NULL)
    {
        if (traverser->type == 'a')
        {
        	//printf("%d\n", (*(int*)traverser->ptr));
            _after[i] = traverser->ptr; //syntax
            i++;
            //printf("allocated node\n");
        }
        // if (traverser->type == 'f')
        //     printf("free node\n");
        traverser = traverser->next;
    }

    fix_before_after(_before, _after);

    // for (int i = 0; i < compacted_size; ++i)
    // {
    // 	printf("%d\n", *(int*)_after[i]);
    // }

    //printf("compacted_size: %d\n", compacted_size);

    return compacted_size;
}

int available_memory() {
    int available_memory_size = 0;
    // Calculate available memory size
    Node* free_mem_traverser = kallocator.free_mem;
    while (free_mem_traverser != NULL)
    {
    	available_memory_size = available_memory_size + free_mem_traverser->size;
   		free_mem_traverser = free_mem_traverser->next;
   	}

    return available_memory_size;
}

int allocated_memory() {
    int allocated_memory_size = 0;
    // Calculate allocated memory size
    Node* allocated_mem_traverser = kallocator.allocated_mem;
    while (allocated_mem_traverser != NULL)
    {
    	allocated_memory_size = allocated_memory_size + allocated_mem_traverser->size;
   		allocated_mem_traverser = allocated_mem_traverser->next;
   	}

    return allocated_memory_size;
}

int find_smallest_free_chunk_size()
{
	Node* pointer = kallocator.free_mem;
	int min = pointer->size;
	while (pointer != NULL)
	{
		if (pointer->size < min)
			min = pointer->size;

		pointer = pointer->next;
	}
	return min;
}

int find_largest_free_chunk_size()
{
	Node* pointer = kallocator.free_mem;
	int max = pointer->size;
	while (pointer != NULL)
	{
		if (pointer->size > max)
			max = pointer->size;

		pointer = pointer->next;
	}
	return max;
}

void print_statistics() {
    int allocated_size = allocated_memory();
    int allocated_chunks = List_countNodes (kallocator.allocated_mem);
    int free_size = available_memory();
    int free_chunks = List_countNodes (kallocator.free_mem);
    int smallest_free_chunk_size = find_smallest_free_chunk_size();
    int largest_free_chunk_size = find_largest_free_chunk_size();

    // Calculate the statistics

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}



