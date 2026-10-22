// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#અન્તર્ભાવ "ulist.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uenumimp.h"

પ્રકારવ્યાખ્યા સંરચના UList_ગાંઠ UList_ગાંઠ;
સંરચના UList_ગાંઠ {
    શૂન્ય *data;
    
    UList_ગાંઠ *next;
    UList_ગાંઠ *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

સંરચના UList {
    UList_ગાંઠ *curr;
    UList_ગાંઠ *head;
    UList_ગાંઠ *tail;
    
    int32_t size;
};

સ્થિર શૂન્ય ulist_addFirstItem(UList *list, UList_ગાંઠ *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }
    
    newList = (UList *)uprv_malloc(કદ(UList));
    જો (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    
    newList->curr = nullptr;
    newList->head = nullptr;
    newList->tail = nullptr;
    newList->size = 0;
    
    પાછા newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
સ્થિર શૂન્ય ulist_addFirstItem(UList *list, UList_ગાંઠ *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->head = newItem;
    list->tail = newItem;
}

સ્થિર શૂન્ય ulist_removeItem(UList *list, UList_ગાંઠ *p) {
    જો (p->previous == nullptr) {
        // p is the list head.
        list->head = p->next;
    } નહીંતર {
        p->previous->next = p->next;
    }
    જો (p->next == nullptr) {
        // p is the list tail.
        list->tail = p->previous;
    } નહીંતર {
        p->next->previous = p->previous;
    }
    જો (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    જો (p->forceDelete) {
        uprv_free(p->data);
    }
    uprv_free(p);
}

U_CAPI શૂન્ય U_EXPORT2 ulist_addItemEndList(UList *list, અચલ શૂન્ય *data, UBool forceDelete, UErrorCode *status) {
    UList_ગાંઠ *newItem = nullptr;
    
    જો (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        જો (forceDelete) {
            uprv_free((શૂન્ય *)data);
        }
        પાછા;
    }
    
    newItem = (UList_ગાંઠ *)uprv_malloc(કદ(UList_ગાંઠ));
    જો (newItem == nullptr) {
        જો (forceDelete) {
            uprv_free((શૂન્ય *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    newItem->data = (શૂન્ય *)(data);
    newItem->forceDelete = forceDelete;
    
    જો (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } નહીંતર {
        newItem->next = nullptr;
        newItem->previous = list->tail;
        list->tail->next = newItem;
        list->tail = newItem;
    }
    
    list->size++;
}

U_CAPI શૂન્ય U_EXPORT2 ulist_addItemBeginList(UList *list, અચલ શૂન્ય *data, UBool forceDelete, UErrorCode *status) {
    UList_ગાંઠ *newItem = nullptr;
    
    જો (U_FAILURE(*status) || list == nullptr || data == nullptr) {
        જો (forceDelete) {
            uprv_free((શૂન્ય *)data);
        }
        પાછા;
    }
    
    newItem = (UList_ગાંઠ *)uprv_malloc(કદ(UList_ગાંઠ));
    જો (newItem == nullptr) {
        જો (forceDelete) {
            uprv_free((શૂન્ય *)data);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    newItem->data = (શૂન્ય *)(data);
    newItem->forceDelete = forceDelete;
    
    જો (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } નહીંતર {
        newItem->previous = nullptr;
        newItem->next = list->head;
        list->head->previous = newItem;
        list->head = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(અચલ UList *list, અચલ અક્ષર *data, int32_t length) {
    જો (list != nullptr) {
        અચલ UList_ગાંઠ *pointer;
        માટે (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            જો (length == (int32_t)uprv_strlen((અચલ અક્ષર *)pointer->data)) {
                જો (uprv_memcmp(data, pointer->data, length) == 0) {
                    પાછા સાચું;
                }
            }
        }
    }
    પાછા ખોટું;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, અચલ અક્ષર *data) {
    જો (list != nullptr) {
        UList_ગાંઠ *pointer;
        માટે (pointer = list->head; pointer != nullptr; pointer = pointer->next) {
            જો (uprv_strcmp(data, (અચલ અક્ષર *)pointer->data) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                પાછા સાચું;
            }
        }
    }
    પાછા ખોટું;
}

U_CAPI શૂન્ય *U_EXPORT2 ulist_getNext(UList *list) {
    UList_ગાંઠ *curr = nullptr;
    
    જો (list == nullptr || list->curr == nullptr) {
        પાછા nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    પાછા curr->data;
}

U_CAPI int32_t U_EXPORT2 ulist_getListSize(અચલ UList *list) {
    જો (list != nullptr) {
        પાછા list->size;
    }
    
    પાછા -1;
}

U_CAPI શૂન્ય U_EXPORT2 ulist_resetList(UList *list) {
    જો (list != nullptr) {
        list->curr = list->head;
    }
}

U_CAPI શૂન્ય U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ગાંઠ *listHead = nullptr;

    જો (list != nullptr) {
        listHead = list->head;
        જ્યારે (listHead != nullptr) {
            UList_ગાંઠ *listPointer = listHead->next;

            જો (listHead->forceDelete) {
                uprv_free(listHead->data);
            }

            uprv_free(listHead);
            listHead = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI શૂન્ય U_EXPORT2 ulist_close_keyword_કિંમતો_iterator(UEnumeration *en) {
    જો (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ગણતરી_keyword_કિંમતો(UEnumeration *en, UErrorCode *status) {
    જો (U_FAILURE(*status)) {
        પાછા -1;
    }
    
    પાછા ulist_getListSize((UList *)(en->context));
}

U_CAPI અચલ અક્ષર * U_EXPORT2 ulist_next_keyword_કિંમત(UEnumeration *en, int32_t *resultLength, UErrorCode *status) {
    અચલ અક્ષર *s;
    જો (U_FAILURE(*status)) {
        પાછા nullptr;
    }

    s = (અચલ અક્ષર *)ulist_getNext((UList *)(en->context));
    જો (s != nullptr && resultLength != nullptr) {
        *resultLength = static_cast<int32_t>(uprv_strlen(s));
    }
    પાછા s;
}

U_CAPI શૂન્ય U_EXPORT2 ulist_reset_keyword_કિંમતો_iterator(UEnumeration *en, UErrorCode *status) {
    જો (U_FAILURE(*status)) {
        પાછા ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    પાછા (UList *)(en->context);
}
