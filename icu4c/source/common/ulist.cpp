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

प्रकारनिर्देश संरचना UList_गाँठो UList_गाँठो;
संरचना UList_गाँठो {
    शून्य *data;
    
    UList_गाँठो *next;
    UList_गाँठो *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

संरचना UList {
    UList_गाँठो *curr;
    UList_गाँठो *head;
    UList_गाँठो *tail;
    
    int32_t size;
};

स्थिर शून्य ulist_addFirstItem(UList *list, UList_गाँठो *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }
    
    newList = (UList *)uprv_malloc(परिमाण(UList));
    यदि (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    फर्काऊ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
स्थिर शून्य ulist_addFirstItem(UList *list, UList_गाँठो *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

स्थिर शून्य ulist_removeItem(UList *list, UList_गाँठो *p) {
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

U_CAPI शून्य U_EXPORT2 ulist_addItemEndList(UList *list, अचर शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाँठो *newItem = nullptr;
    
    यदि (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        फर्काऊ;
    }
    
    newItem = (UList_गाँठो *)uprv_malloc(परिमाण(UList_गाँठो));
    यदि (newItem == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
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

U_CAPI शून्य U_EXPORT2 ulist_addItemBeginList(UList *list, अचर शून्य *data, UBool forceDelete, UErrorCode *status) {
    UList_गाँठो *newItem = nullptr;
    
    यदि (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        फर्काऊ;
    }
    
    newItem = (UList_गाँठो *)uprv_malloc(परिमाण(UList_गाँठो));
    यदि (newItem == nullptr) {
        यदि (forceDelete) {
            uprv_free((शून्य *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ;
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

U_CAPI UBool U_EXPORT2 ulist_containsString(अचर UList *list, अचर वर्ण *data, int32_t length) {
    यदि (list != nullptr) {
        अचर UList_गाँठो *pointer;
        लागि (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            यदि (length == (int32_t)uprv_strlen((अचर वर्ण *)pointer->data)) {
                यदि (uprv_memcmp(data, pointer->data, length) == 0) {
                    फर्काऊ सत्य;
                }
            }
        }
    }
    फर्काऊ असत्य;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, अचर वर्ण *data) {
    यदि (list != nullptr) {
        UList_गाँठो *pointer;
        लागि (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            यदि (uprv_strcmp(data, (अचर वर्ण *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                फर्काऊ सत्य;
            }
        }
    }
    फर्काऊ असत्य;
}

U_CAPI शून्य *U_EXPORT2 ulist_getNext(UList *list) {
    UList_गाँठो *curr = nullptr;
    
    यदि (list == nullptr || list->curr == nullptr) {
        फर्काऊ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    फर्काऊ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(अचर UList *list) {
    यदि (list != nullptr) {
        फर्काऊ list->size;
    }
    
    फर्काऊ -1;
}

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list) {
    यदि (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list) {
    UList_गाँठो *listHead = nullptr;

    यदि (list != nullptr) {
        listHead = list->head;
        जबसम्म (listHead != nullptr) {
            UList_गाँठो *listPointer = listHead->next;

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

U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मानहरू_iterator(UEnumeration *en) {
    यदि (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_गन्ती_keyword_मानहरू(UEnumeration *en, UErrorCode *status) {
    यदि (U_FAILURE(*status)) {
        फर्काऊ -1;
    }
    
    फर्काऊ ulist_getListSize((UList *)(en->context));
}

U_CAPI अचर वर्ण * U_EXPORT2 ulist_next_keyword_मान(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    अचर वर्ण *s;
    यदि (U_FAILURE(*status)) {
        फर्काऊ nullptr;
    }

    s = (अचर वर्ण *)ulist_getNext((UList *)(en->context));
    यदि (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    फर्काऊ s;
}

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मानहरू_iterator(UEnumeration *en, UErrorCode *status) {
    यदि (U_FAILURE(*status)) {
        फर्काऊ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    फर्काऊ (UList *)(en->context);
}
