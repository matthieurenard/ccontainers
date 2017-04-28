#ifndef LIST_H
#define LIST_H

struct List;
struct ListIterator;

struct List* list_new();
struct List *list_newcp(const struct List *, void *(*)(void *));
void list_free(struct List *, void (*)(void *));
struct List* list_init(struct List *);
struct List *list_append(struct List *, void *data);
struct List *list_addHead(struct List *, void *data);
struct List *list_appendList(struct List *, const struct List *, void *(*)(void 
			*));
struct List *list_concatList(struct List *dst, struct List *src);
struct List *list_remove(struct List *, void *data);
void *list_removeHead(struct List *l);
void *list_removeLast(struct List *l);
int list_isEmpty(const struct List *);
unsigned int list_size(const struct List *);
void *list_search(const struct List *, const void *,
		int (*)(const void *, const void *));
void list_cleanup(struct List *, void (*)(void *));

struct ListIterator *listIterator_first(const struct List *);
int listIterator_hasNext(const struct ListIterator *);
int listIterator_isLast(const struct ListIterator *);
struct ListIterator *listIterator_next(struct ListIterator *);
void *listIterator_val(const struct ListIterator *);
void listIterator_release(struct ListIterator *);

#endif
