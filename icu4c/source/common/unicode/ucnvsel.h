// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2008-2011, International Business Machines
*   Corporation, Google and others.  All Rights Reserved.
*
*******************************************************************************
*/
/*
 * Author : eldawy@google.com (Mohamed Eldawy)
 * ucnvsel.h
 *
 * Purpose: To generate a list of encodings capable of handling
 * a given Unicode text
 *
 * Started 09-April-2008
 */

#अगर_अघोषित __ICU_UCNV_SEL_H__
#घोषणा __ICU_UCNV_SEL_H__

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION

#समावेश "unicode/uset.h"
#समावेश "unicode/utf16.h"
#समावेश "unicode/uenum.h"
#समावेश "unicode/ucnv.h"

#अगर U_SHOW_CPLUSPLUS_API
#समावेश "unicode/localpointer.h"
#पूर्ण_अगर   // U_SHOW_CPLUSPLUS_API

/**
 * \file
 * \brief C API: Encoding/charset encoding selector
 *
 * A converter selector is built with a set of encoding/charset names
 * and given an input string returns the set of names of the
 * corresponding converters which can convert the string.
 *
 * A converter selector can be serialized into a buffer and reopened
 * from the serialized form.
 */

काष्ठा UConverterSelector;
/**
 * @{
 * Typedef for selector data structure.
 */
प्रकार काष्ठा UConverterSelector UConverterSelector;
/** @} */

/**
 * Open a selector.
 * If converterListSize is 0, build for all available converters.
 * If excludedCodePoints is NULL, don't exclude any code points.
 *
 * @param converterList a pointer to encoding names needed to be involved. 
 *                      Can be NULL if converterListSize==0.
 *                      The list and the names will be cloned, and the caller
 *                      retains ownership of the original.
 * @param converterListSize number of encodings in above list.
 *                          If 0, builds a selector for all available converters.
 * @param excludedCodePoints a set of code points to be excluded from consideration.
 *                           That is, excluded code points in a string do not change
 *                           the selection result. (They might be handled by a callback.)
 *                           Use NULL to exclude nothing.
 * @param whichSet what converter set to use? Use this to determine whether
 *                 to consider only roundtrip mappings or also fallbacks.
 * @param status an in/out ICU UErrorCode
 * @return the new selector
 *
 * @stable ICU 4.2
 */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_open(स्थिर अक्षर* स्थिर*  converterList, int32_t converter_List_आकार,
             स्थिर USet* excludedCodePoints,
             स्थिर UConverterUnicodeSet whichSet, UErrorCode* status);

/**
 * Closes a selector.
 * If any Enumerations were returned by ucnv_select*, they become invalid.
 * They can be closed before or after calling ucnv_closeSelector,
 * but should never be used after the selector is closed.
 *
 * @see ucnv_selectForString
 * @see ucnv_selectForUTF8
 *
 * @param sel selector to close
 *
 * @stable ICU 4.2
 */
U_CAPI व्योम U_EXPORT2
ucnvsel_close(UConverterSelector *sel);

#अगर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

/**
 * \class LocalUConverterSelectorPointer
 * "Smart pointer" class, closes a UConverterSelector via ucnvsel_close().
 * For most methods see the LocalPointerBase base class.
 *
 * @see LocalPointerBase
 * @see LocalPointer
 * @stable ICU 4.4
 */
U_DEFINE_LOCAL_OPEN_POINTER(LocalUConverterSelectorPointer, UConverterSelector, ucnvsel_close);

U_NAMESPACE_END

#पूर्ण_अगर

/**
 * Open a selector from its serialized form.
 * The buffer must remain valid and unchanged for the lifetime of the selector.
 * This is much faster than creating a selector from scratch.
 * Using a serialized form from a different machine (endianness/charset) is supported.
 *
 * @param buffer pointer to the serialized form of a converter selector;
 *               must be 32-bit-aligned
 * @param length the capacity of this buffer (can be equal to or larger than
 *               the actual data length)
 * @param status an in/out ICU UErrorCode
 * @return the new selector
 *
 * @stable ICU 4.2
 */
U_CAPI UConverterSelector* U_EXPORT2
ucnvsel_openFromSerialized(स्थिर व्योम* बफर U_LIFETIME_BOUND, int32_t लंबाई, UErrorCode* status);

/**
 * Serialize a selector into a linear buffer.
 * The serialized form is portable to different machines.
 *
 * @param sel selector to consider
 * @param buffer pointer to 32-bit-aligned memory to be filled with the
 *               serialized form of this converter selector
 * @param bufferCapacity the capacity of this buffer
 * @param status an in/out ICU UErrorCode
 * @return the required buffer capacity to hold serialize data (even if the call fails
 *         with a U_BUFFER_OVERFLOW_ERROR, it will return the required capacity)
 *
 * @stable ICU 4.2
 */
U_CAPI int32_t U_EXPORT2
ucnvsel_serialize(स्थिर UConverterSelector* sel,
                  व्योम* बफर, int32_t बफर_Capacity, UErrorCode* status);

/**
 * Select converters that can map all characters in a UTF-16 string,
 * ignoring the excluded code points.
 *
 * @param sel a selector
 * @param s UTF-16 string
 * @param length length of the string, or -1 if NUL-terminated
 * @param status an in/out ICU UErrorCode
 * @return an enumeration containing encoding names.
 *         The returned encoding names and their order will be the same as
 *         supplied when building the selector.
 *
 * @stable ICU 4.2
 */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForString(स्थिर UConverterSelector* sel U_LIFETIME_BOUND,
                        स्थिर UChar* s,
                        int32_t लंबाई,
                        UErrorCode* status);

/**
 * Select converters that can map all characters in a UTF-8 string,
 * ignoring the excluded code points.
 *
 * @param sel a selector
 * @param s UTF-8 string
 * @param length length of the string, or -1 if NUL-terminated
 * @param status an in/out ICU UErrorCode
 * @return an enumeration containing encoding names.
 *         The returned encoding names and their order will be the same as
 *         supplied when building the selector.
 *
 * @stable ICU 4.2
 */
U_CAPI UEnumeration* U_EXPORT2
ucnvsel_selectForUTF8(स्थिर UConverterSelector* sel U_LIFETIME_BOUND,
                      स्थिर अक्षर* s,
                      int32_t लंबाई,
                      UErrorCode* status);

#पूर्ण_अगर  /* !UCONFIG_NO_CONVERSION */

#पूर्ण_अगर  /* __ICU_UCNV_SEL_H__ */
