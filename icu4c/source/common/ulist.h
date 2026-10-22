// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 2009-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*/

#यदि_अपरिभाषितम् ULIST_H
#परिभाषय ULIST_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uenum.h"

संरचना UList;
प्रकारनिर्देश संरचना UList UList;

U_CAPI UList * U_EXPORT2 ulist_create_रिक्तानि_List(UError_संहिता *स्थितिः);

U_CAPI शून्य U_EXPORT2 ulist_सङ्कलय_Item_अन्तः_List(UList *list, ध्रुव शून्य *दत्त, UBool forceDelete, UError_संहिता *स्थितिः);

U_CAPI शून्य U_EXPORT2 ulist_सङ्कलय_Item_Begin_List(UList *list, ध्रुव शून्य *दत्त, UBool forceDelete, UError_संहिता *स्थितिः);

U_CAPI UBool U_EXPORT2 ulist_अन्तर्भवति_String(ध्रुव UList *list, ध्रुव वर्ण *दत्त, int32_t दीर्घता);

U_CAPI UBool U_EXPORT2 ulist_removeString(UList *list, ध्रुव वर्ण *दत्त);

U_CAPI शून्य *U_EXPORT2 ulist_get_अग्र(UList *list);

U_CAPI int32_t U_EXPORT2 ulist_get_List_आकार(ध्रुव UList *list);

U_CAPI शून्य U_EXPORT2 ulist_resetList(UList *list);

U_CAPI शून्य U_EXPORT2 ulist_deleteList(UList *list);

/*
 * The following are for use when creating UEnumeration object backed by UList.
 */
U_CAPI शून्य U_EXPORT2 ulist_close_keyword_मानानि_iterator(UEnumeration *en);

U_CAPI int32_t U_EXPORT2 ulist_गणनम्_keyword_मानानि(UEnumeration *en, UError_संहिता *स्थितिः);

U_CAPI ध्रुव वर्ण * U_EXPORT2 ulist_अग्र_keyword_मानम्(UEnumeration* en, int32_t *परिणाम_दीर्घता, UError_संहिता* स्थितिः);

U_CAPI शून्य U_EXPORT2 ulist_reset_keyword_मानानि_iterator(UEnumeration* en, UError_संहिता* स्थितिः);

U_CAPI UList * U_EXPORT2 ulist_getListFromEnum(UEnumeration *en);

#यदि_अन्तः
