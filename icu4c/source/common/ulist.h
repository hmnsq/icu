// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#که_نه_تعریف_شوی ULIST_H
#تعریف_کړه ULIST_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uenum.h"

بنية UList;
تعريفنوع بنية UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_تشه_List(UError_کوډ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جمع_کړه_توکی_پای_ټکی_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_کوډ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جمع_کړه_توکی_Begin_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_کوډ *حالت);

U_CAPI UBool U_EXPORT2 ulist_لري_String(ثابت UList *list, ثابت حرف *معلومات, int32_t اوږدتیا);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *معلومات);

U_CAPI فراغ *U_EXPORT2 ulist_get_بل(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_اندازه(ثابت UList *list);

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI فراغ U_EXPORT2 ulist_close_keyword_ارزښتونه_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_شمېر_keyword_ارزښتونه(UEnumeration *en, UError_کوډ *حالت);

U_CAPI ثابت حرف * U_EXPORT2 ulist_بل_keyword_ارزښت(UEnumeration* en, int32_t *پایله_اوږدتیا, UError_کوډ* حالت);

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_ارزښتونه_iterator(UEnumeration* en, UError_کوډ* حالت);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#د_که_پای
