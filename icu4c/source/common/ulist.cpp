// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "ulist.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UList_ಗಂಟು UList_ಗಂಟು;
ಸಂರಚನಾ UList_ಗಂಟು {
    ಶೂನ್ಯ *data;
    
    UList_ಗಂಟು *next;
    UList_ಗಂಟು *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ಸಂರಚನಾ UList {
    UList_ಗಂಟು *curr;
    UList_ಗಂಟು *head;
    UList_ಗಂಟು *tail;
    
    int32_t size;
};

ಸ್ಥಿರ ಶೂನ್ಯ ulist_addFirstItem(UList *list, UList_ಗಂಟು *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
    newList = (UList *)uprv_malloc(ಪರಿಮಾಣ(UList));
    ಯದಿ (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ಪ್ರತ್ಯಾಗಮ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ulist_addFirstItem(UList *list, UList_ಗಂಟು *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ಸ್ಥಿರ ಶೂನ್ಯ ulist_removeItem(UList *list, UList_ಗಂಟು *p) {
    ಯದಿ (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } ಅನ್ಯಥಾ {
        p->previous->next = p->next;
    }
    ಯದಿ (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } ಅನ್ಯಥಾ {
        p->next->previous = p->previous;
    }
    ಯದಿ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ಯದಿ (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_addItemEndList(UList *list, ಧ್ರುವ ಶೂನ್ಯ *data, UBool forceDelete, UErrorCode *status) {
    UList_ಗಂಟು *newItem = nullptr;
    
    ಯದಿ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ಯದಿ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)data);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    
    newItem = (UList_ಗಂಟು *)uprv_malloc(ಪರಿಮಾಣ(UList_ಗಂಟು));
    ಯದಿ (newItem == nullptr) {
        ಯದಿ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    newItem->data = (ಶೂನ್ಯ *)(data);
    newItem->forceDelete = forceDelete;
    
    ಯದಿ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ಅನ್ಯಥಾ {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_addItemBeginList(UList *list, ಧ್ರುವ ಶೂನ್ಯ *data, UBool forceDelete, UErrorCode *status) {
    UList_ಗಂಟು *newItem = nullptr;
    
    ಯದಿ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ಯದಿ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)data);
        }
        ಪ್ರತ್ಯಾಗಮ;
    }
    
    newItem = (UList_ಗಂಟು *)uprv_malloc(ಪರಿಮಾಣ(UList_ಗಂಟು));
    ಯದಿ (newItem == nullptr) {
        ಯದಿ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    newItem->data = (ಶೂನ್ಯ *)(data);
    newItem->forceDelete = forceDelete;
    
    ಯದಿ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ಅನ್ಯಥಾ {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ಧ್ರುವ UList *list, ಧ್ರುವ ವರ್ಣ *data, int32_t length) {
    ಯದಿ (list != nullptr) {
        ಧ್ರುವ UList_ಗಂಟು *pointer;
        ಪ್ರತಿ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ಯದಿ (length == (int32_t)uprv_strlen((ಧ್ರುವ ವರ್ಣ *)pointer->data)) {
                ಯದಿ (uprv_memcmp(data, pointer->data, length) == 0) {
                    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
                }
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ಧ್ರುವ ವರ್ಣ *data) {
    ಯದಿ (list != nullptr) {
        UList_ಗಂಟು *pointer;
        ಪ್ರತಿ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ಯದಿ (uprv_strcmp(data, (ಧ್ರುವ ವರ್ಣ *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

U_CAPI ಶೂನ್ಯ *U_EXPORT2 ulist_getNext(UList *list) {
    UList_ಗಂಟು *curr = nullptr;
    
    ಯದಿ (list == nullptr || list->curr == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ಪ್ರತ್ಯಾಗಮ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ಧ್ರುವ UList *list) {
    ಯದಿ (list != nullptr) {
        ಪ್ರತ್ಯಾಗಮ list->size;
    }
    
    ಪ್ರತ್ಯಾಗಮ -1;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_resetList(UList *list) {
    ಯದಿ (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ಗಂಟು *listHead = nullptr;

    ಯದಿ (list != nullptr) {
        listHead = list->head;
        ಯಾವತ್ (listHead != nullptr) {
            UList_ಗಂಟು *listPointer = listHead->next;

            ಯದಿ (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en) {
    ಯದಿ (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು(UEnumeration *en, UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ -1;
    }
    
    ಪ್ರತ್ಯಾಗಮ ulist_getListSize((UList *)(en->context));
}

U_CAPI ಧ್ರುವ ವರ್ಣ * U_EXPORT2 ulist_next_keyword_ಮೌಲ್ಯ(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ಧ್ರುವ ವರ್ಣ *s;
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    s = (ಧ್ರುವ ವರ್ಣ *)ulist_getNext((UList *)(en->context));
    ಯದಿ (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ಪ್ರತ್ಯಾಗಮ s;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en, UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) {
        ಪ್ರತ್ಯಾಗಮ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ಪ್ರತ್ಯಾಗಮ (UList *)(en->context);
}
