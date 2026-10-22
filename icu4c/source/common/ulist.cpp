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

تعريفنوع بنية UList_گره UList_گره;
بنية UList_گره {
    فراغ *داده;
    
    UList_گره *next;
    UList_گره *قبلی;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UList_گره *curr;
    UList_گره *سر;
    UList_گره *دم;
    
    int32_t size;
};

ساكن فراغ ulist_جمع_کن_اولی_قلم(UList *list, UList_گره *new_قلم);

U_CAPI UList *U_EXPORT2 ulist_create_خالی_List(UError_کد *وضعیت) {
    UList *newList = اشاره_تهی;
    
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    إذا (newList == اشاره_تهی) {
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    
    newList->curr = اشاره_تهی;
    newList->سر = اشاره_تهی;
    newList->دم = اشاره_تهی;
    newList->size = 0;
    
    أرجع newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_جمع_کن_اولی_قلم(UList *list, UList_گره *new_قلم) {
    new_قلم->next = اشاره_تهی;
    new_قلم->قبلی = اشاره_تهی;
    list->سر = new_قلم;
    list->دم = new_قلم;
}

ساكن فراغ ulist_remove_قلم(UList *list, UList_گره *p) {
    إذا (p->قبلی == اشاره_تهی) {
        // p is the list head.
        list->سر = p->next;
    } وإلا {
        p->قبلی->next = p->next;
    }
    إذا (p->next == اشاره_تهی) {
        // p is the list tail.
        list->دم = p->قبلی;
    } وإلا {
        p->next->قبلی = p->قبلی;
    }
    إذا (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    إذا (p->forceDelete) {
        uprv_free(p->داده);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_جمع_کن_قلم_انتها_List(UList *list, ثابت فراغ *داده, UBool forceDelete, UError_کد *وضعیت) {
    UList_گره *new_قلم = اشاره_تهی;
    
    إذا (U_FAILURE(*وضعیت) || list == اشاره_تهی || داده == اشاره_تهی) {
        إذا (forceDelete) {
            uprv_free((فراغ *)داده);
        }
        أرجع;
    }
    
    new_قلم = (UList_گره *)uprv_malloc(حجم(UList_گره));
    إذا (new_قلم == اشاره_تهی) {
        إذا (forceDelete) {
            uprv_free((فراغ *)داده);
        }
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    new_قلم->داده = (فراغ *)(داده);
    new_قلم->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جمع_کن_اولی_قلم(list, new_قلم);
    } وإلا {
        new_قلم->next = اشاره_تهی;
        new_قلم->قبلی = list->دم;
        list->دم->next = new_قلم;
        list->دم = new_قلم;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_جمع_کن_قلم_Begin_List(UList *list, ثابت فراغ *داده, UBool forceDelete, UError_کد *وضعیت) {
    UList_گره *new_قلم = اشاره_تهی;
    
    إذا (U_FAILURE(*وضعیت) || list == اشاره_تهی || داده == اشاره_تهی) {
        إذا (forceDelete) {
            uprv_free((فراغ *)داده);
        }
        أرجع;
    }
    
    new_قلم = (UList_گره *)uprv_malloc(حجم(UList_گره));
    إذا (new_قلم == اشاره_تهی) {
        إذا (forceDelete) {
            uprv_free((فراغ *)داده);
        }
        *وضعیت = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    new_قلم->داده = (فراغ *)(داده);
    new_قلم->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جمع_کن_اولی_قلم(list, new_قلم);
    } وإلا {
        new_قلم->قبلی = اشاره_تهی;
        new_قلم->next = list->سر;
        list->سر->قبلی = new_قلم;
        list->سر = new_قلم;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_شامل_است_String(ثابت UList *list, ثابت حرف *داده, int32_t length) {
    إذا (list != اشاره_تهی) {
        ثابت UList_گره *اشاره_گر;
        لكل (اشاره_گر = list->سر; اشاره_گر != اشاره_تهی; اشاره_گر = اشاره_گر->next) {
            إذا (length == (int32_t)uprv_strlen((ثابت حرف *)اشاره_گر->داده)) {
                إذا (uprv_memcmp(داده, اشاره_گر->داده, length) == 0) {
                    أرجع درست;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *داده) {
    إذا (list != اشاره_تهی) {
        UList_گره *اشاره_گر;
        لكل (اشاره_گر = list->سر; اشاره_گر != اشاره_تهی; اشاره_گر = اشاره_گر->next) {
            إذا (uprv_strcmp(داده, (ثابت حرف *)اشاره_گر->داده) == 0) {
                ulist_remove_قلم(list, اشاره_گر);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع درست;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_get_بعدی(UList *list) {
    UList_گره *curr = اشاره_تهی;
    
    إذا (list == اشاره_تهی || list->curr == اشاره_تهی) {
        أرجع اشاره_تهی;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->داده;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_اندازه(ثابت UList *list) {
    إذا (list != اشاره_تهی) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    إذا (list != اشاره_تهی) {
        list->curr = list->سر;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_گره *list_سر = اشاره_تهی;

    إذا (list != اشاره_تهی) {
        list_سر = list->سر;
        طالما (list_سر != اشاره_تهی) {
            UList_گره *list_اشاره_گر = list_سر->next;

            إذا (list_سر->forceDelete) {
                uprv_free(list_سر->داده);
            }

            uprv_free(list_سر);
            list_سر = list_اشاره_گر;
        }
        uprv_free(list);
        list = اشاره_تهی;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_مقادیر_iterator(UEnumeration *en) {
    إذا (en != اشاره_تهی) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_شمار_keyword_مقادیر(UEnumeration *en, UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع -1;
    }
    
    أرجع ulist_get_List_اندازه((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_بعدی_keyword_مقدار(UEnumeration *en, int32_t *نتیجه_درازی, UError_کد *وضعیت) {
    ثابت حرف *s;
    إذا (U_FAILURE(*وضعیت)) {
        أرجع اشاره_تهی;
    }

    s = (ثابت حرف *)ulist_get_بعدی((UList *)(en->context));
    إذا (s != اشاره_تهی && نتیجه_درازی != اشاره_تهی) {
        *نتیجه_درازی = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_مقادیر_iterator(UEnumeration *en, UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
