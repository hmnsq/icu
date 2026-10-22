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

typedef sa_nracanaa UListNode UListNode;
sa_nracanaa UListNode {
    void *data;
    
    UListNode *next;
    UListNode *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

sa_nracanaa UList {
    UListNode *curr;
    UListNode *head;
    UListNode *tail;
    
    int32_t size;
};

static void ulist_addFirstItem(UList *list, UListNode *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    ᯀᯮᯝ (U_FAILURE(*status)) {
        ᯀᯮᯞ nullptr;
    }
    
    newList = (UList *)uprv_malloc(parimaa_na(UList));
    ᯀᯮᯝ (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ᯀᯮᯞ newList;
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
    ᯀᯮᯝ (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } ᯀᯮᯝᯂᯪ {
        p->previous->next = p->next;
    }
    ᯀᯮᯝ (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } ᯀᯮᯝᯂᯪ {
        p->next->previous = p->previous;
    }
    ᯀᯮᯝ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ᯀᯮᯝ (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ᯀᯮᯝ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ᯀᯮᯝ (forceDelete) {
            uprv_free((void *)data);
        }
        ᯀᯮᯞ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ᯀᯮᯝ (newItem == nullptr) {
        ᯀᯮᯝ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ᯀᯮᯝ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ᯀᯮᯝᯂᯪ {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ᯀᯮᯝ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ᯀᯮᯝ (forceDelete) {
            uprv_free((void *)data);
        }
        ᯀᯮᯞ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ᯀᯮᯝ (newItem == nullptr) {
        ᯀᯮᯝ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᯀᯮᯞ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ᯀᯮᯝ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ᯀᯮᯝᯂᯪ {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const var_na *data, int32_t length) {
    ᯀᯮᯝ (list != nullptr) {
        const UListNode *pointer;
        for (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ᯀᯮᯝ (length == (int32_t)uprv_strlen((const var_na *)pointer->data)) {
                ᯀᯮᯝ (uprv_memcmp(data, pointer->data, length) == 0) {
                    ᯀᯮᯞ true;
                }
            }
        }
    }
    ᯀᯮᯞ false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const var_na *data) {
    ᯀᯮᯝ (list != nullptr) {
        UListNode *pointer;
        for (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ᯀᯮᯝ (uprv_strcmp(data, (const var_na *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ᯀᯮᯞ true;
            }
        }
    }
    ᯀᯮᯞ false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    ᯀᯮᯝ (list == nullptr || list->curr == nullptr) {
        ᯀᯮᯞ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ᯀᯮᯞ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    ᯀᯮᯝ (list != nullptr) {
        ᯀᯮᯞ list->size;
    }
    
    ᯀᯮᯞ -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    ᯀᯮᯝ (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    ᯀᯮᯝ (list != nullptr) {
        listHead = list->head;
        ᯀᯬᯝ (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            ᯀᯮᯝ (listHead->forceDelete) {
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
    ᯀᯮᯝ (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    ᯀᯮᯝ (U_FAILURE(*status)) {
        ᯀᯮᯞ -1;
    }
    
    ᯀᯮᯞ ulist_getListSize((UList *)(en->context));
}

U_CAPI const var_na * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const var_na *s;
    ᯀᯮᯝ (U_FAILURE(*status)) {
        ᯀᯮᯞ nullptr;
    }

    s = (const var_na *)ulist_getNext((UList *)(en->context));
    ᯀᯮᯝ (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ᯀᯮᯞ s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    ᯀᯮᯝ (U_FAILURE(*status)) {
        ᯀᯮᯞ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ᯀᯮᯞ (UList *)(en->context);
}
