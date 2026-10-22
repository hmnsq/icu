// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#جے_بنا_تعریف ULIST_H
#تعریف_کرو ULIST_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uenum.h"

بنية UList;
تعريفنوع بنية UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_خالی_تھاواں_List(UError_کوڈ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جوڑو_شے_سرا_List(UList *list, ثابت فراغ *انکڑے, UBool forceDelete, UError_کوڈ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جوڑو_شے_Begin_List(UList *list, ثابت فراغ *انکڑے, UBool forceDelete, UError_کوڈ *حالت);

U_CAPI UBool U_EXPORT2 ulist_شامل_اے_String(ثابت UList *list, ثابت حرف *انکڑے, int32_t لمائی);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *انکڑے);

U_CAPI فراغ *U_EXPORT2 ulist_get_اگلا(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_ناپ(ثابت UList *list);

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI فراغ U_EXPORT2 ulist_close_keyword_اقدار_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_گنتی_keyword_اقدار(UEnumeration *en, UError_کوڈ *حالت);

U_CAPI ثابت حرف * U_EXPORT2 ulist_اگلا_keyword_قدر(UEnumeration* en, int32_t *نتیجہ_لمائی, UError_کوڈ* حالت);

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_اقدار_iterator(UEnumeration* en, UError_کوڈ* حالت);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#جے_انت
