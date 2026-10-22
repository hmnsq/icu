// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#অন্তর্ভাৱ "ulist.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "uenumimp.h"

প্রকারনির্দেশ সংরচনা UList_গ্ৰন্থি UList_গ্ৰন্থি;
সংরচনা UList_গ্ৰন্থি {
    শূন্য *data;
    
    UList_গ্ৰন্থি *next;
    UList_গ্ৰন্থি *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

সংরচনা UList {
    UList_গ্ৰন্থি *curr;
    UList_গ্ৰন্থি *head;
    UList_গ্ৰন্থি *tail;
    
    int32_t size;
};

স্থির শূন্য ulist_addFirstItem(UList *list, UList_গ্ৰন্থি *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    যদি (U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }
    
    newList = (UList *)uprv_malloc(পরিমাণ(UList));
    যদি (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ঘূৰাই newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
স্থির শূন্য ulist_addFirstItem(UList *list, UList_গ্ৰন্থি *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

স্থির শূন্য ulist_removeItem(UList *list, UList_গ্ৰন্থি *p) {
    যদি (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } অন্যথা {
        p->previous->next = p->next;
    }
    যদি (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } অন্যথা {
        p->next->previous = p->previous;
    }
    যদি (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    যদি (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI শূন্য U_EXPORT2 ulist_addItemEndList(UList *list, ধ্রুৱ শূন্য *data, UBool forceDelete, UErrorCode *status) {
    UList_গ্ৰন্থি *newItem = nullptr;
    
    যদি (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)data);
        }
        ঘূৰাই;
    }
    
    newItem = (UList_গ্ৰন্থি *)uprv_malloc(পরিমাণ(UList_গ্ৰন্থি));
    যদি (newItem == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    newItem->data = (শূন্য *)(data);
    newItem->forceDelete = forceDelete;
    
    যদি (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } অন্যথা {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI শূন্য U_EXPORT2 ulist_addItemBeginList(UList *list, ধ্রুৱ শূন্য *data, UBool forceDelete, UErrorCode *status) {
    UList_গ্ৰন্থি *newItem = nullptr;
    
    যদি (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)data);
        }
        ঘূৰাই;
    }
    
    newItem = (UList_গ্ৰন্থি *)uprv_malloc(পরিমাণ(UList_গ্ৰন্থি));
    যদি (newItem == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই;
    }
    newItem->data = (শূন্য *)(data);
    newItem->forceDelete = forceDelete;
    
    যদি (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } অন্যথা {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ধ্রুৱ UList *list, ধ্রুৱ ৱর্ণ *data, int32_t length) {
    যদি (list != nullptr) {
        ধ্রুৱ UList_গ্ৰন্থি *pointer;
        বাবে (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            যদি (length == (int32_t)uprv_strlen((ধ্রুৱ ৱর্ণ *)pointer->data)) {
                যদি (uprv_memcmp(data, pointer->data, length) == 0) {
                    ঘূৰাই সঁচা;
                }
            }
        }
    }
    ঘূৰাই মিছা;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ধ্রুৱ ৱর্ণ *data) {
    যদি (list != nullptr) {
        UList_গ্ৰন্থি *pointer;
        বাবে (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            যদি (uprv_strcmp(data, (ধ্রুৱ ৱর্ণ *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ঘূৰাই সঁচা;
            }
        }
    }
    ঘূৰাই মিছা;
}

U_CAPI শূন্য *U_EXPORT2 ulist_getNext(UList *list) {
    UList_গ্ৰন্থি *curr = nullptr;
    
    যদি (list == nullptr || list->curr == nullptr) {
        ঘূৰাই nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ঘূৰাই curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ধ্রুৱ UList *list) {
    যদি (list != nullptr) {
        ঘূৰাই list->size;
    }
    
    ঘূৰাই -1;
}

U_CAPI শূন্য U_EXPORT2 ulist_resetList(UList *list) {
    যদি (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI শূন্য U_EXPORT2 ulist_deleteList(UList *list) {
    UList_গ্ৰন্থি *listHead = nullptr;

    যদি (list != nullptr) {
        listHead = list->head;
        যেতিয়ালৈ (listHead != nullptr) {
            UList_গ্ৰন্থি *listPointer = listHead->next;

            যদি (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI শূন্য U_EXPORT2 ulist_close_keyword_মানসমূহ_iterator(UEnumeration *en) {
    যদি (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_গুনতি_keyword_মানসমূহ(UEnumeration *en, UErrorCode *status) {
    যদি (U_FAILURE(*status)) {
        ঘূৰাই -1;
    }
    
    ঘূৰাই ulist_getListSize((UList *)(en->context));
}

U_CAPI ধ্রুৱ ৱর্ণ * U_EXPORT2 ulist_next_keyword_মান(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ধ্রুৱ ৱর্ণ *s;
    যদি (U_FAILURE(*status)) {
        ঘূৰাই nullptr;
    }

    s = (ধ্রুৱ ৱর্ণ *)ulist_getNext((UList *)(en->context));
    যদি (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ঘূৰাই s;
}

U_CAPI শূন্য U_EXPORT2 ulist_reset_keyword_মানসমূহ_iterator(UEnumeration *en, UErrorCode *status) {
    যদি (U_FAILURE(*status)) {
        ঘূৰাই ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ঘূৰাই (UList *)(en->context);
}
