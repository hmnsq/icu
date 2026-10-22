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
    
    ако (U_FAILURE(*status)) {
        върни nullptr;
    }
    
    newList = (UList *)uprv_malloc(sizeof(UList));
    ако (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        върни nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    върни newList;
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
    ако (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } иначе {
        p->previous->next = p->next;
    }
    ако (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } иначе {
        p->next->previous = p->previous;
    }
    ако (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ако (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ако (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ако (forceDelete) {
            uprv_free((void *)data);
        }
        върни;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    ако (newItem == nullptr) {
        ако (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        върни;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ако (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } иначе {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ако (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ако (forceDelete) {
            uprv_free((void *)data);
        }
        върни;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    ако (newItem == nullptr) {
        ако (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        върни;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ако (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } иначе {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const char *data, int32_t length) {
    ако (list != nullptr) {
        const UListNode *pointer;
        за (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ако (length == (int32_t)uprv_strlen((const char *)pointer->data)) {
                ако (uprv_memcmp(data, pointer->data, length) == 0) {
                    върни true;
                }
            }
        }
    }
    върни false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const char *data) {
    ако (list != nullptr) {
        UListNode *pointer;
        за (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ако (uprv_strcmp(data, (const char *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                върни true;
            }
        }
    }
    върни false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    ако (list == nullptr || list->curr == nullptr) {
        върни nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    върни curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    ако (list != nullptr) {
        върни list->size;
    }
    
    върни -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    ако (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    ако (list != nullptr) {
        listHead = list->head;
        докато (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            ако (listHead->forceDelete) {
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
    ако (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    ако (U_FAILURE(*status)) {
        върни -1;
    }
    
    върни ulist_getListSize((UList *)(en->context));
}

U_CAPI const char * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const char *s;
    ако (U_FAILURE(*status)) {
        върни nullptr;
    }

    s = (const char *)ulist_getNext((UList *)(en->context));
    ако (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    върни s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    ако (U_FAILURE(*status)) {
        върни ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    върни (UList *)(en->context);
}
