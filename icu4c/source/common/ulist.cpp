// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#شامل "ulist.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uenumimp.h"

تعریفنوع ساختار UList_گره UList_گره;
ساختار UList_گره {
    تهی *data;
    
    UList_گره *next;
    UList_گره *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ساختار UList {
    UList_گره *curr;
    UList_گره *head;
    UList_گره *tail;
    
    int32_t size;
};

ایستا تهی ulist_addFirstItem(UList *list, UList_گره *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }
    
    newList = (UList *)uprv_malloc(اندازه(UList));
    اگر (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    بازگشت newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ایستا تهی ulist_addFirstItem(UList *list, UList_گره *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ایستا تهی ulist_removeItem(UList *list, UList_گره *p) {
    اگر (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } وگرنه {
        p->previous->next = p->next;
    }
    اگر (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } وگرنه {
        p->next->previous = p->previous;
    }
    اگر (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    اگر (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI تهی U_EXPORT2 ulist_addItemEndList(UList *list, ثابت تهی *data, UBool forceDelete, UErrorCode *status) {
    UList_گره *newItem = nullptr;
    
    اگر (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        اگر (forceDelete) {
            uprv_free((تهی *)data);
        }
        بازگشت;
    }
    
    newItem = (UList_گره *)uprv_malloc(اندازه(UList_گره));
    اگر (newItem == nullptr) {
        اگر (forceDelete) {
            uprv_free((تهی *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    newItem->data = (تهی *)(data);
    newItem->forceDelete = forceDelete;
    
    اگر (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } وگرنه {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI تهی U_EXPORT2 ulist_addItemBeginList(UList *list, ثابت تهی *data, UBool forceDelete, UErrorCode *status) {
    UList_گره *newItem = nullptr;
    
    اگر (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        اگر (forceDelete) {
            uprv_free((تهی *)data);
        }
        بازگشت;
    }
    
    newItem = (UList_گره *)uprv_malloc(اندازه(UList_گره));
    اگر (newItem == nullptr) {
        اگر (forceDelete) {
            uprv_free((تهی *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    newItem->data = (تهی *)(data);
    newItem->forceDelete = forceDelete;
    
    اگر (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } وگرنه {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ثابت UList *list, ثابت نویسه *data, int32_t length) {
    اگر (list != nullptr) {
        ثابت UList_گره *pointer;
        برای (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            اگر (length == (int32_t)uprv_strlen((ثابت نویسه *)pointer->data)) {
                اگر (uprv_memcmp(data, pointer->data, length) == 0) {
                    بازگشت درست;
                }
            }
        }
    }
    بازگشت نادرست;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت نویسه *data) {
    اگر (list != nullptr) {
        UList_گره *pointer;
        برای (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            اگر (uprv_strcmp(data, (ثابت نویسه *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                بازگشت درست;
            }
        }
    }
    بازگشت نادرست;
}

U_CAPI تهی *U_EXPORT2 ulist_getNext(UList *list) {
    UList_گره *curr = nullptr;
    
    اگر (list == nullptr || list->curr == nullptr) {
        بازگشت nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    بازگشت curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list) {
    اگر (list != nullptr) {
        بازگشت list->size;
    }
    
    بازگشت -1;
}

U_CAPI تهی U_EXPORT2 ulist_resetList(UList *list) {
    اگر (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI تهی U_EXPORT2 ulist_deleteList(UList *list) {
    UList_گره *listHead = nullptr;

    اگر (list != nullptr) {
        listHead = list->head;
        وقتی (listHead != nullptr) {
            UList_گره *listPointer = listHead->next;

            اگر (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI تهی U_EXPORT2 ulist_close_keyword_مقادیر_iterator(UEnumeration *en) {
    اگر (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_شمار_keyword_مقادیر(UEnumeration *en, UErrorCode *status) {
    اگر (U_FAILURE(*status)) {
        بازگشت -1;
    }
    
    بازگشت ulist_getListSize((UList *)(en->context));
}

U_CAPI ثابت نویسه * U_EXPORT2 ulist_next_keyword_مقدار(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ثابت نویسه *s;
    اگر (U_FAILURE(*status)) {
        بازگشت nullptr;
    }

    s = (ثابت نویسه *)ulist_getNext((UList *)(en->context));
    اگر (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    بازگشت s;
}

U_CAPI تهی U_EXPORT2 ulist_reset_keyword_مقادیر_iterator(UEnumeration *en, UErrorCode *status) {
    اگر (U_FAILURE(*status)) {
        بازگشت ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    بازگشت (UList *)(en->context);
}
