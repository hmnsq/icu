// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 1997-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   03/22/00    aliu        Creation.
*   07/06/01    aliu        Modified to support int32_t keys on
*                           platforms with sizeof(void*) < 32.
******************************************************************************
*/

#অন্তর্ভাৱ "hash.h"

/**
 * Deleter for Hashtable objects.
 */
U_CAPI শূন্য U_EXPORT2
uhash_deleteHashtable(শূন্য *obj) {
    U_NAMESPACE_USE
    delete (Hashtable*) obj;
}

//eof
