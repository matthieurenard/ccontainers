#include "list.h"

#include <stdlib.h>
#include <stdio.h>

struct Element
{
	void *data;
	struct Element *next;
	struct Element *prev;
};

struct List
{
	struct Element *head;
	struct Element *tail;
	unsigned int size;
};

struct ListIterator
{
	struct Element *next;
	struct Element *prev;
};


struct List *list_new()
{
	struct List *l = malloc(sizeof *l);

	if (l == NULL)
	{
		perror("malloc list_new");
		exit(EXIT_FAILURE);
	}

	return list_init(l);
}

struct List *list_newcp(const struct List *other, void *(*cp)(void *))
{
	return list_appendList(list_new(), other, cp);
}

void list_free(struct List *l, void (*clean)(void *))
{
	struct Element *e, *next;

	if (l == NULL)
		return;

	e = l->head;
	while (e != NULL)
	{
		next = e->next;
		if (clean != NULL)
			clean(e->data);
		free(e);
		e = next;
	}

	free(l);
}

struct List *list_init(struct List *l)
{
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;
	return l;
}

struct List *list_append(struct List *l, void *data)
{
	struct Element *e = malloc(sizeof *e);

	if (e == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	if (l->head == NULL)
		l->head = e;
	else
		l->tail->next = e;

	e->prev = l->tail;
	l->tail = e;

	e->next = NULL;
	e->data = data;

	l->size++;

	return l;
}

struct List *list_addHead(struct List *l, void *data)
{
	struct Element *e = malloc(sizeof *e);

	if (e == NULL)
	{
		perror("malloc list_add:e");
		exit(EXIT_FAILURE);
	}

	if (l->tail == NULL)
		l->tail = e;
	
	e->prev = NULL;
	e->next = l->head;
	e->data = data;

	l->head = e;
	l->size++;

	return l;
}

struct List *list_appendList(struct List *dst, const struct List *src, void *(*cp)(void *))
{
	const struct Element *el;

	for (el = src->head ; el != NULL ; el = el->next)
	{
		if (cp != NULL)
			list_append(dst, cp(el->data));
		else
			list_append(dst, el->data);
	}

	return dst;
}

/* !! After a call to this function, the second list does not exist anymore, it 
 * is deallocated, the resulting list is stored in the first one : dst <-- dst . 
 * src */
struct List *list_concatList(struct List *dst, struct List *src)
{
	if (list_isEmpty(dst))
		dst->head = src->head;
	else
		dst->tail->next = src->head;
	if (!list_isEmpty(src))
		src->head->prev = dst->tail;
	dst->tail = list_isEmpty(src) ? dst->tail : src->tail;
	dst->size += src->size;

	/* Only free the struct List, do not call list_free, which would free all 
	 * the structs Element as well */
	free(src);

	return dst;
}

struct List *list_remove(struct List *l, void *data)
{
	struct Element *e, *prev;

	if (list_isEmpty(l))
		return l;

	e = l->head;
	prev = e;
	if (e->data == data)
	{
		e = e->next;
		free(prev);
		l->head = e;
		if (e == NULL)
			l->tail = e;

		l->size--;

		return l;
	}

	for ( ; e != NULL ; prev = e, e = e->next)
	{
		if (e->data == data)
		{
			prev->next = e->next;
			if (l->tail == e)
				l->tail = prev;
			free(e);
			l->size--;
			break;
		}
	}

	return l;
}

void *list_first(const struct List *l)
{
	return l->head->data;
}

void *list_removeHead(struct List *l)
{
	struct Element *e = l->head;
	void *ret = e->data;

	l->head = e->next;
	if (l->tail == e)
		l->tail = NULL;
	l->size--;

	free(e);

	return ret;
}

void *list_last(const struct List *l)
{
	return l->tail->data;
}

void *list_removeLast(struct List *l)
{
	struct Element *e = l->tail;
	void *ret = e->data;

	l->tail = e->prev;
	if (l->head == e)
		l->head = NULL;
	l->size--;

	free(e);

	return ret;
}

int list_isEmpty(const struct List *l)
{
	return l->head == NULL;
}

unsigned int list_size(const struct List *l)
{
	return l->size;
}

void *list_search(const struct List *l, const void *val,
		int (*eq)(const void *, const void *))
{
	struct Element *e;

	if (list_isEmpty(l))
		return NULL;

	for (e = l->head ; e != NULL ; e = e->next)
	{
		if (eq(val, e->data))
			return e->data;
	}

	return NULL;
}

void list_cleanup(struct List *l, void (*release)(void *))
{
	struct Element *e, *p;

	if (l == NULL)
		return;

	e = l->head;
	for (p = e ; e != NULL ; p = e)
	{
		e = e->next;
		if (release != NULL)
			release(p->data);
		free(p);
	}
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;
}

struct ListIterator *listIterator_first(const struct List *l)
{
	struct ListIterator *ret = malloc(sizeof *ret);

	if (ret == NULL)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	ret->next = l->head;
	ret->prev = NULL;

	return ret;
}

struct ListIterator *listIterator_last(const struct List *l)
{
	struct ListIterator *ret = malloc(sizeof *ret);

	if (ret == NULL)
	{
		perror("malloc listIterator_last:ret");
		exit(EXIT_FAILURE);
	}

	ret->next = l->tail;
	ret->prev = (l->tail != NULL) ? l->tail->prev : NULL;

	return ret;
}

struct ListIterator *listIterator_cp(const struct ListIterator *other)
{
	struct ListIterator *ret = malloc(sizeof *ret);

	if (ret == NULL)
	{
		perror("malloc listIterator_cp:ret");
		exit(EXIT_FAILURE);
	}

	ret->next = other->next;
	ret->prev = other->prev;

	return ret;
}

int listIterator_hasNext(const struct ListIterator *it)
{
	return (it->next != NULL);
}

int listIterator_hasPrev(const struct ListIterator *it)
{
	return (it->prev != NULL);
}

int listIterator_isLast(const struct ListIterator *it)
{
	return (it->next != NULL && it->next->next == NULL);
}

int listIterator_isFirst(const struct ListIterator *it)
{
	return (it->prev == NULL);
}

struct ListIterator *listIterator_next(struct ListIterator *it)
{
	it->prev = it->next;
	it->next = it->next->next;
	return it;
}

struct ListIterator *listIterator_prev(struct ListIterator *it)
{
	it->next = it->prev;
	it->prev = it->prev->prev;
	return it;
}

void *listIterator_val(const struct ListIterator *it)
{
	return it->next->data;
}

void listIterator_release(struct ListIterator *it)
{
	free(it);
}

