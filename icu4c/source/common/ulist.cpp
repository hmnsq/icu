// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#अन्तर्भाव "ulist.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uenumimp.h"

प्रकारनिर्देश संरचना UList_ग्रन्थिः UList_ग्रन्थिः;
संरचना UList_ग्रन्थिः {
    शून्य *data;
    
    UList_ग्रन्थिः *next;
    UList_ग्रन्थिः *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

संरचना UList {
    UList_ग्रन्थिः *curr;
    UList_ग्रन्थिः *head;
    UList_ग्रन्थिः *tail;
    
    int32_t size;
};

स्थिर शून्य ulist_addFirstItem(UList *list, UList_ग्रन्थिः *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }
    
    newList = (UList *)uprv_malloc(परिमाण(UList));
    सचे (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    पटिनिवत्त newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
स्थिर शून्य ulist_addFirstItem(UList *list, UList_ग्रन्थिः *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

स्थिर शून्य ulist_removeItem(UList *list, UList_ग्रन्थिः *p) {
    सचे (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } अञ्ञथा {
        p->previous->next = p->next;
    }
    सचे (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } अञ्ञथा {
        p->next->previous = p->previous;
    }
    सचे (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    सचे (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI शून्य U_EXPORT2 ulist_addItemEndList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_ग्रन्थिः *newItem = nullptr;
    
    सचे (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        सचे (forceDelete) {
            uprv_free((शून्य *)data);
        }
        पटिनिवत्त;
    }
    
    newItem = (UList_ग्रन्थिः *)uprv_malloc(परिमाण(UList_ग्रन्थिः));
    सचे (newItem == nullptr) {
        सचे (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    सचे (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अञ्ञथा {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI शून्य U_EXPORT2 ulist_addItemBeginList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_ग्रन्थिः *newItem = nullptr;
    
    सचे (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        सचे (forceDelete) {
            uprv_free((शून्य *)data);
        }
        पटिनिवत्त;
    }
    
    newItem = (UList_ग्रन्थिः *)uprv_malloc(परिमाण(UList_ग्रन्थिः));
    सचे (newItem == nullptr) {
        सचे (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    सचे (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अञ्ञथा {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ध्रुव UList *list, ध्रुव वर्ण *data, int32_t length) {
    सचे (list != nullptr) {
        ध्रुव UList_ग्रन्थिः *pointer;
        अत्थाय (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            सचे (length == (int32_t)uprv_strlen((ध्रुव वर्ण *)pointer->data)) {
                सचे (uprv_memcmp(data, pointer->data, length) == 0) {
                    पटिनिवत्त सच्च;
                }
            }
        }
    }
    पटिनिवत्त मिच्छा;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ध्रुव वर्ण *data) {
    सचे (list != nullptr) {
        UList_ग्रन्थिः *pointer;
        अत्थाय (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            सचे (uprv_strcmp(data, (ध्रुव वर्ण *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                पटिनिवत्त सच्च;
            }
        }
    }
    पटिनिवत्त मिच्छा;
}

U_CAPI शून्य *U_EXPORT2 ulist_getNext(UList *list) {
    UList_ग्रन्थिः *curr = nullptr;
    
    सचे (list == nullptr || list->curr == nullptr) {
        पटिनिवत्त nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    पटिनिवत्त curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ध्रुव UList *list) {
    सचे (list != nullptr) {
        पटिनिवत्त list->size;
    }
    
    पटिनिवत्त -1;
}

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list) {
    सचे (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ग्रन्थिः *listHead = nullptr;

    सचे (list != nullptr) {
        listHead = list->head;
        याव (listHead != nullptr) {
            UList_ग्रन्थिः *listPointer = listHead->next;

            सचे (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मानानि_iterator(UEnumeration *en) {
    सचे (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_गणनम्_keyword_मानानि(UEnumeration *en, UErrorCode *status) {
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त -1;
    }
    
    पटिनिवत्त ulist_getListSize((UList *)(en->context));
}

U_CAPI ध्रुव वर्ण * U_EXPORT2 ulist_next_keyword_मानम्(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ध्रुव वर्ण *s;
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त nullptr;
    }

    s = (ध्रुव वर्ण *)ulist_getNext((UList *)(en->context));
    सचे (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    पटिनिवत्त s;
}

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मानानि_iterator(UEnumeration *en, UErrorCode *status) {
    सचे (U_FAILURE(*status)) {
        पटिनिवत्त ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    पटिनिवत्त (UList *)(en->context);
}
