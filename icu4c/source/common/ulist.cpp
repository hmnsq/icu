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

تعريفنوع بنية UList_ڳنڍ UList_ڳنڍ;
بنية UList_ڳنڍ {
    فراغ *معلومات;
    
    UList_ڳنڍ *next;
    UList_ڳنڍ *اڳيون;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

بنية UList {
    UList_ڳنڍ *curr;
    UList_ڳنڍ *سر;
    UList_ڳنڍ *پڇ;
    
    int32_t size;
};

ساكن فراغ ulist_جوڙيو_پهريون_شي(UList *list, UList_ڳنڍ *new_شي);

U_CAPI UList *U_EXPORT2 ulist_create_خالي_جايون_List(UError_ڪوڊ *حالت) {
    UList *newList = خالي_اشارو;
    
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    إذا (newList == خالي_اشارو) {
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    
    newList->curr = خالي_اشارو;
    newList->سر = خالي_اشارو;
    newList->پڇ = خالي_اشارو;
    newList->size = 0;
    
    أرجع newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساكن فراغ ulist_جوڙيو_پهريون_شي(UList *list, UList_ڳنڍ *new_شي) {
    new_شي->next = خالي_اشارو;
    new_شي->اڳيون = خالي_اشارو;
    list->سر = new_شي;
    list->پڇ = new_شي;
}

ساكن فراغ ulist_remove_شي(UList *list, UList_ڳنڍ *p) {
    إذا (p->اڳيون == خالي_اشارو) {
        // p is the list head.
        list->سر = p->next;
    } وإلا {
        p->اڳيون->next = p->next;
    }
    إذا (p->next == خالي_اشارو) {
        // p is the list tail.
        list->پڇ = p->اڳيون;
    } وإلا {
        p->next->اڳيون = p->اڳيون;
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

U_CAPI فراغ U_EXPORT2 ulist_جوڙيو_شي_پڇاڙي_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_ڪوڊ *حالت) {
    UList_ڳنڍ *new_شي = خالي_اشارو;
    
    إذا (U_FAILURE(*حالت) || list == خالي_اشارو || معلومات == خالي_اشارو) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        أرجع;
    }
    
    new_شي = (UList_ڳنڍ *)uprv_malloc(حجم(UList_ڳنڍ));
    إذا (new_شي == خالي_اشارو) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    new_شي->معلومات = (فراغ *)(معلومات);
    new_شي->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جوڙيو_پهريون_شي(list, new_شي);
    } وإلا {
        new_شي->next = خالي_اشارو;
        new_شي->اڳيون = list->پڇ;
        list->پڇ->next = new_شي;
        list->پڇ = new_شي;
    }
    
    list->size++;
}

U_CAPI فراغ U_EXPORT2 ulist_جوڙيو_شي_Begin_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_ڪوڊ *حالت) {
    UList_ڳنڍ *new_شي = خالي_اشارو;
    
    إذا (U_FAILURE(*حالت) || list == خالي_اشارو || معلومات == خالي_اشارو) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        أرجع;
    }
    
    new_شي = (UList_ڳنڍ *)uprv_malloc(حجم(UList_ڳنڍ));
    إذا (new_شي == خالي_اشارو) {
        إذا (forceDelete) {
            uprv_free((فراغ *)معلومات);
        }
        *حالت = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    new_شي->معلومات = (فراغ *)(معلومات);
    new_شي->forceDelete = forceDelete;
    
    إذا (list->size == 0) {
        ulist_جوڙيو_پهريون_شي(list, new_شي);
    } وإلا {
        new_شي->اڳيون = خالي_اشارو;
        new_شي->next = list->سر;
        list->سر->اڳيون = new_شي;
        list->سر = new_شي;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_شامل_آهي_String(ثابت UList *list, ثابت حرف *معلومات, int32_t length) {
    إذا (list != خالي_اشارو) {
        ثابت UList_ڳنڍ *اشارو;
        لكل (اشارو = list->سر; اشارو != خالي_اشارو; اشارو = اشارو->next) {
            إذا (length == (int32_t)uprv_strlen((ثابت حرف *)اشارو->معلومات)) {
                إذا (uprv_memcmp(معلومات, اشارو->معلومات, length) == 0) {
                    أرجع سچ;
                }
            }
        }
    }
    أرجع خطأ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *معلومات) {
    إذا (list != خالي_اشارو) {
        UList_ڳنڍ *اشارو;
        لكل (اشارو = list->سر; اشارو != خالي_اشارو; اشارو = اشارو->next) {
            إذا (uprv_strcmp(معلومات, (ثابت حرف *)اشارو->معلومات) == 0) {
                ulist_remove_شي(list, اشارو);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                أرجع سچ;
            }
        }
    }
    أرجع خطأ;
}

U_CAPI فراغ *U_EXPORT2 ulist_get_ايندڙ(UList *list) {
    UList_ڳنڍ *curr = خالي_اشارو;
    
    إذا (list == خالي_اشارو || list->curr == خالي_اشارو) {
        أرجع خالي_اشارو;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    أرجع curr->معلومات;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_سائيز(ثابت UList *list) {
    إذا (list != خالي_اشارو) {
        أرجع list->size;
    }
    
    أرجع -1;
}

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list) {
    إذا (list != خالي_اشارو) {
        list->curr = list->سر;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list) {
    UList_ڳنڍ *list_سر = خالي_اشارو;

    إذا (list != خالي_اشارو) {
        list_سر = list->سر;
        طالما (list_سر != خالي_اشارو) {
            UList_ڳنڍ *list_اشارو = list_سر->next;

            إذا (list_سر->forceDelete) {
                uprv_free(list_سر->معلومات);
            }

            uprv_free(list_سر);
            list_سر = list_اشارو;
        }
        uprv_free(list);
        list = خالي_اشارو;
    }
}

U_CAPI فراغ U_EXPORT2 ulist_close_keyword_قدرون_iterator(UEnumeration *en) {
    إذا (en != خالي_اشارو) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_ڳڻپ_keyword_قدرون(UEnumeration *en, UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع -1;
    }
    
    أرجع ulist_get_List_سائيز((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_ايندڙ_keyword_قدر(UEnumeration *en, int32_t *نتيجو_ڊيگهائي, UError_ڪوڊ *حالت) {
    ثابت حرف *s;
    إذا (U_FAILURE(*حالت)) {
        أرجع خالي_اشارو;
    }

    s = (ثابت حرف *)ulist_get_ايندڙ((UList *)(en->context));
    إذا (s != خالي_اشارو && نتيجو_ڊيگهائي != خالي_اشارو) {
        *نتيجو_ڊيگهائي = static_cast<int32_t>(uprv_strlen(s));
    }
    أرجع s;
}

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_قدرون_iterator(UEnumeration *en, UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) {
        أرجع ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    أرجع (UList *)(en->context);
}
