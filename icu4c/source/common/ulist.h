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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uenum.h"

संरचना UList;
प्रकारनिर्देश संरचना UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI शून्य U_EXPORT2 ulist_addItemEndList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status);

U_CAPI शून्य U_EXPORT2 ulist_addItemBeginList(UList *list, ध्रुव शून्य *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ध्रुव UList *list, ध्रुव वर्ण *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ध्रुव वर्ण *data);

U_CAPI शून्य *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ध्रुव UList *list);

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list);

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मूल्ये_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_मोजणी_keyword_मूल्ये(UEnumeration *en, UErrorCode *status);

U_CAPI ध्रुव वर्ण * U_EXPORT2 ulist_next_keyword_मूल्य(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मूल्ये_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
