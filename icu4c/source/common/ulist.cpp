// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ضمن "ulist.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uenumimp.h"

تعريفنوع بنية UListNode UListNode;
بنية UListNode {
    فراغ *data;
    
    UListNode *next;
    UListNode *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UListNode *curr;
    UListNode *head;
    UListNode *tail;
    
    int32_t size;
};

ساكن فراغ ulist_addFirstItem(UList *list, UListNode *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    אם (U_FAILURE(*status)) {
        החזר nullptr;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    אם (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    החזר newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_addFirstItem(UList *list, UListNode *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ساكن فراغ ulist_removeItem(UList *list, UListNode *p) {
    אם (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } אחרת {
        p->previous->next = p->next;
    }
    אם (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } אחרת {
        p->next->previous = p->previous;
    }
    אם (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    אם (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_addItemEndList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    אם (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        אם (forceDelete) {
            uprv_free((فراغ *)data);
        }
        החזר;
    }
    
    newItem = (UListNode *)uprv_malloc(حجم(UListNode));
    אם (newItem == nullptr) {
        אם (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        החזר;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    אם (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } אחרת {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_addItemBeginList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    אם (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        אם (forceDelete) {
            uprv_free((فراغ *)data);
        }
        החזר;
    }
    
    newItem = (UListNode *)uprv_malloc(حجم(UListNode));
    אם (newItem == nullptr) {
        אם (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        החזר;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    אם (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } אחרת {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ثابت UList *list, ثابت حرف *data, int32_t length) {
    אם (list != nullptr) {
        ثابت UListNode *pointer;
        עבור (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            אם (length == (int32_t)uprv_strlen((ثابت حرف *)pointer->data)) {
                אם (uprv_memcmp(data, pointer->data, length) == 0) {
                    החזר אמת;
                }
            }
        }
    }
    החזר שקר;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *data) {
    אם (list != nullptr) {
        UListNode *pointer;
        עבור (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            אם (uprv_strcmp(data, (ثابت حرف *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                החזר אמת;
            }
        }
    }
    החזר שקר;
}

U_CAPI فراغ *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    אם (list == nullptr || list->curr == nullptr) {
        החזר nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    החזר curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list) {
    אם (list != nullptr) {
        החזר list->size;
    }
    
    החזר -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    אם (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    אם (list != nullptr) {
        listHead = list->head;
        כלעוד (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            אם (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_values_iterator(UEnumeration *en) {
    אם (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    אם (U_FAILURE(*status)) {
        החזר -1;
    }
    
    החזר ulist_getListSize((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ثابت حرف *s;
    אם (U_FAILURE(*status)) {
        החזר nullptr;
    }

    s = (ثابت حرف *)ulist_getNext((UList *)(en->context));
    אם (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    החזר s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    אם (U_FAILURE(*status)) {
        החזר ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    החזר (UList *)(en->context);
}
