// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*   Copyright (C) 1997-2010, International Business Machines
*   Corporation and others.  All Rights Reserved.
******************************************************************************
*   Date        Name        Description
*   06/23/00    aliu        Creation.
******************************************************************************
*/

#నిర్వచితం_కాకపోతే __UREP_H
#నిర్వచించు __UREP_H

#అన్తర్భావ "unicode/utypes.h"

U_CDECL_BEGIN

/********************************************************************
 * General Notes
 ********************************************************************
 * TODO
 * Add usage scenario
 * Add test code
 * Talk about pinning
 * Talk about "can truncate result if out of memory"
 */

/********************************************************************
 * Data Structures
 ********************************************************************/
/**
 * \file
 * \brief C API: Callbacks for UReplaceable
 */
/**
 * An opaque replaceable text object.  This will be manipulated only
 * through the caller-supplied UReplaceableFunctor struct.  Related
 * to the C++ class Replaceable.
 * This is currently only used in the Transliterator C API, see utrans.h .
 * @stable ICU 2.0
 */
రకనిర్వచనం శూన్య* UReplaceable;

/**
 * A set of function pointers that transliterators use to manipulate a
 * UReplaceable.  The caller should supply the required functions to
 * manipulate their text appropriately.  Related to the C++ class
 * Replaceable.
 * @stable ICU 2.0
 */
రకనిర్వచనం నిర్మాణం UReplaceableCallbacks {

    /**
     * Function pointer that returns the number of UChar code units in
     * this text.
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @return The length of the text.
     * @stable ICU 2.0
     */
    int32_t (*నిడివి)(స్థిరరాశి UReplaceable* rep);

    /**
     * Function pointer that returns a UChar code units at the given
     * offset into this text; 0 <= offset < n, where n is the value
     * returned by (*length)(rep).  See unistr.h for a description of
     * charAt() vs. char32At().
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @param offset The index at which to fetch the UChar (code unit).
     * @return The UChar (code unit) at offset, or U+FFFF if the offset is out of bounds.
     * @stable ICU 2.0
     */
    UChar   (*charAt)(స్థిరరాశి UReplaceable* rep,
                      int32_t స్థానభ్రంశం);

    /**
     * Function pointer that returns a UChar32 code point at the given
     * offset into this text.  See unistr.h for a description of
     * charAt() vs. char32At().
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @param offset The index at which to fetch the UChar32 (code point).
     * @return The UChar32 (code point) at offset, or U+FFFF if the offset is out of bounds.
     * @stable ICU 2.0
     */
    UChar32 (*char32At)(స్థిరరాశి UReplaceable* rep,
                        int32_t స్థానభ్రంశం);
    
    /**
     * Function pointer that replaces text between start and limit in
     * this text with the given text.  Attributes (out of band info)
     * should be retained.
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @param start the starting index of the text to be replaced,
     * inclusive.
     * @param limit the ending index of the text to be replaced,
     * exclusive.
     * @param text the new text to replace the UChars from
     * start..limit-1.
     * @param textLength the number of UChars at text, or -1 if text
     * is null-terminated.
     * @stable ICU 2.0
     */
    శూన్య    (*replace)(UReplaceable* rep,
                       int32_t ప్రారంభం,
                       int32_t హద్దు,
                       స్థిరరాశి UChar* పాఠ్యం,
                       int32_t పాఠ్యం_నిడివి);
    
    /**
     * Function pointer that copies the characters in the range
     * [<tt>start</tt>, <tt>limit</tt>) into the array <tt>dst</tt>.
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @param start offset of first character which will be copied
     * into the array
     * @param limit offset immediately following the last character to
     * be copied
     * @param dst array in which to copy characters.  The length of
     * <tt>dst</tt> must be at least <tt>(limit - start)</tt>.
     * @stable ICU 2.1
     */
    శూన్య    (*వెలికితీయి)(UReplaceable* rep,
                       int32_t ప్రారంభం,
                       int32_t హద్దు,
                       UChar* dst);

    /**
     * Function pointer that copies text between start and limit in
     * this text to another index in the text.  Attributes (out of
     * band info) should be retained.  After this call, there will be
     * (at least) two copies of the characters originally located at
     * start..limit-1.
     *
     * @param rep A pointer to "this" UReplaceable object.
     * @param start the starting index of the text to be copied,
     * inclusive.
     * @param limit the ending index of the text to be copied,
     * exclusive.
     * @param dest the index at which the copy of the UChars should be
     * inserted.
     * @stable ICU 2.0
     */
    శూన్య    (*copy)(UReplaceable* rep,
                    int32_t ప్రారంభం,
                    int32_t హద్దు,
                    int32_t dest);    

} UReplaceableCallbacks;

U_CDECL_చివర

#అయితే_ముగింపు
