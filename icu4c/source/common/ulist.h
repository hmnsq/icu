// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#اگر_غیر_تعریف_شدہ ULIST_H
#تعریف_کرو ULIST_H

#شامل "unicode/utypes.h"
#شامل "unicode/uenum.h"

ساخت UList;
قسمتعریف ساخت UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_خالی_خانے_List(UError_رمز *حالت_2);

U_CAPI خالی U_EXPORT2 ulist_جوڑو_شے_انتہا_List(UList *list, ثابت خالی *ڈیٹا, UBool forceDelete, UError_رمز *حالت_2);

U_CAPI خالی U_EXPORT2 ulist_جوڑو_شے_Begin_List(UList *list, ثابت خالی *ڈیٹا, UBool forceDelete, UError_رمز *حالت_2);

U_CAPI UBool U_EXPORT2 ulist_شامل_ہے_String(ثابت UList *list, ثابت حرف *ڈیٹا, int32_t لمبائی);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *ڈیٹا);

U_CAPI خالی *U_EXPORT2 ulist_get_اگلا(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_سائز(ثابت UList *list);

U_CAPI خالی U_EXPORT2 ulist_resetList(UList *list);

U_CAPI خالی U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI خالی U_EXPORT2 ulist_close_keyword_اقدار_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_گنتی_keyword_اقدار(UEnumeration *en, UError_رمز *حالت_2);

U_CAPI ثابت حرف * U_EXPORT2 ulist_اگلا_keyword_قدر(UEnumeration* en, int32_t *نتیجہ_لمبائی, UError_رمز* حالت_2);

U_CAPI خالی U_EXPORT2 ulist_reset_keyword_اقدار_iterator(UEnumeration* en, UError_رمز* حالت_2);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#اگر_ختم
