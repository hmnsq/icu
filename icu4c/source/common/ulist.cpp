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

प्रकारव्याख्या संरचना UList_गाठ UList_गाठ;
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
    
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }
    
    newList = (UList *)uprv_malloc(आकार(UList));
    जर (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    परत newList;
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
    जर (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } अन्यथा {
        p->previous->next = p->next;
    }
    जर (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } अन्यथा {
        p->next->previous = p->previous;
    }
    जर (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    जर (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI शून्य U_EXPORT2 ulist_addItemEndList(UList *list, स्थिरांक शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाठ *newItem = nullptr;
    
    जर (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        जर (forceDelete) {
            uprv_free((शून्य *)data);
        }
        परत;
    }
    
    newItem = (UList_गाठ *)uprv_malloc(आकार(UList_गाठ));
    जर (newItem == nullptr) {
        जर (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    जर (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI शून्य U_EXPORT2 ulist_addItemBeginList(UList *list, स्थिरांक शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाठ *newItem = nullptr;
    
    जर (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        जर (forceDelete) {
            uprv_free((शून्य *)data);
        }
        परत;
    }
    
    newItem = (UList_गाठ *)uprv_malloc(आकार(UList_गाठ));
    जर (newItem == nullptr) {
        जर (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    newItem->data = (शून्य *)(data);
    newItem->forceDelete = forceDelete;
    
    जर (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(स्थिरांक UList *list, स्थिरांक अक्षर *data, int32_t length) {
    जर (list != nullptr) {
        स्थिरांक UList_गाठ *pointer;
        साठी (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            जर (length == (int32_t)uprv_strlen((स्थिरांक अक्षर *)pointer->data)) {
                जर (uprv_memcmp(data, pointer->data, length) == 0) {
                    परत सत्य;
                }
            }
        }
    }
    परत असत्य;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, स्थिरांक अक्षर *data) {
    जर (list != nullptr) {
        UList_गाठ *pointer;
        साठी (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            जर (uprv_strcmp(data, (स्थिरांक अक्षर *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                परत सत्य;
            }
        }
    }
    परत असत्य;
}

U_CAPI शून्य *U_EXPORT2 ulist_getNext(UList *list) {
    UList_गाठ *curr = nullptr;
    
    जर (list == nullptr || list->curr == nullptr) {
        परत nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    परत curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(स्थिरांक UList *list) {
    जर (list != nullptr) {
        परत list->size;
    }
    
    परत -1;
}

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list) {
    जर (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list) {
    UList_गाठ *listHead = nullptr;

    जर (list != nullptr) {
        listHead = list->head;
        पर्यंत (listHead != nullptr) {
            UList_गाठ *listPointer = listHead->next;

            जर (listHead->forceDelete) {
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
    जर (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_मोजणी_keyword_मूल्ये(UEnumeration *en, UErrorCode *status) {
    जर (U_FAILURE(*status)) {
        परत -1;
    }
    
    परत ulist_getListSize((UList *)(en->context));
}

U_CAPI स्थिरांक अक्षर * U_EXPORT2 ulist_next_keyword_मूल्य(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    स्थिरांक अक्षर *s;
    जर (U_FAILURE(*status)) {
        परत nullptr;
    }

    s = (स्थिरांक अक्षर *)ulist_getNext((UList *)(en->context));
    जर (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    परत s;
}

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मूल्ये_iterator(UEnumeration *en, UErrorCode *status) {
    जर (U_FAILURE(*status)) {
        परत ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    परत (UList *)(en->context);
}
