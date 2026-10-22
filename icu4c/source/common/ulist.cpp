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
    
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    𐡄𐡍 (newList == nullptr) {
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
ساكن فراغ ulist_addFirstItem(UList *list, UListNode *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ساكن فراغ ulist_removeItem(UList *list, UListNode *p) {
    𐡄𐡍 (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } وإلا {
        p->previous->next = p->next;
    }
    𐡄𐡍 (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } وإلا {
        p->next->previous = p->previous;
    }
    𐡄𐡍 (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    𐡄𐡍 (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_addItemEndList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status) {
    UListNode *newItem = nullptr;
    
    𐡄𐡍 (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        𐡄𐡍 (forceDelete) {
            uprv_free((فراغ *)data);
        }
        أرجع;
    }
    
    newItem = (UListNode *)uprv_malloc(حجم(UListNode));
    𐡄𐡍 (newItem == nullptr) {
        𐡄𐡍 (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    𐡄𐡍 (list->size == 0) {
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
    UListNode *newItem = nullptr;
    
    𐡄𐡍 (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        𐡄𐡍 (forceDelete) {
            uprv_free((فراغ *)data);
        }
        أرجع;
    }
    
    newItem = (UListNode *)uprv_malloc(حجم(UListNode));
    𐡄𐡍 (newItem == nullptr) {
        𐡄𐡍 (forceDelete) {
            uprv_free((فراغ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    newItem->data = (فراغ *)(data);
    newItem->forceDelete = forceDelete;
    
    𐡄𐡍 (list->size == 0) {
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
    𐡄𐡍 (list != nullptr) {
        ثابت UListNode *pointer;
        لكل (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            𐡄𐡍 (length == (int32_t)uprv_strlen((ثابت حرف *)pointer->data)) {
                𐡄𐡍 (uprv_memcmp(data, pointer->data, length) == 0) {
                    أرجع 𐡒𐡔𐡈;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *data) {
    𐡄𐡍 (list != nullptr) {
        UListNode *pointer;
        لكل (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            𐡄𐡍 (uprv_strcmp(data, (ثابت حرف *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع 𐡒𐡔𐡈;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_getNext(UList *list) {
    UListNode *curr = nullptr;
    
    𐡄𐡍 (list == nullptr || list->curr == nullptr) {
        أرجع nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list) {
    𐡄𐡍 (list != nullptr) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    𐡄𐡍 (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UListNode *listHead = nullptr;

    𐡄𐡍 (list != nullptr) {
        listHead = list->head;
        طالما (listHead != nullptr) {
            UListNode *listPointer = listHead->next;

            𐡄𐡍 (listHead->forceDelete) {
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
    𐡄𐡍 (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_count_keyword_values(UEnumeration *en, UErrorCode *status) {
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع -1;
    }
    
    أرجع ulist_getListSize((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_next_keyword_value(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ثابت حرف *s;
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع nullptr;
    }

    s = (ثابت حرف *)ulist_getNext((UList *)(en->context));
    𐡄𐡍 (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_values_iterator(UEnumeration *en, UErrorCode *status) {
    𐡄𐡍 (U_FAILURE(*status)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
