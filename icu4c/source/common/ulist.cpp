// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "ulist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uenumimp.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UList_ਗੰਢ UList_ਗੰਢ;
ਬਣਤਰ UList_ਗੰਢ {
    ਖਾਲੀ *ਅੰਕੜੇ;
    
    UList_ਗੰਢ *next;
    UList_ਗੰਢ *ਪਿਛਲਾ;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ਬਣਤਰ UList {
    UList_ਗੰਢ *curr;
    UList_ਗੰਢ *ਸਿਰ;
    UList_ਗੰਢ *ਪੂਛ;
    
    int32_t size;
};

ਸਥਿਰ ਖਾਲੀ ulist_ਜੋੜੋ_ਪਹਿਲਾ_ਵਸਤੂ(UList *list, UList_ਗੰਢ *new_ਵਸਤੂ);

U_CAPI UList *U_EXPORT2 ulist_create_ਖਾਲੀ_ਥਾਵਾਂ_List(UError_ਕੋਡ *ਸਥਿਤੀ) {
    UList *newList = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    newList = (UList *)uprv_malloc(ਆਕਾਰ(UList));
    ਜੇ (newList == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    newList->curr = ਖਾਲੀ_ਸੂਚਕ;
    newList->ਸਿਰ = ਖਾਲੀ_ਸੂਚਕ;
    newList->ਪੂਛ = ਖਾਲੀ_ਸੂਚਕ;
    newList->size = 0;
    
    ਵਾਪਸ newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ਸਥਿਰ ਖਾਲੀ ulist_ਜੋੜੋ_ਪਹਿਲਾ_ਵਸਤੂ(UList *list, UList_ਗੰਢ *new_ਵਸਤੂ) {
    new_ਵਸਤੂ->next = ਖਾਲੀ_ਸੂਚਕ;
    new_ਵਸਤੂ->ਪਿਛਲਾ = ਖਾਲੀ_ਸੂਚਕ;
    list->ਸਿਰ = new_ਵਸਤੂ;
    list->ਪੂਛ = new_ਵਸਤੂ;
}

ਸਥਿਰ ਖਾਲੀ ulist_remove_ਵਸਤੂ(UList *list, UList_ਗੰਢ *p) {
    ਜੇ (p->ਪਿਛਲਾ == ਖਾਲੀ_ਸੂਚਕ) {
        // p is the list head.
        list->ਸਿਰ = p->next;
    } ਨਹੀਂਤਾਂ {
        p->ਪਿਛਲਾ->next = p->next;
    }
    ਜੇ (p->next == ਖਾਲੀ_ਸੂਚਕ) {
        // p is the list tail.
        list->ਪੂਛ = p->ਪਿਛਲਾ;
    } ਨਹੀਂਤਾਂ {
        p->next->ਪਿਛਲਾ = p->ਪਿਛਲਾ;
    }
    ਜੇ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ਜੇ (p->forceDelete) {
        uprv_free(p->ਅੰਕੜੇ);
    }
    uprv_free(p);
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_ਜੋੜੋ_ਵਸਤੂ_ਸਿਰਾ_List(UList *list, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UBool forceDelete, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UList_ਗੰਢ *new_ਵਸਤੂ = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ) || list == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (forceDelete) {
            uprv_free((ਖਾਲੀ *)ਅੰਕੜੇ);
        }
        ਵਾਪਸ;
    }
    
    new_ਵਸਤੂ = (UList_ਗੰਢ *)uprv_malloc(ਆਕਾਰ(UList_ਗੰਢ));
    ਜੇ (new_ਵਸਤੂ == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (forceDelete) {
            uprv_free((ਖਾਲੀ *)ਅੰਕੜੇ);
        }
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    new_ਵਸਤੂ->ਅੰਕੜੇ = (ਖਾਲੀ *)(ਅੰਕੜੇ);
    new_ਵਸਤੂ->forceDelete = forceDelete;
    
    ਜੇ (list->size == 0) {
        ulist_ਜੋੜੋ_ਪਹਿਲਾ_ਵਸਤੂ(list, new_ਵਸਤੂ);
    } ਨਹੀਂਤਾਂ {
        new_ਵਸਤੂ->next = ਖਾਲੀ_ਸੂਚਕ;
        new_ਵਸਤੂ->ਪਿਛਲਾ = list->ਪੂਛ;
        list->ਪੂਛ->next = new_ਵਸਤੂ;
        list->ਪੂਛ = new_ਵਸਤੂ;
    }
    
    list->size++;
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_ਜੋੜੋ_ਵਸਤੂ_Begin_List(UList *list, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UBool forceDelete, UError_ਕੋਡ *ਸਥਿਤੀ) {
    UList_ਗੰਢ *new_ਵਸਤੂ = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ) || list == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (forceDelete) {
            uprv_free((ਖਾਲੀ *)ਅੰਕੜੇ);
        }
        ਵਾਪਸ;
    }
    
    new_ਵਸਤੂ = (UList_ਗੰਢ *)uprv_malloc(ਆਕਾਰ(UList_ਗੰਢ));
    ਜੇ (new_ਵਸਤੂ == ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ (forceDelete) {
            uprv_free((ਖਾਲੀ *)ਅੰਕੜੇ);
        }
        *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    new_ਵਸਤੂ->ਅੰਕੜੇ = (ਖਾਲੀ *)(ਅੰਕੜੇ);
    new_ਵਸਤੂ->forceDelete = forceDelete;
    
    ਜੇ (list->size == 0) {
        ulist_ਜੋੜੋ_ਪਹਿਲਾ_ਵਸਤੂ(list, new_ਵਸਤੂ);
    } ਨਹੀਂਤਾਂ {
        new_ਵਸਤੂ->ਪਿਛਲਾ = ਖਾਲੀ_ਸੂਚਕ;
        new_ਵਸਤੂ->next = list->ਸਿਰ;
        list->ਸਿਰ->ਪਿਛਲਾ = new_ਵਸਤੂ;
        list->ਸਿਰ = new_ਵਸਤੂ;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_ਸ਼ਾਮਲ_ਹੈ_String(ਅਚਲ UList *list, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, int32_t length) {
    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        ਅਚਲ UList_ਗੰਢ *ਸੂਚਕ;
        ਲਈ (ਸੂਚਕ = list->ਸਿਰ; ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ; ਸੂਚਕ = ਸੂਚਕ->next) {
            ਜੇ (length == (int32_t)uprv_strlen((ਅਚਲ ਅੱਖਰ *)ਸੂਚਕ->ਅੰਕੜੇ)) {
                ਜੇ (uprv_memcmp(ਅੰਕੜੇ, ਸੂਚਕ->ਅੰਕੜੇ, length) == 0) {
                    ਵਾਪਸ ਸੱਚ;
                }
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ) {
    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        UList_ਗੰਢ *ਸੂਚਕ;
        ਲਈ (ਸੂਚਕ = list->ਸਿਰ; ਸੂਚਕ != ਖਾਲੀ_ਸੂਚਕ; ਸੂਚਕ = ਸੂਚਕ->next) {
            ਜੇ (uprv_strcmp(ਅੰਕੜੇ, (ਅਚਲ ਅੱਖਰ *)ਸੂਚਕ->ਅੰਕੜੇ) == 0) {
                ulist_remove_ਵਸਤੂ(list, ਸੂਚਕ);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ਵਾਪਸ ਸੱਚ;
            }
        }
    }
    ਵਾਪਸ ਝੂਠ;
}

U_CAPI ਖਾਲੀ *U_EXPORT2 ulist_get_ਅਗਲਾ(UList *list) {
    UList_ਗੰਢ *curr = ਖਾਲੀ_ਸੂਚਕ;
    
    ਜੇ (list == ਖਾਲੀ_ਸੂਚਕ || list->curr == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ਵਾਪਸ curr->ਅੰਕੜੇ;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_ਮਾਪ(ਅਚਲ UList *list) {
    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ list->size;
    }
    
    ਵਾਪਸ -1;
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_resetList(UList *list) {
    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        list->curr = list->ਸਿਰ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ਗੰਢ *list_ਸਿਰ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (list != ਖਾਲੀ_ਸੂਚਕ) {
        list_ਸਿਰ = list->ਸਿਰ;
        ਜਦੋਂ (list_ਸਿਰ != ਖਾਲੀ_ਸੂਚਕ) {
            UList_ਗੰਢ *list_ਸੂਚਕ = list_ਸਿਰ->next;

            ਜੇ (list_ਸਿਰ->forceDelete) {
                uprv_free(list_ਸਿਰ->ਅੰਕੜੇ);
            }

            uprv_free(list_ਸਿਰ);
            list_ਸਿਰ = list_ਸੂਚਕ;
        }
        uprv_free(list);
        list = ਖਾਲੀ_ਸੂਚਕ;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_close_keyword_ਮੁੱਲਾਂ_iterator(UEnumeration *en) {
    ਜੇ (en != ਖਾਲੀ_ਸੂਚਕ) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ਗਿਣਤੀ_keyword_ਮੁੱਲਾਂ(UEnumeration *en, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ -1;
    }
    
    ਵਾਪਸ ulist_get_List_ਮਾਪ((UList *)(en->context));
}

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2 ulist_ਅਗਲਾ_keyword_ਮੁੱਲ(UEnumeration *en, int32_t *ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਅਚਲ ਅੱਖਰ *s;
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    s = (ਅਚਲ ਅੱਖਰ *)ulist_get_ਅਗਲਾ((UList *)(en->context));
    ਜੇ (s != ਖਾਲੀ_ਸੂਚਕ && ਨਤੀਜਾ_ਲੰਮਾਈ != ਖਾਲੀ_ਸੂਚਕ) {
        *ਨਤੀਜਾ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(s));
    }
    ਵਾਪਸ s;
}

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_reset_keyword_ਮੁੱਲਾਂ_iterator(UEnumeration *en, UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) {
        ਵਾਪਸ ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ਵਾਪਸ (UList *)(en->context);
}
