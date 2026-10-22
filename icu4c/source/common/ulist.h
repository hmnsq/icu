// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#اگر_تعریف_نشده ULIST_H
#تعریف_کن ULIST_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uenum.h"

بنية UList;
تعريفنوع بنية UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_خالی_List(UError_کد *وضعیت);

U_CAPI فراغ U_EXPORT2 ulist_جمع_کن_قلم_انتها_List(UList *list, ثابت فراغ *داده, UBool forceDelete, UError_کد *وضعیت);

U_CAPI فراغ U_EXPORT2 ulist_جمع_کن_قلم_Begin_List(UList *list, ثابت فراغ *داده, UBool forceDelete, UError_کد *وضعیت);

U_CAPI UBool U_EXPORT2 ulist_شامل_است_String(ثابت UList *list, ثابت حرف *داده, int32_t درازی);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *داده);

U_CAPI فراغ *U_EXPORT2 ulist_get_بعدی(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_اندازه(ثابت UList *list);

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI فراغ U_EXPORT2 ulist_close_keyword_مقادیر_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_شمار_keyword_مقادیر(UEnumeration *en, UError_کد *وضعیت);

U_CAPI ثابت حرف * U_EXPORT2 ulist_بعدی_keyword_مقدار(UEnumeration* en, int32_t *نتیجه_درازی, UError_کد* وضعیت);

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_مقادیر_iterator(UEnumeration* en, UError_کد* وضعیت);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#پایان_اگر
