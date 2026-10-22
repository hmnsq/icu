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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uenum.h"

কাঠামো UList;
প্রকারলেখ কাঠামো UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI শূন্য U_EXPORT2 ulist_addItemEndList(UList *list, ধ্রুবক শূন্য *তথ্য, UBool forceDelete, UErrorCode *status);

U_CAPI শূন্য U_EXPORT2 ulist_addItemBeginList(UList *list, ধ্রুবক শূন্য *তথ্য, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ধ্রুবক UList *list, ধ্রুবক অক্ষর *তথ্য, int32_t দৈর্ঘ্য);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ধ্রুবক অক্ষর *তথ্য);

U_CAPI শূন্য *U_EXPORT2 ulist_get_পরবর্তী(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_আকার(ধ্রুবক UList *list);

U_CAPI শূন্য U_EXPORT2 ulist_resetList(UList *list);

U_CAPI শূন্য U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI শূন্য U_EXPORT2 ulist_close_keyword_মানসমূহ_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_গুনতি_keyword_মানসমূহ(UEnumeration *en, UErrorCode *status);

U_CAPI ধ্রুবক অক্ষর * U_EXPORT2 ulist_পরবর্তী_keyword_মান(UEnumeration* en, int32_t *ফলাফল_দৈর্ঘ্য, UErrorCode* status);

U_CAPI শূন্য U_EXPORT2 ulist_reset_keyword_মানসমূহ_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
