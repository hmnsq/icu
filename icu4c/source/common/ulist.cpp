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
    
    ຖ້າ (U_FAILURE(*status)) {
        ສົ່ງຄືນ nullptr;
    }
    
    newList = (UList *)uprv_malloc(parimaa_na(UList));
    ຖ້າ (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ສົ່ງຄືນ newList;
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
    ຖ້າ (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } ບໍ່ດັ່ງນັ້ນ {
        p->previous->next = p->next;
    }
    ຖ້າ (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } ບໍ່ດັ່ງນັ້ນ {
        p->next->previous = p->previous;
    }
    ຖ້າ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ຖ້າ (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI void U_EXPORT2 ulist_addItemEndList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ຖ້າ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ຖ້າ (forceDelete) {
            uprv_free((void *)data);
        }
        ສົ່ງຄືນ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ຖ້າ (newItem == nullptr) {
        ຖ້າ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ຖ້າ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ບໍ່ດັ່ງນັ້ນ {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI void U_EXPORT2 ulist_addItemBeginList(UList *list, const void *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    ຖ້າ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ຖ້າ (forceDelete) {
            uprv_free((void *)data);
        }
        ສົ່ງຄືນ;
    }
    
    newItem = (UListNode *)uprv_malloc(parimaa_na(UListNode));
    ຖ້າ (newItem == nullptr) {
        ຖ້າ (forceDelete) {
            uprv_free((void *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    newItem->data = (void *)(data);
    newItem->forceDelete = forceDelete;
    
    ຖ້າ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ບໍ່ດັ່ງນັ້ນ {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(const UList *list, const var_na *data, int32_t length) {
    ຖ້າ (list != nullptr) {
        const UListNode *pointer;
        ສຳລັບ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ຖ້າ (length == (int32_t)uprv_strlen((const var_na *)pointer->data)) {
                ຖ້າ (uprv_memcmp(data, pointer->data, length) == 0) {
                    ສົ່ງຄືນ true;
                }
            }
        }
    }
    ສົ່ງຄືນ false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, const var_na *data) {
    ຖ້າ (list != nullptr) {
        UListNode *pointer;
        ສຳລັບ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ຖ້າ (uprv_strcmp(data, (const var_na *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ສົ່ງຄືນ true;
            }
        }
    }
    ສົ່ງຄືນ false;
}

U_CAPI void *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    ຖ້າ (list == nullptr || list->curr == nullptr) {
        ສົ່ງຄືນ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ສົ່ງຄືນ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(const UList *list) {
    ຖ້າ (list != nullptr) {
        ສົ່ງຄືນ list->size;
    }
    
    ສົ່ງຄືນ -1;
}

U_CAPI void U_EXPORT2 ulist_resetList(UList *list) {
    ຖ້າ (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI void U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    ຖ້າ (list != nullptr) {
        listHead = list->head;
        ໃນຂະນະທີ່ (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            ຖ້າ (listHead->forceDelete) {
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
    ຖ້າ (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    ຖ້າ (U_FAILURE(*status)) {
        ສົ່ງຄືນ -1;
    }
    
    ສົ່ງຄືນ ulist_getListSize((UList *)(en->context));
}

U_CAPI const var_na * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    const var_na *s;
    ຖ້າ (U_FAILURE(*status)) {
        ສົ່ງຄືນ nullptr;
    }

    s = (const var_na *)ulist_getNext((UList *)(en->context));
    ຖ້າ (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ສົ່ງຄືນ s;
}

U_CAPI void U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    ຖ້າ (U_FAILURE(*status)) {
        ສົ່ງຄືນ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ສົ່ງຄືນ (UList *)(en->context);
}
