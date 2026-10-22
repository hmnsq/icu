// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#నిర్వచితం_కాకపోతే ULIST_H
#నిర్వచించు ULIST_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uenum.h"

నిర్మాణం UList;
రకనిర్వచనం నిర్మాణం UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_ఖాళీలు_List(UError_సంకేతం *స్థితి_2);

U_CAPI శూన్య U_EXPORT2 ulist_కూడు_అంశం_చివర_List(UList *list, స్థిరరాశి శూన్య *దత్తాంశం, UBool forceDelete, UError_సంకేతం *స్థితి_2);

U_CAPI శూన్య U_EXPORT2 ulist_కూడు_అంశం_Begin_List(UList *list, స్థిరరాశి శూన్య *దత్తాంశం, UBool forceDelete, UError_సంకేతం *స్థితి_2);

U_CAPI UBool U_EXPORT2 ulist_కలిగియుంది_String(స్థిరరాశి UList *list, స్థిరరాశి అక్షరం *దత్తాంశం, int32_t నిడివి);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, స్థిరరాశి అక్షరం *దత్తాంశం);

U_CAPI శూన్య *U_EXPORT2 ulist_get_తదుపరి(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_కొలత(స్థిరరాశి UList *list);

U_CAPI శూన్య U_EXPORT2 ulist_resetList(UList *list);

U_CAPI శూన్య U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI శూన్య U_EXPORT2 ulist_close_keyword_విలువలు_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_లెక్క_keyword_విలువలు(UEnumeration *en, UError_సంకేతం *స్థితి_2);

U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2 ulist_తదుపరి_keyword_విలువ(UEnumeration* en, int32_t *ఫలితం_నిడివి, UError_సంకేతం* స్థితి_2);

U_CAPI శూన్య U_EXPORT2 ulist_reset_keyword_విలువలు_iterator(UEnumeration* en, UError_సంకేతం* స్థితి_2);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#అయితే_ముగింపు
