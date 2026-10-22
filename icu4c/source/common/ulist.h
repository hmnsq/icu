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

#شامل "unicode/utypes.h"
#شامل "unicode/uenum.h"

ساختار UList;
تعریفنوع ساختار UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI تهی U_EXPORT2 ulist_addItemEndList(UList *list, ثابت تهی *data, UBool forceDelete, UErrorCode *status);

U_CAPI تهی U_EXPORT2 ulist_addItemBeginList(UList *list, ثابت تهی *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ثابت UList *list, ثابت نویسه *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت نویسه *data);

U_CAPI تهی *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ثابت UList *list);

U_CAPI تهی U_EXPORT2 ulist_resetList(UList *list);

U_CAPI تهی U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI تهی U_EXPORT2 ulist_close_keyword_مقادیر_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_شمار_keyword_مقادیر(UEnumeration *en, UErrorCode *status);

U_CAPI ثابت نویسه * U_EXPORT2 ulist_next_keyword_مقدار(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI تهی U_EXPORT2 ulist_reset_keyword_مقادیر_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
