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

#ضمن "unicode/utypes.h"
#ضمن "unicode/uenum.h"

بنية UList;
تعريفنوع بنية UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI فراغ U_EXPORT2 ulist_addItemEndList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status);

U_CAPI فراغ U_EXPORT2 ulist_addItemBeginList(UList *list, ثابت فراغ *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ثابت UList *list, ثابت حرف *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *data);

U_CAPI فراغ *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list);

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI فراغ U_EXPORT2 ulist_close_keyword_قيم_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_عدد_keyword_قيم(UEnumeration *en, UErrorCode *status);

U_CAPI ثابت حرف * U_EXPORT2 ulist_next_keyword_قيمة(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_قيم_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
