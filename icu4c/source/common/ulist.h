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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uenum.h"

கட்டமைப்பு UList;
வகைவரை கட்டமைப்பு UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI வெறுமை U_EXPORT2 ulist_addItemEndList(UList *list, மாறிலி வெறுமை *data, UBool forceDelete, UErrorCode *status);

U_CAPI வெறுமை U_EXPORT2 ulist_addItemBeginList(UList *list, மாறிலி வெறுமை *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(மாறிலி UList *list, மாறிலி எழுத்து *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, மாறிலி எழுத்து *data);

U_CAPI வெறுமை *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(மாறிலி UList *list);

U_CAPI வெறுமை U_EXPORT2 ulist_resetList(UList *list);

U_CAPI வெறுமை U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI வெறுமை U_EXPORT2 ulist_close_keyword_மதிப்புகள்_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_மொத்தம்_keyword_மதிப்புகள்(UEnumeration *en, UErrorCode *status);

U_CAPI மாறிலி எழுத்து * U_EXPORT2 ulist_next_keyword_மதிப்பு(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI வெறுமை U_EXPORT2 ulist_reset_keyword_மதிப்புகள்_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
