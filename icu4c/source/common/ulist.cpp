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

प्रकारनिर्देश संरचना UList_गाठ UList_गाठ;
संरचना UList_गाठ {
    शून्य *data;
    
    UList_गाठ *next;
    UList_गाठ *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

संरचना UList {
    UList_गाठ *curr;
    UList_गाठ *head;
    UList_गाठ *tail;
    
    int32_t size;
};

स्थिर शून्य ulist_addFirstItem(UList *list, UList_गाठ *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    यदि (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }
    
    newList = (UList *)uprv_malloc(परिमाण(UList));
    यदि (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    प्रत्यागम newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
स्थिर शून्य ulist_addFirstItem(UList *list, UList_गाठ *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

स्थिर शून्य ulist_removeItem(UList *list, UList_गाठ *p) {
    यदि (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } अन्यथा {
        p->previous->next = p->next;
    }
    यदि (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } अन्यथा {
        p->next->previous = p->previous;
    }
    यदि (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    यदि (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI शून्य U_EXPORT2 ulist_addItemEndList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाठ *newItem = nullptr;
    
    यदि (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        प्रत्यागम;
    }
    
    newItem = (UList_गाठ *)uprv_malloc(परिमाण(UList_गाठ));
    यदि (newItem == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    यदि (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI शून्य U_EXPORT2 ulist_addItemBeginList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाठ *newItem = nullptr;
    
    यदि (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        प्रत्यागम;
    }
    
    newItem = (UList_गाठ *)uprv_malloc(परिमाण(UList_गाठ));
    यदि (newItem == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    यदि (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ध्रुव UList *list, ध्रुव वर्ण *data, int32_t length) {
    यदि (list != nullptr) {
        ध्रुव UList_गाठ *pointer;
        प्रति (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            यदि (length == (int32_t)uprv_strlen((ध्रुव वर्ण *)pointer->data)) {
                यदि (uprv_memcmp(data, pointer->data, length) == 0) {
                    प्रत्यागम सत्य;
                }
            }
        }
    }
    प्रत्यागम असत्य;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ध्रुव वर्ण *data) {
    यदि (list != nullptr) {
        UList_गाठ *pointer;
        प्रति (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            यदि (uprv_strcmp(data, (ध्रुव वर्ण *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

U_CAPI शून्य *U_EXPORT2 ulist_getNext(UList *list) {
    UList_गाठ *curr = nullptr;
    
    यदि (list == nullptr || list->curr == nullptr) {
        प्रत्यागम nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    प्रत्यागम curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ध्रुव UList *list) {
    यदि (list != nullptr) {
        प्रत्यागम list->size;
    }
    
    प्रत्यागम -1;
}

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list) {
    यदि (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list) {
    UList_गाठ *listHead = nullptr;

    यदि (list != nullptr) {
        listHead = list->head;
        यावत् (listHead != nullptr) {
            UList_गाठ *listPointer = listHead->next;

            यदि (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मूल्ये_iterator(UEnumeration *en) {
    यदि (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_मोजणी_keyword_मूल्ये(UEnumeration *en, UErrorCode *status) {
    यदि (U_FAILURE(*status)) {
        प्रत्यागम -1;
    }
    
    प्रत्यागम ulist_getListSize((UList *)(en->context));
}

U_CAPI ध्रुव वर्ण * U_EXPORT2 ulist_next_keyword_मूल्य(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    ध्रुव वर्ण *s;
    यदि (U_FAILURE(*status)) {
        प्रत्यागम nullptr;
    }

    s = (ध्रुव वर्ण *)ulist_getNext((UList *)(en->context));
    यदि (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    प्रत्यागम s;
}

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मूल्ये_iterator(UEnumeration *en, UErrorCode *status) {
    यदि (U_FAILURE(*status)) {
        प्रत्यागम ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    प्रत्यागम (UList *)(en->context);
}
