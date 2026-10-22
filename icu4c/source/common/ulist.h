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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uenum.h"

ସଂରଚନା UList;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_addItemEndList(UList *list, ଅଚଳ ଶୂନ୍ୟ *data, UBool forceDelete, UErrorCode *status);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_addItemBeginList(UList *list, ଅଚଳ ଶୂନ୍ୟ *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(ଅଚଳ UList *list, ଅଚଳ ଅକ୍ଷର *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ଅଚଳ ଅକ୍ଷର *data);

U_CAPI ଶୂନ୍ୟ *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(ଅଚଳ UList *list);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_close_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ଗଣନା_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ(UEnumeration *en, UErrorCode *status);

U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2 ulist_next_keyword_ମୂଲ୍ୟ(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI ଶୂନ୍ୟ U_EXPORT2 ulist_reset_keyword_ମୂଲ୍ୟଗୁଡ଼ିକ_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
