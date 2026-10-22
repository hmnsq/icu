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

تعريفنوع بنية UList_گرێ UList_گرێ;
بنية UList_گرێ {
    فراغ *data;
    
    UList_گرێ *next;
    UList_گرێ *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UList_گرێ *curr;
    UList_گرێ *head;
    UList_گرێ *tail;
    
    int32_t size;
};

ساكن فراغ ulist_addFirstItem(UList *list, UList_گرێ *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    إذا (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    أرجع newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_addFirstItem(UList *list, UList_گرێ *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ساكن فراغ ulist_removeItem(UList *list, UList_گرێ *p) {
    إذا (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } وإلا {
        p->previous->next = p->next;
    }
    إذا (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } وإلا {
        p->next->previous = p->previous;
    }
    إذا (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    إذا (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_addItemEndList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status) {
    UList_گرێ *newItem = nullptr;
    
    إذا (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        إذا (forceDelete) {
            uprv_free((فراغ *)data);
        }
        أرجع;
    }
    
    newItem = (UList_گرێ *)uprv_malloc(حجم(UList_گرێ));
    إذا (newItem == nullptr) {
        إذا (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } وإلا {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_addItemBeginList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status) {
    UList_گرێ *newItem = nullptr;
    
    إذا (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        إذا (forceDelete) {
            uprv_free((فراغ *)data);
        }
        أرجع;
    }
    
    newItem = (UList_گرێ *)uprv_malloc(حجم(UList_گرێ));
    إذا (newItem == nullptr) {
        إذا (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } وإلا {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ثابت UList *list, ثابت حرف *data, int32_t length) {
    إذا (list != nullptr) {
        ثابت UList_گرێ *pointer;
        لكل (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            إذا (length == (int32_t)uprv_strlen((ثابت حرف *)pointer->data)) {
                إذا (uprv_memcmp(data, pointer->data, length) == 0) {
                    أرجع true;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *data) {
    إذا (list != nullptr) {
        UList_گرێ *pointer;
        لكل (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            إذا (uprv_strcmp(data, (ثابت حرف *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع true;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_getNext(UList *list) {
    UList_گرێ *curr = nullptr;
    
    إذا (list == nullptr || list->curr == nullptr) {
        أرجع nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list) {
    إذا (list != nullptr) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    إذا (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_گرێ *listHead = nullptr;

    إذا (list != nullptr) {
        listHead = list->head;
        طالما (listHead != nullptr) {
            UList_گرێ *listPointer = listHead->next;

            إذا (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_بەهاکان_iterator(UEnumeration *en) {
    إذا (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ژماردن_keyword_بەهاکان(UEnumeration *en, UErrorCode *status) {
    إذا (U_FAILURE(*status)) {
        أرجع -1;
    }
    
    أرجع ulist_getListSize((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_next_keyword_بەها(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ثابت حرف *s;
    إذا (U_FAILURE(*status)) {
        أرجع nullptr;
    }

    s = (ثابت حرف *)ulist_getNext((UList *)(en->context));
    إذا (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_بەهاکان_iterator(UEnumeration *en, UErrorCode *status) {
    إذا (U_FAILURE(*status)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
