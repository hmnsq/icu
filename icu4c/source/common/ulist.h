// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#جيڪڏهن_وصف_نه_ٿيل ULIST_H
#وصف_ڪريو ULIST_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uenum.h"

بنية UList;
تعريفنوع بنية UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_خالي_جايون_List(UError_ڪوڊ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جوڙيو_شي_پڇاڙي_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_ڪوڊ *حالت);

U_CAPI فراغ U_EXPORT2 ulist_جوڙيو_شي_Begin_List(UList *list, ثابت فراغ *معلومات, UBool forceDelete, UError_ڪوڊ *حالت);

U_CAPI UBool U_EXPORT2 ulist_شامل_آهي_String(ثابت UList *list, ثابت حرف *معلومات, int32_t ڊيگهائي);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ثابت حرف *معلومات);

U_CAPI فراغ *U_EXPORT2 ulist_get_ايندڙ(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_سائيز(ثابت UList *list);

U_CAPI فراغ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI فراغ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI فراغ U_EXPORT2 ulist_close_keyword_قدرون_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ڳڻپ_keyword_قدرون(UEnumeration *en, UError_ڪوڊ *حالت);

U_CAPI ثابت حرف * U_EXPORT2 ulist_ايندڙ_keyword_قدر(UEnumeration* en, int32_t *نتيجو_ڊيگهائي, UError_ڪوڊ* حالت);

U_CAPI فراغ U_EXPORT2 ulist_reset_keyword_قدرون_iterator(UEnumeration* en, UError_ڪوڊ* حالت);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#جيڪڏهن_پورو
