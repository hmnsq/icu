// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#అన్తర్భావ "ulist.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uenumimp.h"

రకనిర్వచనం నిర్మాణం UList_కణుపు UList_కణుపు;
నిర్మాణం UList_కణుపు {
    శూన్య *దత్తాంశం;
    
    UList_కణుపు *next;
    UList_కణుపు *మునుపటి;
    
    /* When data is created with uprv_malloc, needs to be freed during deleteList function. */
    UBool forceDelete;
};

నిర్మాణం UList {
    UList_కణుపు *curr;
    UList_కణుపు *తల;
    UList_కణుపు *తోక;
    
    int32_t size;
};

స్థిర శూన్య ulist_కూడు_మొదటి_అంశం(UList *list, UList_కణుపు *new_అంశం);

U_CAPI UList *U_EXPORT2 ulist_create_ఖాళీలు_List(UError_సంకేతం *స్థితి_2) {
    UList *newList = శూన్యసూచిక;
    
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    
    newList = (UList *)uprv_malloc(పరిమాణం(UList));
    ఒకవేళ (newList == శూన్యసూచిక) {
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    
    newList->curr = శూన్యసూచిక;
    newList->తల = శూన్యసూచిక;
    newList->తోక = శూన్యసూచిక;
    newList->size = 0;
    
    తిరిగి newList;
}

/*
 * Function called by addItemEndList or addItemBeginList when the first item is added to the list.
 * This function properly sets the pointers for the first item added.
 */
స్థిర శూన్య ulist_కూడు_మొదటి_అంశం(UList *list, UList_కణుపు *new_అంశం) {
    new_అంశం->next = శూన్యసూచిక;
    new_అంశం->మునుపటి = శూన్యసూచిక;
    list->తల = new_అంశం;
    list->తోక = new_అంశం;
}

స్థిర శూన్య ulist_remove_అంశం(UList *list, UList_కణుపు *p) {
    ఒకవేళ (p->మునుపటి == శూన్యసూచిక) {
        // p is the list head.
        list->తల = p->next;
    } లేకపోతే {
        p->మునుపటి->next = p->next;
    }
    ఒకవేళ (p->next == శూన్యసూచిక) {
        // p is the list tail.
        list->తోక = p->మునుపటి;
    } లేకపోతే {
        p->next->మునుపటి = p->మునుపటి;
    }
    ఒకవేళ (p == list->curr) {
        list->curr = p->next;
    }
    --list->size;
    ఒకవేళ (p->forceDelete) {
        uprv_free(p->దత్తాంశం);
    }
    uprv_free(p);
}

U_CAPI శూన్య U_EXPORT2 ulist_కూడు_అంశం_చివర_List(UList *list, స్థిరరాశి శూన్య *దత్తాంశం, UBool forceDelete, UError_సంకేతం *స్థితి_2) {
    UList_కణుపు *new_అంశం = శూన్యసూచిక;
    
    ఒకవేళ (U_FAILURE(*స్థితి_2) || list == శూన్యసూచిక || దత్తాంశం == శూన్యసూచిక) {
        ఒకవేళ (forceDelete) {
            uprv_free((శూన్య *)దత్తాంశం);
        }
        తిరిగి;
    }
    
    new_అంశం = (UList_కణుపు *)uprv_malloc(పరిమాణం(UList_కణుపు));
    ఒకవేళ (new_అంశం == శూన్యసూచిక) {
        ఒకవేళ (forceDelete) {
            uprv_free((శూన్య *)దత్తాంశం);
        }
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    new_అంశం->దత్తాంశం = (శూన్య *)(దత్తాంశం);
    new_అంశం->forceDelete = forceDelete;
    
    ఒకవేళ (list->size == 0) {
        ulist_కూడు_మొదటి_అంశం(list, new_అంశం);
    } లేకపోతే {
        new_అంశం->next = శూన్యసూచిక;
        new_అంశం->మునుపటి = list->తోక;
        list->తోక->next = new_అంశం;
        list->తోక = new_అంశం;
    }
    
    list->size++;
}

U_CAPI శూన్య U_EXPORT2 ulist_కూడు_అంశం_Begin_List(UList *list, స్థిరరాశి శూన్య *దత్తాంశం, UBool forceDelete, UError_సంకేతం *స్థితి_2) {
    UList_కణుపు *new_అంశం = శూన్యసూచిక;
    
    ఒకవేళ (U_FAILURE(*స్థితి_2) || list == శూన్యసూచిక || దత్తాంశం == శూన్యసూచిక) {
        ఒకవేళ (forceDelete) {
            uprv_free((శూన్య *)దత్తాంశం);
        }
        తిరిగి;
    }
    
    new_అంశం = (UList_కణుపు *)uprv_malloc(పరిమాణం(UList_కణుపు));
    ఒకవేళ (new_అంశం == శూన్యసూచిక) {
        ఒకవేళ (forceDelete) {
            uprv_free((శూన్య *)దత్తాంశం);
        }
        *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    new_అంశం->దత్తాంశం = (శూన్య *)(దత్తాంశం);
    new_అంశం->forceDelete = forceDelete;
    
    ఒకవేళ (list->size == 0) {
        ulist_కూడు_మొదటి_అంశం(list, new_అంశం);
    } లేకపోతే {
        new_అంశం->మునుపటి = శూన్యసూచిక;
        new_అంశం->next = list->తల;
        list->తల->మునుపటి = new_అంశం;
        list->తల = new_అంశం;
    }
    
    list->size++;
}

U_CAPI UBool U_EXPORT2 ulist_కలిగియుంది_String(స్థిరరాశి UList *list, స్థిరరాశి అక్షరం *దత్తాంశం, int32_t length) {
    ఒకవేళ (list != శూన్యసూచిక) {
        స్థిరరాశి UList_కణుపు *సూచిక;
        కొరకు (సూచిక = list->తల; సూచిక != శూన్యసూచిక; సూచిక = సూచిక->next) {
            ఒకవేళ (length == (int32_t)uprv_strlen((స్థిరరాశి అక్షరం *)సూచిక->దత్తాంశం)) {
                ఒకవేళ (uprv_memcmp(దత్తాంశం, సూచిక->దత్తాంశం, length) == 0) {
                    తిరిగి నిజం;
                }
            }
        }
    }
    తిరిగి అబద్ధం;
}

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, స్థిరరాశి అక్షరం *దత్తాంశం) {
    ఒకవేళ (list != శూన్యసూచిక) {
        UList_కణుపు *సూచిక;
        కొరకు (సూచిక = list->తల; సూచిక != శూన్యసూచిక; సూచిక = సూచిక->next) {
            ఒకవేళ (uprv_strcmp(దత్తాంశం, (స్థిరరాశి అక్షరం *)సూచిక->దత్తాంశం) == 0) {
                ulist_remove_అంశం(list, సూచిక);
                // Remove only the first occurrence, like Java LinkedList.remove(Object).
                తిరిగి నిజం;
            }
        }
    }
    తిరిగి అబద్ధం;
}

U_CAPI శూన్య *U_EXPORT2 ulist_get_తదుపరి(UList *list) {
    UList_కణుపు *curr = శూన్యసూచిక;
    
    ఒకవేళ (list == శూన్యసూచిక || list->curr == శూన్యసూచిక) {
        తిరిగి శూన్యసూచిక;
    }
    
    curr = list->curr;
    list->curr = curr->next;
    
    తిరిగి curr->దత్తాంశం;
}

U_CAPI int32_t U_EXPORT2 ulist_get_List_కొలత(స్థిరరాశి UList *list) {
    ఒకవేళ (list != శూన్యసూచిక) {
        తిరిగి list->size;
    }
    
    తిరిగి -1;
}

U_CAPI శూన్య U_EXPORT2 ulist_resetList(UList *list) {
    ఒకవేళ (list != శూన్యసూచిక) {
        list->curr = list->తల;
    }
}

U_CAPI శూన్య U_EXPORT2 ulist_deleteList(UList *list) {
    UList_కణుపు *list_తల = శూన్యసూచిక;

    ఒకవేళ (list != శూన్యసూచిక) {
        list_తల = list->తల;
        అయితే (list_తల != శూన్యసూచిక) {
            UList_కణుపు *list_సూచిక = list_తల->next;

            ఒకవేళ (list_తల->forceDelete) {
                uprv_free(list_తల->దత్తాంశం);
            }

            uprv_free(list_తల);
            list_తల = list_సూచిక;
        }
        uprv_free(list);
        list = శూన్యసూచిక;
    }
}

U_CAPI శూన్య U_EXPORT2 ulist_close_keyword_విలువలు_iterator(UEnumeration *en) {
    ఒకవేళ (en != శూన్యసూచిక) {
        ulist_deleteList((UList *)(en->context));
        uprv_free(en);
    }
}

U_CAPI int32_t U_EXPORT2 ulist_లెక్క_keyword_విలువలు(UEnumeration *en, UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి -1;
    }
    
    తిరిగి ulist_get_List_కొలత((UList *)(en->context));
}

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2 ulist_తదుపరి_keyword_విలువ(UEnumeration *en, int32_t *ఫలితం_నిడివి, UError_సంకేతం *స్థితి_2) {
    స్థిరరాశి అక్షరం *s;
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    s = (స్థిరరాశి అక్షరం *)ulist_get_తదుపరి((UList *)(en->context));
    ఒకవేళ (s != శూన్యసూచిక && ఫలితం_నిడివి != శూన్యసూచిక) {
        *ఫలితం_నిడివి = static_cast<int32_t>(uprv_strlen(s));
    }
    తిరిగి s;
}

U_CAPI శూన్య U_EXPORT2 ulist_reset_keyword_విలువలు_iterator(UEnumeration *en, UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) {
        తిరిగి ;
    }
    
    ulist_resetList((UList *)(en->context));
}

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en) {
    తిరిగి (UList *)(en->context);
}
