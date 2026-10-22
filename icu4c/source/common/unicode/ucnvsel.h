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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ICU_UCNV_SEL_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __ICU_UCNV_SEL_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "unicode/uenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಆದರೆ_ಅಂತ್ಯ   // U_SHOW_CPLUSPLUS_API

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

ರಚನೆ UConverterSelector;
/**
 * @{
 * Typedef for selector data structure.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UConverterSelector UConverterSelector;
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
ucnvsel_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*  converterList, int32_t converter_List_ಗಾತ್ರ,
             ಸ್ಥಿರಾಂಕ USet* excluded_ಸಂಕೇತ_Points,
             ಸ್ಥಿರಾಂಕ UConverter_Unicode_ಗಣ which_ಗಣ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

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
U_CAPI ಶೂನ್ಯ U_EXPORT2
ucnvsel_close(UConverterSelector *sel);

#ಆದರೆ U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_ಸೂಚಕ(Local_UConverter_Selector_ಸೂಚಕ, UConverterSelector, ucnvsel_close);

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ

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
ucnvsel_openFromSerialized(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* ಬಫರ್ U_LIFETIME_BOUND, int32_t ಉದ್ದಳತೆ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

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
ucnvsel_serialize(ಸ್ಥಿರಾಂಕ UConverterSelector* sel,
                  ಶೂನ್ಯ* ಬಫರ್, int32_t ಬಫರ್_ಸಾಮರ್ಥ್ಯ, UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

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
ucnvsel_selectForString(ಸ್ಥಿರಾಂಕ UConverterSelector* sel U_LIFETIME_BOUND,
                        ಸ್ಥಿರಾಂಕ UChar* s,
                        int32_t ಉದ್ದಳತೆ,
                        UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

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
ucnvsel_selectForUTF8(ಸ್ಥಿರಾಂಕ UConverterSelector* sel U_LIFETIME_BOUND,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s,
                      int32_t ಉದ್ದಳತೆ,
                      UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2);

#ಆದರೆ_ಅಂತ್ಯ  /* !UCONFIG_NO_CONVERSION */

#ಆದರೆ_ಅಂತ್ಯ  /* __ICU_UCNV_SEL_H__ */
