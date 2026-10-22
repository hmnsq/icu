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
    
    만약 (U_FAILURE(*status)) {
        반환 nullptr;
    }
    
    newList = (UList *)uprv_malloc(sizeof(UList));
    만약 (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        반환 nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    반환 newList;
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
    만약 (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } 아니면 {
        p->previous->next = p->next;
    }
    만약 (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } 아니면 {
        p->next->previous = p->previous;
    }
    만약 (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    만약 (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    만약 (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        만약 (forceDelete) {
            uprv_free((void *)data);
        }
        반환;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    만약 (newItem == nullptr) {
        만약 (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        반환;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    만약 (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } 아니면 {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    만약 (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        만약 (forceDelete) {
            uprv_free((void *)data);
        }
        반환;
    }
    
    newItem = (UListNode *)uprv_malloc(sizeof(UListNode));
    만약 (newItem == nullptr) {
        만약 (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        반환;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    만약 (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } 아니면 {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const char *data, int32_t length) {
    만약 (list != nullptr) {
        const UListNode *pointer;
        위해 (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            만약 (length == (int32_t)uprv_strlen((const char *)pointer->data)) {
                만약 (uprv_memcmp(data, pointer->data, length) == 0) {
                    반환 true;
                }
            }
        }
    }
    반환 false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const char *data) {
    만약 (list != nullptr) {
        UListNode *pointer;
        위해 (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            만약 (uprv_strcmp(data, (const char *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                반환 true;
            }
        }
    }
    반환 false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    만약 (list == nullptr || list->curr == nullptr) {
        반환 nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    반환 curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    만약 (list != nullptr) {
        반환 list->size;
    }
    
    반환 -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    만약 (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    만약 (list != nullptr) {
        listHead = list->head;
        동안 (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            만약 (listHead->forceDelete) {
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
    만약 (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    만약 (U_FAILURE(*status)) {
        반환 -1;
    }
    
    반환 ulist_getListSize((UList *)(en->context));
}

U_CAPI const char * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const char *s;
    만약 (U_FAILURE(*status)) {
        반환 nullptr;
    }

    s = (const char *)ulist_getNext((UList *)(en->context));
    만약 (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    반환 s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    만약 (U_FAILURE(*status)) {
        반환 ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    반환 (UList *)(en->context);
}
