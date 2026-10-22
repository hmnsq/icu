// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ULIST_H
#ವ್ಯಾಖ್ಯಾನಿಸು ULIST_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"

ರಚನೆ UList;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_ಖಾಲಿಗಳು_List(UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_ಕೂಡಿಸು_ಅಂಶ_ಕೊನೆ_List(UList *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UBool forceDelete, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_ಕೂಡಿಸು_ಅಂಶ_Begin_List(UList *list, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, UBool forceDelete, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

U_CAPI UBool U_EXPORT2 ulist_ಒಳಗೊಂಡಿದೆ_String(ಸ್ಥಿರಾಂಕ UList *list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ, int32_t ಉದ್ದಳತೆ);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ);

U_CAPI ಶೂನ್ಯ *U_EXPORT2 ulist_get_ಮುಂದಿನ(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UList *list);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_resetList(UList *list);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_close_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_ಎಣಿಕೆ_keyword_ಮೌಲ್ಯಗಳು(UEnumeration *en, UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2);

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2 ulist_ಮುಂದಿನ_keyword_ಮೌಲ್ಯ(UEnumeration* en, int32_t *ಫಲಿತಾಂಶ_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

U_CAPI ಶೂನ್ಯ U_EXPORT2 ulist_reset_keyword_ಮೌಲ್ಯಗಳು_iterator(UEnumeration* en, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#ಆದರೆ_ಅಂತ್ಯ
