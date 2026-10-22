// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ifndef ULIST_H
#define ULIST_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"

ಸಂರಚನಾ UList;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_addItemEndList(UList *list, ಧ್ರುವ ಶೂನ್ಯ *data, UBool forceDelete, UErrorCode *status);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_addItemBeginList(UList *list, ಧ್ರುವ ಶೂನ್ಯ *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ಧ್ರುವ UList *list, ಧ್ರುವ ವರ್ಣ *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ಧ್ರುವ ವರ್ಣ *data);

U_CAPI ಶೂನ್ಯ *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ಧ್ರುವ UList *list);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು(UEnumeration *en, UErrorCode *status);

U_CAPI ಧ್ರುವ ವರ್ಣ * U_EXPORT2 ulist_next_keyword_ಮೌಲ್ಯ(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
