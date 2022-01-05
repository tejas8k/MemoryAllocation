#include <iostream>

using namespace std;

#define HEAP_START_ADDR (size_t)0
#define HEAP_LEN        (size_t)10000
#define HEAP_END_ADDR   (size_t)(HEAP_START_ADDR + HEAP_LEN - 1)

typedef struct node {
	size_t base_add;
	size_t len;
	struct node* next;

	node(int b, int l, struct node* n = nullptr) : base_add(b), len(l), next(n) {}

}NODE;

NODE* Head = nullptr;

/* Return Null if memory can not be allocated
 * Memory is allocated to first available free slot
 */
void* Malloc(size_t size) {
	NODE* p = nullptr;
	if (size != 0) {
		if (Head == nullptr) {
			/* This is the first call of Malloc */
			if (size <= HEAP_LEN) {
				NODE* new_node = new NODE(HEAP_START_ADDR, size, nullptr);
				Head = new_node;
				p = Head;
				return (void*)(p->base_add);
			}
		}
		else {
			NODE* curr;
			curr = Head;

			/* Check if there is any Free space at start of Heap (but before Head) */
			if (curr->base_add != HEAP_START_ADDR) {
				if ((HEAP_START_ADDR + size - 1) < curr->base_add) {
					NODE* new_node = new NODE(HEAP_START_ADDR, size, curr);
					Head = p = new_node;
					return (void*)(p->base_add);
				}
			}

			/* Look for first available in between free slot */
			while (curr->next != nullptr) {
				if ((curr->base_add + curr->len + size - 1) < curr->next->base_add) {
					NODE* new_node = new NODE(curr->base_add + curr->len, size, curr->next);
					curr->next = p = new_node;
					return (void*)(p->base_add);
				}
				curr = curr->next;
			}
			/*This is the last node, make sure size is within head limit */
			if ((curr->base_add + curr->len + size - 1) <= HEAP_END_ADDR) {
				NODE* new_node = new NODE(curr->base_add + curr->len, size, curr->next);
				curr->next = p = new_node;
				return (void*)(p->base_add);
			}
		}
	}
	if (p == nullptr) {
		cout << "Can Not allocate memory size " << size << endl;
	}
	return (void*)p;
}

void Free(void* p) {
	NODE* curr, * prev;
	size_t free_addr = (size_t)p;

	if (Head->base_add == free_addr) {
		curr = Head; /*Take back up of Head */
		Head = curr->next;
		/* Delete Head */
		delete((NODE*)curr);
		return;
	}
	curr = Head, prev = nullptr;
	while (curr != nullptr) {
		if (curr->base_add == free_addr) {
			prev->next = curr->next;
			delete((NODE*)curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	cout << "Can not Free memory " << free_addr << endl;
}

void Print() {
	NODE* curr = Head;
	while (curr != nullptr) {
		cout << curr->base_add << "(" << curr->len << ")" << " -> ";
		curr = curr->next;
	}
	cout << endl;
}

int main() {
	int* p1 = (int*)Malloc(1000);	Print();
	int* p2 = (int*)Malloc(2000);	Print();
	int* p3 = (int*)Malloc(500);	Print();
	int* p4 = (int*)Malloc(1000);	Print();
	Free(p3);						Print();
	int* p5 = (int*)Malloc(500);	Print();
	Free(p1);						Print();
	int* p6 = (int*)Malloc(200);	Print();
	Free(p2);						Print();
	Free(p4);						Print();
	Free(p5);						Print();
	Free(p6);						Print();
	return 0;
}