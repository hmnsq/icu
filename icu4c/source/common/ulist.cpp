// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "ulist.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UList_ಗಂಟು UList_ಗಂಟು;
ರಚನೆ UList_ಗಂಟು {
    ಶೂನ್ಯ *ದತ್ತಾಂಶ;
    
    UList_ಗಂಟು *next;
    UList_ಗಂಟು *ಹಿಂದಿನ;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ರಚನೆ UList {
    UList_ಗಂಟು *curr;
    UList_ಗಂಟು *ತಲೆ;
    UList_ಗಂಟು *ಬಾಲ;
    
    int32_t size;
};

ಸ್ಥಿರ ಶೂನ್ಯ ulist_ಕೂಡಿಸು_ಮೊದಲ_ಅಂಶ(UList *list, UList_ಗಂಟು *new_ಅಂಶ);

U_CAPI UList *U_EXPORT2 ulist_create_ಖಾಲಿಗಳು_List(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UList *newList = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    newList = (UList *)uprv_malloc(ಗಾತ್ರ(UList));
    ಆದರೆ (newList == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    newList->curr = ಶೂನ್ಯಸೂಚಕ;
    newList->ತಲೆ = ಶೂನ್ಯಸೂಚಕ;
    newList->ಬಾಲ = ಶೂನ್ಯಸೂಚಕ;
    newList->size = 0;
    
    ಹಿಂತಿರುಗಿ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ulist_ಕೂಡಿಸು_ಮೊದಲ_ಅಂಶ(UList *list, UList_ಗಂಟು *new_ಅಂಶ) {
    new_ಅಂಶ->next = ಶೂನ್ಯಸೂಚಕ;
    new_ಅಂಶ->ಹಿಂದಿನ = ಶೂನ್ಯಸೂಚಕ;
    list->ತಲೆ = new_ಅಂಶ;
    list->ಬಾಲ = new_ಅಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ulist_remove_ಅಂಶ(UList *list, UList_ಗಂಟು *p) {
    ಆದರೆ (p->ಹಿಂದಿನ == ಶೂನ್ಯಸೂಚಕ) {
        // p is the list head.
        list->ತಲೆ = p->next;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p->ಹಿಂದಿನ->next = p->next;
    }
    ಆದರೆ (p->next == ಶೂನ್ಯಸೂಚಕ) {
        // p is the list tail.
        list->ಬಾಲ = p->ಹಿಂದಿನ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        p->next->ಹಿಂದಿನ = p->ಹಿಂದಿನ;
    }
    ಆದರೆ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ಆದರೆ (p->forceDelete) {
        uprv_free(p->ದತ್ತಾಂಶ);
    }
    uprv_free(p);
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_ಕೂಡಿಸು_ಅಂಶ_ಕೊನೆ_List(UList *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UBool forceDelete, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UList_ಗಂಟು *new_ಅಂಶ = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2) || list == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)ದತ್ತಾಂಶ);
        }
        ಹಿಂತಿರುಗಿ;
    }
    
    new_ಅಂಶ = (UList_ಗಂಟು *)uprv_malloc(ಗಾತ್ರ(UList_ಗಂಟು));
    ಆದರೆ (new_ಅಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)ದತ್ತಾಂಶ);
        }
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    new_ಅಂಶ->ದತ್ತಾಂಶ = (ಶೂನ್ಯ *)(ದತ್ತಾಂಶ);
    new_ಅಂಶ->forceDelete = forceDelete;
    
    ಆದರೆ (list->size == 0) {
        ulist_ಕೂಡಿಸು_ಮೊದಲ_ಅಂಶ(list, new_ಅಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        new_ಅಂಶ->next = ಶೂನ್ಯಸೂಚಕ;
        new_ಅಂಶ->ಹಿಂದಿನ = list->ಬಾಲ;
        list->ಬಾಲ->next = new_ಅಂಶ;
        list->ಬಾಲ = new_ಅಂಶ;
    }
    
    list->size++;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_ಕೂಡಿಸು_ಅಂಶ_Begin_List(UList *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UBool forceDelete, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    UList_ಗಂಟು *new_ಅಂಶ = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2) || list == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)ದತ್ತಾಂಶ);
        }
        ಹಿಂತಿರುಗಿ;
    }
    
    new_ಅಂಶ = (UList_ಗಂಟು *)uprv_malloc(ಗಾತ್ರ(UList_ಗಂಟು));
    ಆದರೆ (new_ಅಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ (forceDelete) {
            uprv_free((ಶೂನ್ಯ *)ದತ್ತಾಂಶ);
        }
        *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    new_ಅಂಶ->ದತ್ತಾಂಶ = (ಶೂನ್ಯ *)(ದತ್ತಾಂಶ);
    new_ಅಂಶ->forceDelete = forceDelete;
    
    ಆದರೆ (list->size == 0) {
        ulist_ಕೂಡಿಸು_ಮೊದಲ_ಅಂಶ(list, new_ಅಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        new_ಅಂಶ->ಹಿಂದಿನ = ಶೂನ್ಯಸೂಚಕ;
        new_ಅಂಶ->next = list->ತಲೆ;
        list->ತಲೆ->ಹಿಂದಿನ = new_ಅಂಶ;
        list->ತಲೆ = new_ಅಂಶ;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_ಒಳಗೊಂಡಿದೆ_String(ಸ್ಥಿರಾಂಕ UList *list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, int32_t length) {
    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ UList_ಗಂಟು *ಸೂಚಕ;
        ಗಾಗಿ (ಸೂಚಕ = list->ತಲೆ; ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ; ಸೂಚಕ = ಸೂಚಕ->next) {
            ಆದರೆ (length == (int32_t)uprv_strlen((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಸೂಚಕ->ದತ್ತಾಂಶ)) {
                ಆದರೆ (uprv_memcmp(ದತ್ತಾಂಶ, ಸೂಚಕ->ದತ್ತಾಂಶ, length) == 0) {
                    ಹಿಂತಿರುಗಿ ನಿಜ;
                }
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ) {
    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        UList_ಗಂಟು *ಸೂಚಕ;
        ಗಾಗಿ (ಸೂಚಕ = list->ತಲೆ; ಸೂಚಕ != ಶೂನ್ಯಸೂಚಕ; ಸೂಚಕ = ಸೂಚಕ->next) {
            ಆದರೆ (uprv_strcmp(ದತ್ತಾಂಶ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಸೂಚಕ->ದತ್ತಾಂಶ) == 0) {
                ulist_remove_ಅಂಶ(list, ಸೂಚಕ);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI ಶೂನ್ಯ *U_EXPORT2 ulist_get_ಮುಂದಿನ(UList *list) {
    UList_ಗಂಟು *curr = ಶೂನ್ಯಸೂಚಕ;
    
    ಆದರೆ (list == ಶೂನ್ಯಸೂಚಕ || list->curr == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ಹಿಂತಿರುಗಿ curr->ದತ್ತಾಂಶ;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UList *list) {
    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ list->size;
    }
    
    ಹಿಂತಿರುಗಿ -1;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_resetList(UList *list) {
    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        list->curr = list->ತಲೆ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ಗಂಟು *list_ತಲೆ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (list != ಶೂನ್ಯಸೂಚಕ) {
        list_ತಲೆ = list->ತಲೆ;
        ಆಗ (list_ತಲೆ != ಶೂನ್ಯಸೂಚಕ) {
            UList_ಗಂಟು *list_ಸೂಚಕ = list_ತಲೆ->next;

            ಆದರೆ (list_ತಲೆ->forceDelete) {
                uprv_free(list_ತಲೆ->ದತ್ತಾಂಶ);
            }

            uprv_free(list_ತಲೆ);
            list_ತಲೆ = list_ಸೂಚಕ;
        }
        uprv_free(list);
        list = ಶೂನ್ಯಸೂಚಕ;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en) {
    ಆದರೆ (en != ಶೂನ್ಯಸೂಚಕ) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು(UEnumeration *en, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ -1;
    }
    
    ಹಿಂತಿರುಗಿ ulist_get_List_ಗಾತ್ರ((UList *)(en->context));
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2 ulist_ಮುಂದಿನ_keyword_ಮೌಲ್ಯ(UEnumeration *en, int32_t *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ulist_get_ಮುಂದಿನ((UList *)(en->context));
    ಆದರೆ (s != ಶೂನ್ಯಸೂಚಕ && ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ != ಶೂನ್ಯಸೂಚಕ) {
        *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(s));
    }
    ಹಿಂತಿರುಗಿ s;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ಹಿಂತಿರುಗಿ (UList *)(en->context);
}
