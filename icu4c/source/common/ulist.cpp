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

تعريفنوع بنية UList_گرہ UList_گرہ;
بنية UList_گرہ {
    فراغ *انکڑے;
    
    UList_گرہ *next;
    UList_گرہ *پچھلا;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UList_گرہ *curr;
    UList_گرہ *سر;
    UList_گرہ *پوچھ;
    
    int32_t size;
};

ساكن فراغ ulist_جوڑو_پہلا_شے(UList *list, UList_گرہ *new_شے);

U_CAPI UList *U_EXPORT2 ulist_create_خالی_تھاواں_List(UError_کوڈ *حالت) {
    UList *newList = خالی_سوچک;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    إذا (newList == خالی_سوچک) {
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    
    newList->curr = خالی_سوچک;
    newList->سر = خالی_سوچک;
    newList->پوچھ = خالی_سوچک;
    newList->size = 0;
    
    أرجع newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_جوڑو_پہلا_شے(UList *list, UList_گرہ *new_شے) {
    new_شے->next = خالی_سوچک;
    new_شے->پچھلا = خالی_سوچک;
    list->سر = new_شے;
    list->پوچھ = new_شے;
}

ساكن فراغ ulist_remove_شے(UList *list, UList_گرہ *p) {
    إذا (p->پچھلا == خالی_سوچک) {
        // p is the list head.
        list->سر = p->next;
    } وإلا {
        p->پچھلا->next = p->next;
    }
    إذا (p->next == خالی_سوچک) {
        // p is the list tail.
        list->پوچھ = p->پچھلا;
    } وإلا {
        p->next->پچھلا = p->پچھلا;
    }
    إذا (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    إذا (p->forceDelete) {
        uprv_free(p->انکڑے);
    }
    uprv_free(p);
}

U_CAPI فراغ U_EXPORT2 ulist_جوڑو_شے_سرا_List(UList *list, ثابت فراغ *انکڑے, UBool forceDelete, UError_کوڈ *حالت) {
    UList_گرہ *new_شے = خالی_سوچک;
    
    إذا (U_FAILURE(*حالت) || list == خالی_سوچک || انکڑے == خالی_سوچک) {
        إذا (forceDelete) {
            uprv_free((فراغ *)انکڑے);
        }
        أرجع;
    }
    
    new_شے = (UList_گرہ *)uprv_malloc(حجم(UList_گرہ));
    إذا (new_شے == خالی_سوچک) {
        إذا (forceDelete) {
            uprv_free((فراغ *)انکڑے);
        }
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    new_شے->انکڑے = (فراغ *)(انکڑے);
    new_شے->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جوڑو_پہلا_شے(list, new_شے);
    } وإلا {
        new_شے->next = خالی_سوچک;
        new_شے->پچھلا = list->پوچھ;
        list->پوچھ->next = new_شے;
        list->پوچھ = new_شے;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_جوڑو_شے_Begin_List(UList *list, ثابت فراغ *انکڑے, UBool forceDelete, UError_کوڈ *حالت) {
    UList_گرہ *new_شے = خالی_سوچک;
    
    إذا (U_FAILURE(*حالت) || list == خالی_سوچک || انکڑے == خالی_سوچک) {
        إذا (forceDelete) {
            uprv_free((فراغ *)انکڑے);
        }
        أرجع;
    }
    
    new_شے = (UList_گرہ *)uprv_malloc(حجم(UList_گرہ));
    إذا (new_شے == خالی_سوچک) {
        إذا (forceDelete) {
            uprv_free((فراغ *)انکڑے);
        }
        *حالت = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    new_شے->انکڑے = (فراغ *)(انکڑے);
    new_شے->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جوڑو_پہلا_شے(list, new_شے);
    } وإلا {
        new_شے->پچھلا = خالی_سوچک;
        new_شے->next = list->سر;
        list->سر->پچھلا = new_شے;
        list->سر = new_شے;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_شامل_اے_String(ثابت UList *list, ثابت حرف *انکڑے, int32_t length) {
    إذا (list != خالی_سوچک) {
        ثابت UList_گرہ *اشارہ_گر;
        لكل (اشارہ_گر = list->سر; اشارہ_گر != خالی_سوچک; اشارہ_گر = اشارہ_گر->next) {
            إذا (length == (int32_t)uprv_strlen((ثابت حرف *)اشارہ_گر->انکڑے)) {
                إذا (uprv_memcmp(انکڑے, اشارہ_گر->انکڑے, length) == 0) {
                    أرجع سچ;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *انکڑے) {
    إذا (list != خالی_سوچک) {
        UList_گرہ *اشارہ_گر;
        لكل (اشارہ_گر = list->سر; اشارہ_گر != خالی_سوچک; اشارہ_گر = اشارہ_گر->next) {
            إذا (uprv_strcmp(انکڑے, (ثابت حرف *)اشارہ_گر->انکڑے) == 0) {
                ulist_remove_شے(list, اشارہ_گر);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_get_اگلا(UList *list) {
    UList_گرہ *curr = خالی_سوچک;
    
    إذا (list == خالی_سوچک || list->curr == خالی_سوچک) {
        أرجع خالی_سوچک;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->انکڑے;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_ناپ(ثابت UList *list) {
    إذا (list != خالی_سوچک) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    إذا (list != خالی_سوچک) {
        list->curr = list->سر;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_گرہ *list_سر = خالی_سوچک;

    إذا (list != خالی_سوچک) {
        list_سر = list->سر;
        طالما (list_سر != خالی_سوچک) {
            UList_گرہ *list_اشارہ_گر = list_سر->next;

            إذا (list_سر->forceDelete) {
                uprv_free(list_سر->انکڑے);
            }

            uprv_free(list_سر);
            list_سر = list_اشارہ_گر;
        }
        uprv_free(list);
        list = خالی_سوچک;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_اقدار_iterator(UEnumeration *en) {
    إذا (en != خالی_سوچک) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_گنتی_keyword_اقدار(UEnumeration *en, UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع -1;
    }
    
    أرجع ulist_get_List_ناپ((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_اگلا_keyword_قدر(UEnumeration *en, int32_t *نتیجہ_لمائی, UError_کوڈ *حالت) {
    ثابت حرف *s;
    إذا (U_FAILURE(*حالت)) {
        أرجع خالی_سوچک;
    }

    s = (ثابت حرف *)ulist_get_اگلا((UList *)(en->context));
    إذا (s != خالی_سوچک && نتیجہ_لمائی != خالی_سوچک) {
        *نتیجہ_لمائی = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_اقدار_iterator(UEnumeration *en, UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
