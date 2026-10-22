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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uenum.h"

ഘടന UList;
തരനിർവചനം ഘടന UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI ശൂന്യം U_EXPORT2 ulist_addItemEndList(UList *list, സ്ഥിരം ശൂന്യം *data, UBool forceDelete, UErrorCode *status);

U_CAPI ശൂന്യം U_EXPORT2 ulist_addItemBeginList(UList *list, സ്ഥിരം ശൂന്യം *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(സ്ഥിരം UList *list, സ്ഥിരം അക്ഷരം *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, സ്ഥിരം അക്ഷരം *data);

U_CAPI ശൂന്യം *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(സ്ഥിരം UList *list);

U_CAPI ശൂന്യം U_EXPORT2 ulist_resetList(UList *list);

U_CAPI ശൂന്യം U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI ശൂന്യം U_EXPORT2 ulist_close_keyword_മൂല്യങ്ങൾ_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_എണ്ണം_keyword_മൂല്യങ്ങൾ(UEnumeration *en, UErrorCode *status);

U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2 ulist_next_keyword_മൂല്യം(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI ശൂന്യം U_EXPORT2 ulist_reset_keyword_മൂല്യങ്ങൾ_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
