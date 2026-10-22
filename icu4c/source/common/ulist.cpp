// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#অন্তর্ভাব "ulist.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uenumimp.h"

প্রকারলেখ কাঠামো UList_গ্রন্থি UList_গ্রন্থি;
কাঠামো UList_গ্রন্থি {
    শূন্য *তথ্য;
    
    UList_গ্রন্থি *next;
    UList_গ্রন্থি *previous;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

কাঠামো UList {
    UList_গ্রন্থি *curr;
    UList_গ্রন্থি *শীর্ষ;
    UList_গ্রন্থি *লেজ;
    
    int32_t size;
};

স্থির শূন্য ulist_addFirstItem(UList *list, UList_গ্রন্থি *newItem);

U_CAPI UList *U_EXPORT2 ulist_createEmptyList(UErrorCode *status) {
    UList *newList = nullptr;
    
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    
    newList = (UList *)uprv_malloc(আকার(UList));
    যদি (newList == nullptr) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    
    newList->curr = nullptr;
    newList->শীর্ষ = nullptr;
    newList->লেজ = nullptr;
    newList->size = 0;
    
    ফেরত newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
স্থির শূন্য ulist_addFirstItem(UList *list, UList_গ্রন্থি *newItem) {
    newItem->next = nullptr;
    newItem->previous = nullptr;
    list->শীর্ষ = newItem;
    list->লেজ = newItem;
}

স্থির শূন্য ulist_removeItem(UList *list, UList_গ্রন্থি *p) {
    যদি (p->previous == nullptr) {
        // p is the list head.
        list->শীর্ষ = p->next;
    } অন্যথা {
        p->previous->next = p->next;
    }
    যদি (p->next == nullptr) {
        // p is the list tail.
        list->লেজ = p->previous;
    } অন্যথা {
        p->next->previous = p->previous;
    }
    যদি (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    যদি (p->forceDelete) {
        uprv_free(p->তথ্য);
    }
    uprv_free(p);
}

U_CAPI শূন্য U_EXPORT2 ulist_addItemEndList(UList *list, ধ্রুবক শূন্য *তথ্য, UBool forceDelete, UErrorCode *status) {
    UList_গ্রন্থি *newItem = nullptr;
    
    যদি (U_FAILURE(*status) || list == nullptr || তথ্য == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)তথ্য);
        }
        ফেরত;
    }
    
    newItem = (UList_গ্রন্থি *)uprv_malloc(আকার(UList_গ্রন্থি));
    যদি (newItem == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)তথ্য);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    newItem->তথ্য = (শূন্য *)(তথ্য);
    newItem->forceDelete = forceDelete;
    
    যদি (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } অন্যথা {
        newItem->next = nullptr;
        newItem->previous = list->লেজ;
        list->লেজ->next = newItem;
        list->লেজ = newItem;
    }
    
    list->size++;
}

U_CAPI শূন্য U_EXPORT2 ulist_addItemBeginList(UList *list, ধ্রুবক শূন্য *তথ্য, UBool forceDelete, UErrorCode *status) {
    UList_গ্রন্থি *newItem = nullptr;
    
    যদি (U_FAILURE(*status) || list == nullptr || তথ্য == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)তথ্য);
        }
        ফেরত;
    }
    
    newItem = (UList_গ্রন্থি *)uprv_malloc(আকার(UList_গ্রন্থি));
    যদি (newItem == nullptr) {
        যদি (forceDelete) {
            uprv_free((শূন্য *)তথ্য);
        }
        *status = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    newItem->তথ্য = (শূন্য *)(তথ্য);
    newItem->forceDelete = forceDelete;
    
    যদি (list->size == 0) {
        ulist_addFirstItem(list, newItem);
    } অন্যথা {
        newItem->previous = nullptr;
        newItem->next = list->শীর্ষ;
        list->শীর্ষ->previous = newItem;
        list->শীর্ষ = newItem;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_containsString(ধ্রুবক UList *list, ধ্রুবক অক্ষর *তথ্য, int32_t length) {
    যদি (list != nullptr) {
        ধ্রুবক UList_গ্রন্থি *pointer;
        জন্য (pointer = list->শীর্ষ; pointer != nullptr; pointer = pointer->next) {
            যদি (length == (int32_t)uprv_strlen((ধ্রুবক অক্ষর *)pointer->তথ্য)) {
                যদি (uprv_memcmp(তথ্য, pointer->তথ্য, length) == 0) {
                    ফেরত সত্য;
                }
            }
        }
    }
    ফেরত মিথ্যা;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ধ্রুবক অক্ষর *তথ্য) {
    যদি (list != nullptr) {
        UList_গ্রন্থি *pointer;
        জন্য (pointer = list->শীর্ষ; pointer != nullptr; pointer = pointer->next) {
            যদি (uprv_strcmp(তথ্য, (ধ্রুবক অক্ষর *)pointer->তথ্য) == 0) {
                ulist_removeItem(list, pointer);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                ফেরত সত্য;
            }
        }
    }
    ফেরত মিথ্যা;
}

U_CAPI শূন্য *U_EXPORT2 ulist_get_পরবর্তী(UList *list) {
    UList_গ্রন্থি *curr = nullptr;
    
    যদি (list == nullptr || list->curr == nullptr) {
        ফেরত nullptr;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    ফেরত curr->তথ্য;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_আকার(ধ্রুবক UList *list) {
    যদি (list != nullptr) {
        ফেরত list->size;
    }
    
    ফেরত -1;
}

U_CAPI শূন্য U_EXPORT2 ulist_resetList(UList *list) {
    যদি (list != nullptr) {
        list->curr = list->শীর্ষ;
    }
}

U_CAPI শূন্য U_EXPORT2 ulist_deleteList(UList *list) {
    UList_গ্রন্থি *list_শীর্ষ = nullptr;

    যদি (list != nullptr) {
        list_শীর্ষ = list->শীর্ষ;
        যখন (list_শীর্ষ != nullptr) {
            UList_গ্রন্থি *listPointer = list_শীর্ষ->next;

            যদি (list_শীর্ষ->forceDelete) {
                uprv_free(list_শীর্ষ->তথ্য);
            }

            uprv_free(list_শীর্ষ);
            list_শীর্ষ = listPointer;
        }
        uprv_free(list);
        list = nullptr;
    }
}

U_CAPI শূন্য U_EXPORT2 ulist_close_keyword_মানসমূহ_iterator(UEnumeration *en) {
    যদি (en != nullptr) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_গুনতি_keyword_মানসমূহ(UEnumeration *en, UErrorCode *status) {
    যদি (U_FAILURE(*status)) {
        ফেরত -1;
    }
    
    ফেরত ulist_get_List_আকার((UList *)(en->context));
}

U_CAPI ধ্রুবক অক্ষর * U_EXPORT2 ulist_পরবর্তী_keyword_মান(UEnumeration *en, int32_t *ফলাফল_দৈর্ঘ্য, UErrorCode *status) {
    ধ্রুবক অক্ষর *s;
    যদি (U_FAILURE(*status)) {
        ফেরত nullptr;
    }

    s = (ধ্রুবক অক্ষর *)ulist_get_পরবর্তী((UList *)(en->context));
    যদি (s != nullptr && ফলাফল_দৈর্ঘ্য != nullptr) {
        *ফলাফল_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(s));
    }
    ফেরত s;
}

U_CAPI শূন্য U_EXPORT2 ulist_reset_keyword_মানসমূহ_iterator(UEnumeration *en, UErrorCode *status) {
    যদি (U_FAILURE(*status)) {
        ফেরত ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    ফেরত (UList *)(en->context);
}
