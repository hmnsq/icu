// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#अगर_अघोषित ULIST_H
#घोषणा ULIST_H

#समावेश "unicode/utypes.h"
#समावेश "unicode/uenum.h"

काष्ठा UList;
प्रकार काष्ठा UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI व्योम U_EXPORT2 ulist_addItemEndList(UList *list, स्थिर व्योम *तथ्य, UBool forceDelete, UErrorCode *status);

U_CAPI व्योम U_EXPORT2 ulist_addItemBeginList(UList *list, स्थिर व्योम *तथ्य, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(स्थिर UList *list, स्थिर अक्षर *तथ्य, int32_t लंबाई);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, स्थिर अक्षर *तथ्य);

U_CAPI व्योम *U_EXPORT2 ulist_get_अग्र(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_आकार(स्थिर UList *list);

U_CAPI व्योम U_EXPORT2 ulist_resetList(UList *list);

U_CAPI व्योम U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI व्योम U_EXPORT2 ulist_close_keyword_मूल्यावली_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_गिनती_keyword_मूल्यावली(UEnumeration *en, UErrorCode *status);

U_CAPI स्थिर अक्षर * U_EXPORT2 ulist_अग्र_keyword_मूल्य(UEnumeration* en, int32_t *परिणाम_लंबाई, UErrorCode* status);

U_CAPI व्योम U_EXPORT2 ulist_reset_keyword_मूल्यावली_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#पूर्ण_अगर
