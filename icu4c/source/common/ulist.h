// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ULIST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULIST_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uenum.h"

ਬਣਤਰ UList;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_ਖਾਲੀ_ਥਾਵਾਂ_List(UError_ਕੋਡ *ਸਥਿਤੀ);

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_ਜੋੜੋ_ਵਸਤੂ_ਸਿਰਾ_List(UList *list, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UBool forceDelete, UError_ਕੋਡ *ਸਥਿਤੀ);

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_ਜੋੜੋ_ਵਸਤੂ_Begin_List(UList *list, ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ, UBool forceDelete, UError_ਕੋਡ *ਸਥਿਤੀ);

U_CAPI UBool U_EXPORT2 ulist_ਸ਼ਾਮਲ_ਹੈ_String(ਅਚਲ UList *list, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ, int32_t ਲੰਮਾਈ);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ);

U_CAPI ਖਾਲੀ *U_EXPORT2 ulist_get_ਅਗਲਾ(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_ਮਾਪ(ਅਚਲ UList *list);

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI ਖਾਲੀ U_EXPORT2 ulist_close_keyword_ਮੁੱਲਾਂ_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ਗਿਣਤੀ_keyword_ਮੁੱਲਾਂ(UEnumeration *en, UError_ਕੋਡ *ਸਥਿਤੀ);

U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2 ulist_ਅਗਲਾ_keyword_ਮੁੱਲ(UEnumeration* en, int32_t *ਨਤੀਜਾ_ਲੰਮਾਈ, UError_ਕੋਡ* ਸਥਿਤੀ);

U_CAPI ਖਾਲੀ U_EXPORT2 ulist_reset_keyword_ਮੁੱਲਾਂ_iterator(UEnumeration* en, UError_ਕੋਡ* ਸਥਿਤੀ);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#ਜੇ_ਅੰਤ
