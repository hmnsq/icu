// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "ulist.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UList_ଗଣ୍ଠି UList_ଗଣ୍ଠି;
ସଂରଚନା UList_ଗଣ୍ଠି {
    ଶୂନ୍ୟ *data;
    
    UList_ଗଣ୍ଠି *next;
    UList_ଗଣ୍ଠି *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ସଂରଚନା UList {
    UList_ଗଣ୍ଠି *curr;
    UList_ଗଣ୍ଠି *head;
    UList_ଗଣ୍ଠି *tail;
    
    int32_t size;
};

ସ୍ଥିର ଶୂନ୍ୟ ulist_addFirstItem(UList *list, UList_ଗଣ୍ଠି *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    
    newList = (UList *)uprv_malloc(ଆକାର(UList));
    ଯଦି (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    ଫେରନ୍ତୁ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ସ୍ଥିର ଶୂନ୍ୟ ulist_addFirstItem(UList *list, UList_ଗଣ୍ଠି *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

ସ୍ଥିର ଶୂନ୍ୟ ulist_removeItem(UList *list, UList_ଗଣ୍ଠି *p) {
    ଯଦି (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } ଅନ୍ୟଥା {
        p->previous->next = p->next;
    }
    ଯଦି (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } ଅନ୍ୟଥା {
        p->next->previous = p->previous;
    }
    ଯଦି (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ଯଦି (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_addItemEndList(UList *list, ଅଚଳ ଶୂନ୍ୟ *data, UBool forceDelete, UErrorCode *status) {
    UList_ଗଣ୍ଠି *newItem = nullptr;
    
    ଯଦି (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ଯଦି (forceDelete) {
            uprv_free((ଶୂନ୍ୟ *)data);
        }
        ଫେରନ୍ତୁ;
    }
    
    newItem = (UList_ଗଣ୍ଠି *)uprv_malloc(ଆକାର(UList_ଗଣ୍ଠି));
    ଯଦି (newItem == nullptr) {
        ଯଦି (forceDelete) {
            uprv_free((ଶୂନ୍ୟ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    newItem->data = (ଶୂନ୍ୟ *)(data);
    newItem->forceDelete = forceDelete;
    
    ଯଦି (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ଅନ୍ୟଥା {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_addItemBeginList(UList *list, ଅଚଳ ଶୂନ୍ୟ *data, UBool forceDelete, UErrorCode *status) {
    UList_ଗଣ୍ଠି *newItem = nullptr;
    
    ଯଦି (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        ଯଦି (forceDelete) {
            uprv_free((ଶୂନ୍ୟ *)data);
        }
        ଫେରନ୍ତୁ;
    }
    
    newItem = (UList_ଗଣ୍ଠି *)uprv_malloc(ଆକାର(UList_ଗଣ୍ଠି));
    ଯଦି (newItem == nullptr) {
        ଯଦି (forceDelete) {
            uprv_free((ଶୂନ୍ୟ *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    newItem->data = (ଶୂନ୍ୟ *)(data);
    newItem->forceDelete = forceDelete;
    
    ଯଦି (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } ଅନ୍ୟଥା {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ଅଚଳ UList *list, ଅଚଳ ଅକ୍ଷର *data, int32_t length) {
    ଯଦି (list != nullptr) {
        ଅଚଳ UList_ଗଣ୍ଠି *pointer;
        ପାଇଁ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ଯଦି (length == (int32_t)uprv_strlen((ଅଚଳ ଅକ୍ଷର *)pointer->data)) {
                ଯଦି (uprv_memcmp(data, pointer->data, length) == 0) {
                    ଫେରନ୍ତୁ ସତ୍ୟ;
                }
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ଅଚଳ ଅକ୍ଷର *data) {
    ଯଦି (list != nullptr) {
        UList_ଗଣ୍ଠି *pointer;
        ପାଇଁ (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            ଯଦି (uprv_strcmp(data, (ଅଚଳ ଅକ୍ଷର *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ଫେରନ୍ତୁ ସତ୍ୟ;
            }
        }
    }
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

U_CAPI ଶୂନ୍ୟ *U_EXPORT2 ulist_getNext(UList *list) {
    UList_ଗଣ୍ଠି *curr = nullptr;
    
    ଯଦି (list == nullptr || list->curr == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ଫେରନ୍ତୁ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ଅଚଳ UList *list) {
    ଯଦି (list != nullptr) {
        ଫେରନ୍ତୁ list->size;
    }
    
    ଫେରନ୍ତୁ -1;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_resetList(UList *list) {
    ଯଦି (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ଗଣ୍ଠି *listHead = nullptr;

    ଯଦି (list != nullptr) {
        listHead = list->head;
        ଯେତେବେଳେ (listHead != nullptr) {
            UList_ଗଣ୍ଠି *listPointer = listHead->next;

            ଯଦି (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_close_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator(UEnumeration *en) {
    ଯଦି (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ଗଣନା_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ(UEnumeration *en, UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ -1;
    }
    
    ଫେରନ୍ତୁ ulist_getListSize((UList *)(en->context));
}

U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2 ulist_next_keyword_ମୂଲ୍ୟ(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ଅଚଳ ଅକ୍ଷର *s;
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    s = (ଅଚଳ ଅକ୍ଷର *)ulist_getNext((UList *)(en->context));
    ଯଦି (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    ଫେରନ୍ତୁ s;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_reset_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator(UEnumeration *en, UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) {
        ଫେରନ୍ତୁ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ଫେରନ୍ତୁ (UList *)(en->context);
}
