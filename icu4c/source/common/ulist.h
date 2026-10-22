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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uenum.h"

સંરચના UList;
પ્રકારવ્યાખ્યા સંરચના UList UList;

U_CAPI UList * U_EXPORT2 ulist_createEmptyList(UErrorCode *status);

U_CAPI શૂન્ય U_EXPORT2 ulist_addItemEndList(UList *list, અચલ શૂન્ય *data, UBool forceDelete, UErrorCode *status);

U_CAPI શૂન્ય U_EXPORT2 ulist_addItemBeginList(UList *list, અચલ શૂન્ય *data, UBool forceDelete, UErrorCode *status);

U_CAPI UBool U_EXPORT2 ulist_containsString(અચલ UList *list, અચલ અક્ષર *data, int32_t length);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, અચલ અક્ષર *data);

U_CAPI શૂન્ય *U_EXPORT2 ulist_getNext(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_getListSize(અચલ UList *list);

U_CAPI શૂન્ય U_EXPORT2 ulist_resetList(UList *list);

U_CAPI શૂન્ય U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI શૂન્ય U_EXPORT2 ulist_close_keyword_કિંમતો_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ગણતરી_keyword_કિંમતો(UEnumeration *en, UErrorCode *status);

U_CAPI અચલ અક્ષર * U_EXPORT2 ulist_next_keyword_કિંમત(UEnumeration* en, int32_t *resultLength, UErrorCode* status);

U_CAPI શૂન્ય U_EXPORT2 ulist_reset_keyword_કિંમતો_iterator(UEnumeration* en, UErrorCode* status);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#endif
