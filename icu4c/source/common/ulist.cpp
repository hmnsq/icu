// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ضمن "ulist.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uenumimp.h"

تعريفنوع بنية UList_غوټه UList_غوټه;
بنية UList_غوټه {
    فراغ *معلومات;
    
    UList_غوټه *next;
    UList_غوټه *پخوانی;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UList_غوټه *curr;
    UList_غوټه *سر;
    UList_غوټه *لکۍ;
    
    int32_t size;
};

ساكن فراغ ulist_جمع_کړه_لومړی_توکی(UList *list, UList_غوټه *new_توکی);

U_CAPI UList *U_EXPORT2 ulist_create_تشه_List(UError_کوډ *حالت) {
    UList *newList = تش_اشاره;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    إذا (newList == تش_اشاره) {
        *حالت = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    
    newList->curr = تش_اشاره;
    newList->سر = تش_اشاره;
    newList->لکۍ = تش_اشاره;
    newList->size = 0;
    
    أرجع newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_جمع_کړه_لومړی_توکی(UList *list, UList_غوټه *new_توکی) {
    new_توکی->next = تش_اشاره;
    new_توکی->پخوانی = تش_اشاره;
    list->سر = new_توکی;
    list->لکۍ = new_توکی;
}

ساكن فراغ ulist_remove_توکی(UList *list, UList_غوټه *p) {
    إذا (p->پخوانی == تش_اشاره) {
        // p is the list head.
        list->سر = p->next;
    } وإلا {
        p->پخوانی->next = p->next;
    }
    إذا (p->next == تش_اشاره) {
        // p is the list tail.
        list->لکۍ = p->پخوانی;
    } وإلا {
        p->next->پخوانی = p->پخوانی;
    }
    إذا (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    إذا (p->forceDelete) {
        uprv_free(p->معلومات);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_جمع_کړه_توکی_پای_ټکی_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_کوډ *حالت) {
    UList_غوټه *new_توکی = تش_اشاره;
    
    إذا (U_FAILURE(*حالت) || list == تش_اشاره || معلومات == تش_اشاره) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        أرجع;
    }
    
    new_توکی = (UList_غوټه *)uprv_malloc(حجم(UList_غوټه));
    إذا (new_توکی == تش_اشاره) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        *حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    new_توکی->معلومات = (فراغ *)(معلومات);
    new_توکی->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جمع_کړه_لومړی_توکی(list, new_توکی);
    } وإلا {
        new_توکی->next = تش_اشاره;
        new_توکی->پخوانی = list->لکۍ;
        list->لکۍ->next = new_توکی;
        list->لکۍ = new_توکی;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_جمع_کړه_توکی_Begin_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_کوډ *حالت) {
    UList_غوټه *new_توکی = تش_اشاره;
    
    إذا (U_FAILURE(*حالت) || list == تش_اشاره || معلومات == تش_اشاره) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        أرجع;
    }
    
    new_توکی = (UList_غوټه *)uprv_malloc(حجم(UList_غوټه));
    إذا (new_توکی == تش_اشاره) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        *حالت = U_حافظه_ویش_ERROR;
        أرجع;
    }
    new_توکی->معلومات = (فراغ *)(معلومات);
    new_توکی->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جمع_کړه_لومړی_توکی(list, new_توکی);
    } وإلا {
        new_توکی->پخوانی = تش_اشاره;
        new_توکی->next = list->سر;
        list->سر->پخوانی = new_توکی;
        list->سر = new_توکی;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_لري_String(ثابت UList *list, ثابت حرف *معلومات, int32_t length) {
    إذا (list != تش_اشاره) {
        ثابت UList_غوټه *اشاره_ګر;
        لكل (اشاره_ګر = list->سر; اشاره_ګر != تش_اشاره; اشاره_ګر = اشاره_ګر->next) {
            إذا (length == (int32_t)uprv_strlen((ثابت حرف *)اشاره_ګر->معلومات)) {
                إذا (uprv_memcmp(معلومات, اشاره_ګر->معلومات, length) == 0) {
                    أرجع ریښتیا;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *معلومات) {
    إذا (list != تش_اشاره) {
        UList_غوټه *اشاره_ګر;
        لكل (اشاره_ګر = list->سر; اشاره_ګر != تش_اشاره; اشاره_ګر = اشاره_ګر->next) {
            إذا (uprv_strcmp(معلومات, (ثابت حرف *)اشاره_ګر->معلومات) == 0) {
                ulist_remove_توکی(list, اشاره_ګر);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع ریښتیا;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_get_بل(UList *list) {
    UList_غوټه *curr = تش_اشاره;
    
    إذا (list == تش_اشاره || list->curr == تش_اشاره) {
        أرجع تش_اشاره;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->معلومات;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_اندازه(ثابت UList *list) {
    إذا (list != تش_اشاره) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    إذا (list != تش_اشاره) {
        list->curr = list->سر;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_غوټه *list_سر = تش_اشاره;

    إذا (list != تش_اشاره) {
        list_سر = list->سر;
        طالما (list_سر != تش_اشاره) {
            UList_غوټه *list_اشاره_ګر = list_سر->next;

            إذا (list_سر->forceDelete) {
                uprv_free(list_سر->معلومات);
            }

            uprv_free(list_سر);
            list_سر = list_اشاره_ګر;
        }
        uprv_free(list);
        list = تش_اشاره;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_ارزښتونه_iterator(UEnumeration *en) {
    إذا (en != تش_اشاره) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_شمېر_keyword_ارزښتونه(UEnumeration *en, UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع -1;
    }
    
    أرجع ulist_get_List_اندازه((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_بل_keyword_ارزښت(UEnumeration *en, int32_t *پایله_اوږدتیا, UError_کوډ *حالت) {
    ثابت حرف *s;
    إذا (U_FAILURE(*حالت)) {
        أرجع تش_اشاره;
    }

    s = (ثابت حرف *)ulist_get_بل((UList *)(en->context));
    إذا (s != تش_اشاره && پایله_اوږدتیا != تش_اشاره) {
        *پایله_اوږدتیا = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_ارزښتونه_iterator(UEnumeration *en, UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
