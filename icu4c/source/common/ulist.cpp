// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#include "ulist.h"
#include "cmemory.h"
#include "cstring.h"
#include "uenumimp.h"

typedef struct UListNode UListNode;
struct UListNode {
    void *data;
    
    UListNode *next;
    UListNode *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

struct UList {
    UListNode *curr;
    UListNode *head;
    UListNode *tail;
    
    int32_t size;
};

static void ulist_addFirstItem(UList *list, UListNode *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }
    
    newList = (UList *)uprv_malloc(sizeof(UList));
    nếu (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    trả_về newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
static void ulist_addFirstItem(UList *list, UListNode *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

static void ulist_removeItem(UList *list, UListNode *p) {
    nếu (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } không_thì {
        p->previous->next = p->next;
    }
    nếu (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } không_thì {
        p->next->previous = p->previous;
    }
    nếu (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    nếu (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    nếu (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        nếu (forceDelete) {
            uprv_free((void *)data);
        }
        trả_về;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    nếu (newItem == nullptr) {
        nếu (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    nếu (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } không_thì {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    nếu (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        nếu (forceDelete) {
            uprv_free((void *)data);
        }
        trả_về;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    nếu (newItem == nullptr) {
        nếu (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        trả_về;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    nếu (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } không_thì {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const char *data, int32_t length) {
    nếu (list != nullptr) {
        const UListNode *pointer;
        cho (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            nếu (length == (int32_t)uprv_strlen((const char *)pointer->data)) {
                nếu (uprv_memcmp(data, pointer->data, length) == 0) {
                    trả_về true;
                }
            }
        }
    }
    trả_về false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const char *data) {
    nếu (list != nullptr) {
        UListNode *pointer;
        cho (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            nếu (uprv_strcmp(data, (const char *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                trả_về true;
            }
        }
    }
    trả_về false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    nếu (list == nullptr || list->curr == nullptr) {
        trả_về nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    trả_về curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    nếu (list != nullptr) {
        trả_về list->size;
    }
    
    trả_về -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    nếu (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    nếu (list != nullptr) {
        listHead = list->head;
        trong_khi (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            nếu (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI void U_EXPORT2 ulist_close_keyword_values_iterator(UEnumeration *en) {
    nếu (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    nếu (U_FAILURE(*status)) {
        trả_về -1;
    }
    
    trả_về ulist_getListSize((UList *)(en->context));
}

U_CAPI const char * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const char *s;
    nếu (U_FAILURE(*status)) {
        trả_về nullptr;
    }

    s = (const char *)ulist_getNext((UList *)(en->context));
    nếu (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    trả_về s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    nếu (U_FAILURE(*status)) {
        trả_về ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    trả_về (UList *)(en->context);
}
