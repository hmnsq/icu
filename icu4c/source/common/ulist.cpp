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
    शून्य *दत्त;
    
    UList_ग्रन्थिः *next;
    UList_ग्रन्थिः *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

संरचना UList {
    UList_ग्रन्थिः *curr;
    UList_ग्रन्थिः *शीर्ष;
    UList_ग्रन्थिः *पुच्छ;
    
    int32_t size;
};

स्थिर शून्य ulist_सङ्कलय_First_Item(UList *list, UList_ग्रन्थिः *newItem);

U_CAPI UList *U_EXPORT2 ulist_create_रिक्तानि_List(UError_संहिता *स्थितिः) {
    UList *newList = शून्यसूचकः;
    
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    
    newList = (UList *)uprv_malloc(परिमाण(UList));
    यदि (newList == शून्यसूचकः) {
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    
    newList->curr = शून्यसूचकः;
    newList->शीर्ष = शून्यसूचकः;
    newList->पुच्छ = शून्यसूचकः;
    newList->size = 0;
    
    प्रत्यागम newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
स्थिर शून्य ulist_सङ्कलय_First_Item(UList *list, UList_ग्रन्थिः *newItem) {
    newItem->next = शून्यसूचकः;
    newItem->previous = शून्यसूचकः;
    list->शीर्ष = newItem;
    list->पुच्छ = newItem;
}

स्थिर शून्य ulist_removeItem(UList *list, UList_ग्रन्थिः *p) {
    यदि (p->previous == शून्यसूचकः) {
        // p is the list head.
        list->शीर्ष = p->next;
    } अन्यथा {
        p->previous->next = p->next;
    }
    यदि (p->next == शून्यसूचकः) {
        // p is the list tail.
        list->पुच्छ = p->previous;
    } अन्यथा {
        p->next->previous = p->previous;
    }
    यदि (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    यदि (p->forceDelete) {
        uprv_free(p->दत्त);
    }
    uprv_free(p);
}

U_CAPI शून्य U_EXPORT2 ulist_सङ्कलय_Item_अन्तः_List(UList *list, ध्रुव शून्य *दत्त, UBool forceDelete, UError_संहिता *स्थितिः) {
    UList_ग्रन्थिः *newItem = शून्यसूचकः;
    
    यदि (U_FAILURE(*स्थितिः) || list == शून्यसूचकः || दत्त == शून्यसूचकः) {
        यदि (forceDelete) {
            uprv_free((शून्य *)दत्त);
        }
        प्रत्यागम;
    }
    
    newItem = (UList_ग्रन्थिः *)uprv_malloc(परिमाण(UList_ग्रन्थिः));
    यदि (newItem == शून्यसूचकः) {
        यदि (forceDelete) {
            uprv_free((शून्य *)दत्त);
        }
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    newItem->दत्त = (शून्य *)(दत्त);
    newItem->forceDelete = forceDelete;
    
    यदि (list->size == 0) {
        ulist_सङ्कलय_First_Item(list, newItem);
    } अन्यथा {
        newItem->next = शून्यसूचकः;
        newItem->previous = list->पुच्छ;
        list->पुच्छ->next = newItem;
        list->पुच्छ = newItem;
    }
    
    list->size++;
}

U_CAPI शून्य U_EXPORT2 ulist_सङ्कलय_Item_Begin_List(UList *list, ध्रुव शून्य *दत्त, UBool forceDelete, UError_संहिता *स्थितिः) {
    UList_ग्रन्थिः *newItem = शून्यसूचकः;
    
    यदि (U_FAILURE(*स्थितिः) || list == शून्यसूचकः || दत्त == शून्यसूचकः) {
        यदि (forceDelete) {
            uprv_free((शून्य *)दत्त);
        }
        प्रत्यागम;
    }
    
    newItem = (UList_ग्रन्थिः *)uprv_malloc(परिमाण(UList_ग्रन्थिः));
    यदि (newItem == शून्यसूचकः) {
        यदि (forceDelete) {
            uprv_free((शून्य *)दत्त);
        }
        *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    newItem->दत्त = (शून्य *)(दत्त);
    newItem->forceDelete = forceDelete;
    
    यदि (list->size == 0) {
        ulist_सङ्कलय_First_Item(list, newItem);
    } अन्यथा {
        newItem->previous = शून्यसूचकः;
        newItem->next = list->शीर्ष;
        list->शीर्ष->previous = newItem;
        list->शीर्ष = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_अन्तर्भवति_String(ध्रुव UList *list, ध्रुव वर्ण *दत्त, int32_t length) {
    यदि (list != शून्यसूचकः) {
        ध्रुव UList_ग्रन्थिः *pointer;
        प्रति (pointer = list->शीर्ष; pointer != शून्यसूचकः; pointer = pointer->next) {
            यदि (length == (int32_t)uprv_strlen((ध्रुव वर्ण *)pointer->दत्त)) {
                यदि (uprv_memcmp(दत्त, pointer->दत्त, length) == 0) {
                    प्रत्यागम सत्य;
                }
            }
        }
    }
    प्रत्यागम असत्य;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ध्रुव वर्ण *दत्त) {
    यदि (list != शून्यसूचकः) {
        UList_ग्रन्थिः *pointer;
        प्रति (pointer = list->शीर्ष; pointer != शून्यसूचकः; pointer = pointer->next) {
            यदि (uprv_strcmp(दत्त, (ध्रुव वर्ण *)pointer->दत्त) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                प्रत्यागम सत्य;
            }
        }
    }
    प्रत्यागम असत्य;
}

U_CAPI शून्य *U_EXPORT2 ulist_get_अग्र(UList *list) {
    UList_ग्रन्थिः *curr = शून्यसूचकः;
    
    यदि (list == शून्यसूचकः || list->curr == शून्यसूचकः) {
        प्रत्यागम शून्यसूचकः;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    प्रत्यागम curr->दत्त;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_आकार(ध्रुव UList *list) {
    यदि (list != शून्यसूचकः) {
        प्रत्यागम list->size;
    }
    
    प्रत्यागम -1;
}

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list) {
    यदि (list != शून्यसूचकः) {
        list->curr = list->शीर्ष;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ग्रन्थिः *list_शीर्ष = शून्यसूचकः;

    यदि (list != शून्यसूचकः) {
        list_शीर्ष = list->शीर्ष;
        यावत् (list_शीर्ष != शून्यसूचकः) {
            UList_ग्रन्थिः *listPointer = list_शीर्ष->next;

            यदि (list_शीर्ष->forceDelete) {
                uprv_free(list_शीर्ष->दत्त);
            }

            uprv_free(list_शीर्ष);
            list_शीर्ष = listPointer;
        }
        uprv_free(list);
        list = शून्यसूचकः;
    }
}

U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मानानि_iterator(UEnumeration *en) {
    यदि (en != शून्यसूचकः) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_गणनम्_keyword_मानानि(UEnumeration *en, UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम -1;
    }
    
    प्रत्यागम ulist_get_List_आकार((UList *)(en->context));
}

U_CAPI ध्रुव वर्ण * U_EXPORT2 ulist_अग्र_keyword_मानम्(UEnumeration *en, int32_t *परिणाम_दीर्घता, UError_संहिता *स्थितिः) {
    ध्रुव वर्ण *s;
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    s = (ध्रुव वर्ण *)ulist_get_अग्र((UList *)(en->context));
    यदि (s != शून्यसूचकः && परिणाम_दीर्घता != शून्यसूचकः) {
        *परिणाम_दीर्घता = static_cast<int32_t>(uprv_strlen(s));
    }
    प्रत्यागम s;
}

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मानानि_iterator(UEnumeration *en, UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) {
        प्रत्यागम ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    प्रत्यागम (UList *)(en->context);
}
