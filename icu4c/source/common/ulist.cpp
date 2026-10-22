// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#समावेश "ulist.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "uenumimp.h"

प्रकार काष्ठा UList_गाँठ UList_गाँठ;
काष्ठा UList_गाँठ {
    व्योम *तथ्य;
    
    UList_गाँठ *next;
    UList_गाँठ *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

काष्ठा UList {
    UList_गाँठ *curr;
    UList_गाँठ *शीर्ष;
    UList_गाँठ *पुच्छ;
    
    int32_t size;
};

अटल व्योम ulist_addFirstItem(UList *list, UList_गाँठ *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }
    
    newList = (UList *)uprv_malloc(माप(UList));
    अगर (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    
    newList->curr = nullptr;
    newList->शीर्ष = nullptr;
    newList->पुच्छ = nullptr;
    newList->size = 0;
    
    वापस newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
अटल व्योम ulist_addFirstItem(UList *list, UList_गाँठ *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->शीर्ष = newItem;
    list->पुच्छ = newItem;
}

अटल व्योम ulist_removeItem(UList *list, UList_गाँठ *p) {
    अगर (p->previous == nullptr) {
        // p is the list head.
        list->शीर्ष = p->next;
    } अन्यथा {
        p->previous->next = p->next;
    }
    अगर (p->next == nullptr) {
        // p is the list tail.
        list->पुच्छ = p->previous;
    } अन्यथा {
        p->next->previous = p->previous;
    }
    अगर (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    अगर (p->forceDelete) {
        uprv_free(p->तथ्य);
    }
    uprv_free(p);
}

U_CAPI व्योम U_EXPORT2 ulist_addItemEndList(UList *list, स्थिर व्योम *तथ्य, UBool forceDelete, UErrorCode *status) {
    UList_गाँठ *newItem = nullptr;
    
    अगर (U_FAILURE(*status) || list == nullptr || तथ्य == nullptr) {
        अगर (forceDelete) {
            uprv_free((व्योम *)तथ्य);
        }
        वापस;
    }
    
    newItem = (UList_गाँठ *)uprv_malloc(माप(UList_गाँठ));
    अगर (newItem == nullptr) {
        अगर (forceDelete) {
            uprv_free((व्योम *)तथ्य);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    newItem->तथ्य = (व्योम *)(तथ्य);
    newItem->forceDelete = forceDelete;
    
    अगर (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->next = nullptr;
        newItem->previous = list->पुच्छ;
        list->पुच्छ->next = newItem;
        list->पुच्छ = newItem;
    }
    
    list->size++;
}

U_CAPI व्योम U_EXPORT2 ulist_addItemBeginList(UList *list, स्थिर व्योम *तथ्य, UBool forceDelete, UErrorCode *status) {
    UList_गाँठ *newItem = nullptr;
    
    अगर (U_FAILURE(*status) || list == nullptr || तथ्य == nullptr) {
        अगर (forceDelete) {
            uprv_free((व्योम *)तथ्य);
        }
        वापस;
    }
    
    newItem = (UList_गाँठ *)uprv_malloc(माप(UList_गाँठ));
    अगर (newItem == nullptr) {
        अगर (forceDelete) {
            uprv_free((व्योम *)तथ्य);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    newItem->तथ्य = (व्योम *)(तथ्य);
    newItem->forceDelete = forceDelete;
    
    अगर (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } अन्यथा {
        newItem->previous = nullptr;
        newItem->next = list->शीर्ष;
        list->शीर्ष->previous = newItem;
        list->शीर्ष = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(स्थिर UList *list, स्थिर अक्षर *तथ्य, int32_t length) {
    अगर (list != nullptr) {
        स्थिर UList_गाँठ *pointer;
        क्रम (pointer = list->शीर्ष; pointer != nullptr; pointer = pointer->next) {
            अगर (length == (int32_t)uprv_strlen((स्थिर अक्षर *)pointer->तथ्य)) {
                अगर (uprv_memcmp(तथ्य, pointer->तथ्य, length) == 0) {
                    वापस true;
                }
            }
        }
    }
    वापस false;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, स्थिर अक्षर *तथ्य) {
    अगर (list != nullptr) {
        UList_गाँठ *pointer;
        क्रम (pointer = list->शीर्ष; pointer != nullptr; pointer = pointer->next) {
            अगर (uprv_strcmp(तथ्य, (स्थिर अक्षर *)pointer->तथ्य) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                वापस true;
            }
        }
    }
    वापस false;
}

U_CAPI व्योम *U_EXPORT2 ulist_get_अग्र(UList *list) {
    UList_गाँठ *curr = nullptr;
    
    अगर (list == nullptr || list->curr == nullptr) {
        वापस nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    वापस curr->तथ्य;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_आकार(स्थिर UList *list) {
    अगर (list != nullptr) {
        वापस list->size;
    }
    
    वापस -1;
}

U_CAPI व्योम U_EXPORT2 ulist_resetList(UList *list) {
    अगर (list != nullptr) {
        list->curr = list->शीर्ष;
    }
}

U_CAPI व्योम U_EXPORT2 ulist_deleteList(UList *list) {
    UList_गाँठ *list_शीर्ष = nullptr;

    अगर (list != nullptr) {
        list_शीर्ष = list->शीर्ष;
        जबतक (list_शीर्ष != nullptr) {
            UList_गाँठ *listPointer = list_शीर्ष->next;

            अगर (list_शीर्ष->forceDelete) {
                uprv_free(list_शीर्ष->तथ्य);
            }

            uprv_free(list_शीर्ष);
            list_शीर्ष = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI व्योम U_EXPORT2 ulist_close_keyword_मूल्यावली_iterator(UEnumeration *en) {
    अगर (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_गिनती_keyword_मूल्यावली(UEnumeration *en, UErrorCode *status) {
    अगर (U_FAILURE(*status)) {
        वापस -1;
    }
    
    वापस ulist_get_List_आकार((UList *)(en->context));
}

U_CAPI स्थिर अक्षर * U_EXPORT2 ulist_अग्र_keyword_मूल्य(UEnumeration *en, int32_t *परिणाम_लंबाई, UErrorCode *status) {
    स्थिर अक्षर *s;
    अगर (U_FAILURE(*status)) {
        वापस nullptr;
    }

    s = (स्थिर अक्षर *)ulist_get_अग्र((UList *)(en->context));
    अगर (s != nullptr && परिणाम_लंबाई != nullptr) {
        *परिणाम_लंबाई = static_cast<int32_t>(uprv_strlen(s));
    }
    वापस s;
}

U_CAPI व्योम U_EXPORT2 ulist_reset_keyword_मूल्यावली_iterator(UEnumeration *en, UErrorCode *status) {
    अगर (U_FAILURE(*status)) {
        वापस ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    वापस (UList *)(en->context);
}
