// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#அந்தர்பாவ "ulist.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uenumimp.h"

வகைவரை கட்டமைப்பு UList_முனை UList_முனை;
கட்டமைப்பு UList_முனை {
    வெறுமை *data;
    
    UList_முனை *next;
    UList_முனை *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

கட்டமைப்பு UList {
    UList_முனை *curr;
    UList_முனை *head;
    UList_முனை *tail;
    
    int32_t size;
};

நிலையான வெறுமை ulist_addFirstItem(UList *list, UList_முனை *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }
    
    newList = (UList *)uprv_malloc(அளவு(UList));
    எனில் (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    திரும்பு newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
நிலையான வெறுமை ulist_addFirstItem(UList *list, UList_முனை *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

நிலையான வெறுமை ulist_removeItem(UList *list, UList_முனை *p) {
    எனில் (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } இல்லையெனில் {
        p->previous->next = p->next;
    }
    எனில் (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } இல்லையெனில் {
        p->next->previous = p->previous;
    }
    எனில் (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    எனில் (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI வெறுமை U_EXPORT2 ulist_addItemEndList(UList *list, மாறிலி வெறுமை *data, UBool forceDelete, UErrorCode *status) {
    UList_முனை *newItem = nullptr;
    
    எனில் (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        எனில் (forceDelete) {
            uprv_free((வெறுமை *)data);
        }
        திரும்பு;
    }
    
    newItem = (UList_முனை *)uprv_malloc(அளவு(UList_முனை));
    எனில் (newItem == nullptr) {
        எனில் (forceDelete) {
            uprv_free((வெறுமை *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    newItem->data = (வெறுமை *)(data);
    newItem->forceDelete = forceDelete;
    
    எனில் (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } இல்லையெனில் {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI வெறுமை U_EXPORT2 ulist_addItemBeginList(UList *list, மாறிலி வெறுமை *data, UBool forceDelete, UErrorCode *status) {
    UList_முனை *newItem = nullptr;
    
    எனில் (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        எனில் (forceDelete) {
            uprv_free((வெறுமை *)data);
        }
        திரும்பு;
    }
    
    newItem = (UList_முனை *)uprv_malloc(அளவு(UList_முனை));
    எனில் (newItem == nullptr) {
        எனில் (forceDelete) {
            uprv_free((வெறுமை *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    newItem->data = (வெறுமை *)(data);
    newItem->forceDelete = forceDelete;
    
    எனில் (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } இல்லையெனில் {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(மாறிலி UList *list, மாறிலி எழுத்து *data, int32_t length) {
    எனில் (list != nullptr) {
        மாறிலி UList_முனை *pointer;
        க்கு (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            எனில் (length == (int32_t)uprv_strlen((மாறிலி எழுத்து *)pointer->data)) {
                எனில் (uprv_memcmp(data, pointer->data, length) == 0) {
                    திரும்பு உண்மை;
                }
            }
        }
    }
    திரும்பு பொய்;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, மாறிலி எழுத்து *data) {
    எனில் (list != nullptr) {
        UList_முனை *pointer;
        க்கு (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            எனில் (uprv_strcmp(data, (மாறிலி எழுத்து *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                திரும்பு உண்மை;
            }
        }
    }
    திரும்பு பொய்;
}

U_CAPI வெறுமை *U_EXPORT2 ulist_getNext(UList *list) {
    UList_முனை *curr = nullptr;
    
    எனில் (list == nullptr || list->curr == nullptr) {
        திரும்பு nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    திரும்பு curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(மாறிலி UList *list) {
    எனில் (list != nullptr) {
        திரும்பு list->size;
    }
    
    திரும்பு -1;
}

U_CAPI வெறுமை U_EXPORT2 ulist_resetList(UList *list) {
    எனில் (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI வெறுமை U_EXPORT2 ulist_deleteList(UList *list) {
    UList_முனை *listHead = nullptr;

    எனில் (list != nullptr) {
        listHead = list->head;
        போது (listHead != nullptr) {
            UList_முனை *listPointer = listHead->next;

            எனில் (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI வெறுமை U_EXPORT2 ulist_close_keyword_மதிப்புகள்_iterator(UEnumeration *en) {
    எனில் (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_மொத்தம்_keyword_மதிப்புகள்(UEnumeration *en, UErrorCode *status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு -1;
    }
    
    திரும்பு ulist_getListSize((UList *)(en->context));
}

U_CAPI மாறிலி எழுத்து * U_EXPORT2 ulist_next_keyword_மதிப்பு(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    மாறிலி எழுத்து *s;
    எனில் (U_FAILURE(*status)) {
        திரும்பு nullptr;
    }

    s = (மாறிலி எழுத்து *)ulist_getNext((UList *)(en->context));
    எனில் (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    திரும்பு s;
}

U_CAPI வெறுமை U_EXPORT2 ulist_reset_keyword_மதிப்புகள்_iterator(UEnumeration *en, UErrorCode *status) {
    எனில் (U_FAILURE(*status)) {
        திரும்பு ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    திரும்பு (UList *)(en->context);
}
