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
    
    ᱡᱩᱫᱤ (U_FAILURE(*status)) {
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }
    
    newList = (UList *)uprv_malloc(parimaa_na(UList));
    ᱡᱩᱫᱤ (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ᱨᱩᱣᱟᱹᱲ newList;
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
    ᱡᱩᱫᱤ (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } ᱵᱟᱝ {
        p->previous->next = p->next;
    }
    ᱡᱩᱫᱤ (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } ᱵᱟᱝ {
        p->next->previous = p->previous;
    }
    ᱡᱩᱫᱤ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ᱡᱩᱫᱤ (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ᱡᱩᱫᱤ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ᱡᱩᱫᱤ (forceDelete) {
            uprv_free((void *)data);
        }
        ᱨᱩᱣᱟᱹᱲ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ᱡᱩᱫᱤ (newItem == nullptr) {
        ᱡᱩᱫᱤ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᱨᱩᱣᱟᱹᱲ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ᱡᱩᱫᱤ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ᱵᱟᱝ {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ᱡᱩᱫᱤ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ᱡᱩᱫᱤ (forceDelete) {
            uprv_free((void *)data);
        }
        ᱨᱩᱣᱟᱹᱲ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ᱡᱩᱫᱤ (newItem == nullptr) {
        ᱡᱩᱫᱤ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ᱨᱩᱣᱟᱹᱲ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ᱡᱩᱫᱤ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ᱵᱟᱝ {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const var_na *data, int32_t length) {
    ᱡᱩᱫᱤ (list != nullptr) {
        const UListNode *pointer;
        for (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ᱡᱩᱫᱤ (length == (int32_t)uprv_strlen((const var_na *)pointer->data)) {
                ᱡᱩᱫᱤ (uprv_memcmp(data, pointer->data, length) == 0) {
                    ᱨᱩᱣᱟᱹᱲ true;
                }
            }
        }
    }
    ᱨᱩᱣᱟᱹᱲ false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const var_na *data) {
    ᱡᱩᱫᱤ (list != nullptr) {
        UListNode *pointer;
        for (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ᱡᱩᱫᱤ (uprv_strcmp(data, (const var_na *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ᱨᱩᱣᱟᱹᱲ true;
            }
        }
    }
    ᱨᱩᱣᱟᱹᱲ false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    ᱡᱩᱫᱤ (list == nullptr || list->curr == nullptr) {
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ᱨᱩᱣᱟᱹᱲ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    ᱡᱩᱫᱤ (list != nullptr) {
        ᱨᱩᱣᱟᱹᱲ list->size;
    }
    
    ᱨᱩᱣᱟᱹᱲ -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    ᱡᱩᱫᱤ (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    ᱡᱩᱫᱤ (list != nullptr) {
        listHead = list->head;
        ᱡᱚᱠᱷᱚᱱ (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            ᱡᱩᱫᱤ (listHead->forceDelete) {
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
    ᱡᱩᱫᱤ (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    ᱡᱩᱫᱤ (U_FAILURE(*status)) {
        ᱨᱩᱣᱟᱹᱲ -1;
    }
    
    ᱨᱩᱣᱟᱹᱲ ulist_getListSize((UList *)(en->context));
}

U_CAPI const var_na * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const var_na *s;
    ᱡᱩᱫᱤ (U_FAILURE(*status)) {
        ᱨᱩᱣᱟᱹᱲ nullptr;
    }

    s = (const var_na *)ulist_getNext((UList *)(en->context));
    ᱡᱩᱫᱤ (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ᱨᱩᱣᱟᱹᱲ s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    ᱡᱩᱫᱤ (U_FAILURE(*status)) {
        ᱨᱩᱣᱟᱹᱲ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ᱨᱩᱣᱟᱹᱲ (UList *)(en->context);
}
