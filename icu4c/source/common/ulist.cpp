// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#شامل "ulist.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uenumimp.h"

قسمتعریف ساخت UList_گرہ UList_گرہ;
ساخت UList_گرہ {
    خالی *ڈیٹا;
    
    UList_گرہ *next;
    UList_گرہ *پچھلا_2;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

ساخت UList {
    UList_گرہ *curr;
    UList_گرہ *سر;
    UList_گرہ *دم;
    
    int32_t size;
};

ساکن خالی ulist_جوڑو_پہلا_شے(UList *list, UList_گرہ *new_شے);

U_CAPI UList *U_EXPORT2 ulist_create_خالی_خانے_List(UError_رمز *حالت_2) {
    UList *newList = خالی_اشاریہ;
    
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }
    
    newList = (UList *)uprv_malloc(حجم(UList));
    اگر (newList == خالی_اشاریہ) {
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    
    newList->curr = خالی_اشاریہ;
    newList->سر = خالی_اشاریہ;
    newList->دم = خالی_اشاریہ;
    newList->size = 0;
    
    واپس newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
ساکن خالی ulist_جوڑو_پہلا_شے(UList *list, UList_گرہ *new_شے) {
    new_شے->next = خالی_اشاریہ;
    new_شے->پچھلا_2 = خالی_اشاریہ;
    list->سر = new_شے;
    list->دم = new_شے;
}

ساکن خالی ulist_remove_شے(UList *list, UList_گرہ *p) {
    اگر (p->پچھلا_2 == خالی_اشاریہ) {
        // p is the list head.
        list->سر = p->next;
    } ورنہ {
        p->پچھلا_2->next = p->next;
    }
    اگر (p->next == خالی_اشاریہ) {
        // p is the list tail.
        list->دم = p->پچھلا_2;
    } ورنہ {
        p->next->پچھلا_2 = p->پچھلا_2;
    }
    اگر (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    اگر (p->forceDelete) {
        uprv_free(p->ڈیٹا);
    }
    uprv_free(p);
}

U_CAPI خالی U_EXPORT2 ulist_جوڑو_شے_انتہا_List(UList *list, ثابت خالی *ڈیٹا, UBool forceDelete, UError_رمز *حالت_2) {
    UList_گرہ *new_شے = خالی_اشاریہ;
    
    اگر (U_FAILURE(*حالت_2) || list == خالی_اشاریہ || ڈیٹا == خالی_اشاریہ) {
        اگر (forceDelete) {
            uprv_free((خالی *)ڈیٹا);
        }
        واپس;
    }
    
    new_شے = (UList_گرہ *)uprv_malloc(حجم(UList_گرہ));
    اگر (new_شے == خالی_اشاریہ) {
        اگر (forceDelete) {
            uprv_free((خالی *)ڈیٹا);
        }
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    new_شے->ڈیٹا = (خالی *)(ڈیٹا);
    new_شے->forceDelete = forceDelete;
    
    اگر (list->size == 0) {
        ulist_جوڑو_پہلا_شے(list, new_شے);
    } ورنہ {
        new_شے->next = خالی_اشاریہ;
        new_شے->پچھلا_2 = list->دم;
        list->دم->next = new_شے;
        list->دم = new_شے;
    }
    
    list->size++;
}

U_CAPI خالی U_EXPORT2 ulist_جوڑو_شے_Begin_List(UList *list, ثابت خالی *ڈیٹا, UBool forceDelete, UError_رمز *حالت_2) {
    UList_گرہ *new_شے = خالی_اشاریہ;
    
    اگر (U_FAILURE(*حالت_2) || list == خالی_اشاریہ || ڈیٹا == خالی_اشاریہ) {
        اگر (forceDelete) {
            uprv_free((خالی *)ڈیٹا);
        }
        واپس;
    }
    
    new_شے = (UList_گرہ *)uprv_malloc(حجم(UList_گرہ));
    اگر (new_شے == خالی_اشاریہ) {
        اگر (forceDelete) {
            uprv_free((خالی *)ڈیٹا);
        }
        *حالت_2 = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    new_شے->ڈیٹا = (خالی *)(ڈیٹا);
    new_شے->forceDelete = forceDelete;
    
    اگر (list->size == 0) {
        ulist_جوڑو_پہلا_شے(list, new_شے);
    } ورنہ {
        new_شے->پچھلا_2 = خالی_اشاریہ;
        new_شے->next = list->سر;
        list->سر->پچھلا_2 = new_شے;
        list->سر = new_شے;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_شامل_ہے_String(ثابت UList *list, ثابت حرف *ڈیٹا, int32_t length) {
    اگر (list != خالی_اشاریہ) {
        ثابت UList_گرہ *اشارہ_گر;
        برائے (اشارہ_گر = list->سر; اشارہ_گر != خالی_اشاریہ; اشارہ_گر = اشارہ_گر->next) {
            اگر (length == (int32_t)uprv_strlen((ثابت حرف *)اشارہ_گر->ڈیٹا)) {
                اگر (uprv_memcmp(ڈیٹا, اشارہ_گر->ڈیٹا, length) == 0) {
                    واپس سچ;
                }
            }
        }
    }
    واپس جھوٹ;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *ڈیٹا) {
    اگر (list != خالی_اشاریہ) {
        UList_گرہ *اشارہ_گر;
        برائے (اشارہ_گر = list->سر; اشارہ_گر != خالی_اشاریہ; اشارہ_گر = اشارہ_گر->next) {
            اگر (uprv_strcmp(ڈیٹا, (ثابت حرف *)اشارہ_گر->ڈیٹا) == 0) {
                ulist_remove_شے(list, اشارہ_گر);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                واپس سچ;
            }
        }
    }
    واپس جھوٹ;
}

U_CAPI خالی *U_EXPORT2 ulist_get_اگلا(UList *list) {
    UList_گرہ *curr = خالی_اشاریہ;
    
    اگر (list == خالی_اشاریہ || list->curr == خالی_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    واپس curr->ڈیٹا;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_سائز(ثابت UList *list) {
    اگر (list != خالی_اشاریہ) {
        واپس list->size;
    }
    
    واپس -1;
}

U_CAPI خالی U_EXPORT2 ulist_resetList(UList *list) {
    اگر (list != خالی_اشاریہ) {
        list->curr = list->سر;
    }
}

U_CAPI خالی U_EXPORT2 ulist_deleteList(UList *list) {
    UList_گرہ *list_سر = خالی_اشاریہ;

    اگر (list != خالی_اشاریہ) {
        list_سر = list->سر;
        جب (list_سر != خالی_اشاریہ) {
            UList_گرہ *list_اشارہ_گر = list_سر->next;

            اگر (list_سر->forceDelete) {
                uprv_free(list_سر->ڈیٹا);
            }

            uprv_free(list_سر);
            list_سر = list_اشارہ_گر;
        }
        uprv_free(list);
        list = خالی_اشاریہ;
    }
}

U_CAPI خالی U_EXPORT2 ulist_close_keyword_اقدار_iterator(UEnumeration *en) {
    اگر (en != خالی_اشاریہ) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_گنتی_keyword_اقدار(UEnumeration *en, UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) {
        واپس -1;
    }
    
    واپس ulist_get_List_سائز((UList *)(en->context));
}

U_CAPI ثابت حرف * U_EXPORT2 ulist_اگلا_keyword_قدر(UEnumeration *en, int32_t *نتیجہ_لمبائی, UError_رمز *حالت_2) {
    ثابت حرف *s;
    اگر (U_FAILURE(*حالت_2)) {
        واپس خالی_اشاریہ;
    }

    s = (ثابت حرف *)ulist_get_اگلا((UList *)(en->context));
    اگر (s != خالی_اشاریہ && نتیجہ_لمبائی != خالی_اشاریہ) {
        *نتیجہ_لمبائی = static_cast<int32_t>(uprv_strlen(s));
    }
    واپس s;
}

U_CAPI خالی U_EXPORT2 ulist_reset_keyword_اقدار_iterator(UEnumeration *en, UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) {
        واپس ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    واپس (UList *)(en->context);
}
