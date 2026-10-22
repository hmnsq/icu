// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#അന്തര്ഭാവ "ulist.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uenumimp.h"

തരനിർവചനം ഘടന UList_കണ്ണി UList_കണ്ണി;
ഘടന UList_കണ്ണി {
    ശൂന്യം *data;
    
    UList_കണ്ണി *next;
    UList_കണ്ണി *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ഘടന UList {
    UList_കണ്ണി *curr;
    UList_കണ്ണി *head;
    UList_കണ്ണി *tail;
    
    int32_t size;
};

സ്ഥിതം ശൂന്യം ulist_addFirstItem(UList *list, UList_കണ്ണി *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }
    
    newList = (UList *)uprv_malloc(വലുപ്പം(UList));
    എങ്കിൽ (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    തിരികെ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
സ്ഥിതം ശൂന്യം ulist_addFirstItem(UList *list, UList_കണ്ണി *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

സ്ഥിതം ശൂന്യം ulist_removeItem(UList *list, UList_കണ്ണി *p) {
    എങ്കിൽ (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } അല്ലെങ്കിൽ {
        p->previous->next = p->next;
    }
    എങ്കിൽ (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } അല്ലെങ്കിൽ {
        p->next->previous = p->previous;
    }
    എങ്കിൽ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    എങ്കിൽ (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_addItemEndList(UList *list, സ്ഥിരം ശൂന്യം *data, UBool forceDelete, UErrorCode *status) {
    UList_കണ്ണി *newItem = nullptr;
    
    എങ്കിൽ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        എങ്കിൽ (forceDelete) {
            uprv_free((ശൂന്യം *)data);
        }
        തിരികെ;
    }
    
    newItem = (UList_കണ്ണി *)uprv_malloc(വലുപ്പം(UList_കണ്ണി));
    എങ്കിൽ (newItem == nullptr) {
        എങ്കിൽ (forceDelete) {
            uprv_free((ശൂന്യം *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    newItem->data = (ശൂന്യം *)(data);
    newItem->forceDelete = forceDelete;
    
    എങ്കിൽ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } അല്ലെങ്കിൽ {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_addItemBeginList(UList *list, സ്ഥിരം ശൂന്യം *data, UBool forceDelete, UErrorCode *status) {
    UList_കണ്ണി *newItem = nullptr;
    
    എങ്കിൽ (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        എങ്കിൽ (forceDelete) {
            uprv_free((ശൂന്യം *)data);
        }
        തിരികെ;
    }
    
    newItem = (UList_കണ്ണി *)uprv_malloc(വലുപ്പം(UList_കണ്ണി));
    എങ്കിൽ (newItem == nullptr) {
        എങ്കിൽ (forceDelete) {
            uprv_free((ശൂന്യം *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    newItem->data = (ശൂന്യം *)(data);
    newItem->forceDelete = forceDelete;
    
    എങ്കിൽ (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } അല്ലെങ്കിൽ {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(സ്ഥിരം UList *list, സ്ഥിരം അക്ഷരം *data, int32_t length) {
    എങ്കിൽ (list != nullptr) {
        സ്ഥിരം UList_കണ്ണി *pointer;
        വേണ്ടി (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            എങ്കിൽ (length == (int32_t)uprv_strlen((സ്ഥിരം അക്ഷരം *)pointer->data)) {
                എങ്കിൽ (uprv_memcmp(data, pointer->data, length) == 0) {
                    തിരികെ സത്യം;
                }
            }
        }
    }
    തിരികെ അസത്യം;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, സ്ഥിരം അക്ഷരം *data) {
    എങ്കിൽ (list != nullptr) {
        UList_കണ്ണി *pointer;
        വേണ്ടി (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            എങ്കിൽ (uprv_strcmp(data, (സ്ഥിരം അക്ഷരം *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                തിരികെ സത്യം;
            }
        }
    }
    തിരികെ അസത്യം;
}

U_CAPI ശൂന്യം *U_EXPORT2 ulist_getNext(UList *list) {
    UList_കണ്ണി *curr = nullptr;
    
    എങ്കിൽ (list == nullptr || list->curr == nullptr) {
        തിരികെ nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    തിരികെ curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(സ്ഥിരം UList *list) {
    എങ്കിൽ (list != nullptr) {
        തിരികെ list->size;
    }
    
    തിരികെ -1;
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_resetList(UList *list) {
    എങ്കിൽ (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_deleteList(UList *list) {
    UList_കണ്ണി *listHead = nullptr;

    എങ്കിൽ (list != nullptr) {
        listHead = list->head;
        ആകുമ്പോൾ (listHead != nullptr) {
            UList_കണ്ണി *listPointer = listHead->next;

            എങ്കിൽ (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_close_keyword_മൂല്യങ്ങൾ_iterator(UEnumeration *en) {
    എങ്കിൽ (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_എണ്ണം_keyword_മൂല്യങ്ങൾ(UEnumeration *en, UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ -1;
    }
    
    തിരികെ ulist_getListSize((UList *)(en->context));
}

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2 ulist_next_keyword_മൂല്യം(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    സ്ഥിരം അക്ഷരം *s;
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ nullptr;
    }

    s = (സ്ഥിരം അക്ഷരം *)ulist_getNext((UList *)(en->context));
    എങ്കിൽ (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    തിരികെ s;
}

U_CAPI ശൂന്യം U_EXPORT2 ulist_reset_keyword_മൂല്യങ്ങൾ_iterator(UEnumeration *en, UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) {
        തിരികെ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    തിരികെ (UList *)(en->context);
}
