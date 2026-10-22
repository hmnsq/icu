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

#నిర్వచితం_కాకపోతే __ICU_UCNV_SEL_H__
#నిర్వచించు __ICU_UCNV_SEL_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "unicode/uenum.h"
#అన్తర్భావ "unicode/ucnv.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API
#అన్తర్భావ "unicode/localpointer.h"
#అయితే_ముగింపు   // U_SHOW_CPLUSPLUS_API

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

నిర్మాణం UConverterSelector;
/**
 * @{
 * Typedef for selector data structure.
 */
రకనిర్వచనం నిర్మాణం UConverterSelector UConverterSelector;
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
ucnvsel_open(స్థిరరాశి అక్షరం* స్థిరరాశి*  converterList, int32_t converter_List_కొలత,
             స్థిరరాశి USet* excluded_సంకేతం_Points,
             స్థిరరాశి UConverter_Unicode_సమితి which_సమితి, UError_సంకేతం* స్థితి_2);

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
U_CAPI శూన్య U_EXPORT2
ucnvsel_close(UConverterSelector *sel);

#ఒకవేళ U_SHOW_CPLUSPLUS_API

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
U_DEFINE_LOCAL_OPEN_సూచిక(Local_UConverter_Selector_సూచిక, UConverterSelector, ucnvsel_close);

U_NAMESPACE_చివర

#అయితే_ముగింపు

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
ucnvsel_openFromSerialized(స్థిరరాశి శూన్య* బఫరు U_LIFETIME_BOUND, int32_t నిడివి, UError_సంకేతం* స్థితి_2);

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
ucnvsel_serialize(స్థిరరాశి UConverterSelector* sel,
                  శూన్య* బఫరు, int32_t బఫరు_సామర్థ్యం, UError_సంకేతం* స్థితి_2);

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
ucnvsel_selectForString(స్థిరరాశి UConverterSelector* sel U_LIFETIME_BOUND,
                        స్థిరరాశి UChar* s,
                        int32_t నిడివి,
                        UError_సంకేతం* స్థితి_2);

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
ucnvsel_selectForUTF8(స్థిరరాశి UConverterSelector* sel U_LIFETIME_BOUND,
                      స్థిరరాశి అక్షరం* s,
                      int32_t నిడివి,
                      UError_సంకేతం* స్థితి_2);

#అయితే_ముగింపు  /* !UCONFIG_NO_CONVERSION */

#అయితే_ముగింపు  /* __ICU_UCNV_SEL_H__ */
