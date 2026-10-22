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

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/uenum.h"

সংরচনা UList;
প্রকারনির্দেশ সংরচনা UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI শূন্য U_EXPORT2 ulist_addItemEndList(UList *list, ধ্রুৱ শূন্য *data, UBool forceDelete, UErrorCode *status);

U_CAPI শূন্য U_EXPORT2 ulist_addItemBeginList(UList *list, ধ্রুৱ শূন্য *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ধ্রুৱ UList *list, ধ্রুৱ ৱর্ণ *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ধ্রুৱ ৱর্ণ *data);

U_CAPI শূন্য *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ধ্রুৱ UList *list);

U_CAPI শূন্য U_EXPORT2 ulist_resetList(UList *list);

U_CAPI শূন্য U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI শূন্য U_EXPORT2 ulist_close_keyword_মানসমূহ_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_গুনতি_keyword_মানসমূহ(UEnumeration *en, UErrorCode *status);

U_CAPI ধ্রুৱ ৱর্ণ * U_EXPORT2 ulist_next_keyword_মান(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI শূন্য U_EXPORT2 ulist_reset_keyword_মানসমূহ_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
