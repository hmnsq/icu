// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2015-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* resource.h
*
* created on: 2015nov04
* created by: Markus W. Scherer
*/

#ifndef __URESOURCE_H__
#define __URESOURCE_H__

/**
 * \file
 * \brief ICU resource bundle key and value types.
 */

// Note: Ported from ICU4J class UResource and its nested classes,
// but the C++ classes are separate, not nested.

// We use the Resource prefix for C++ classes, as usual.
// The UResource prefix would be used for C types.

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "restrace.h"

ಸಂರಚನಾ ResourceData;

U_NAMESPACE_BEGIN

class Resource_ಮೌಲ್ಯ;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
ಸಾರ್ವಜನಿಕ:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), length(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(ಧ್ರುವ uint16_t *i16, ಧ್ರುವ uint32_t *i32, int32_t len,
                  ಧ್ರುವ ResourceTracer& traceInfo) :
            items16(i16), items32(i32), length(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t getSize() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ length; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ಮೌಲ್ಯ(int32_t i, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಧ್ರುವ;

    /** Only for implementation use. @internal */
    uint32_t internalGetResource(ಧ್ರುವ ResourceData *pResData, int32_t i) ಧ್ರುವ;

ಗುಪ್ತ:
    ಧ್ರುವ uint16_t *items16;
    ಧ್ರುವ uint32_t *items32;
    int32_t length;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API ResourceTable {
ಸಾರ್ವಜನಿಕ:
    /** Constructs an empty table object. */
    ResourceTable() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), length(0) {}

    /** Only for implementation use. @internal */
    ResourceTable(ಧ್ರುವ uint16_t *k16, ಧ್ರುವ int32_t *k32,
                  ಧ್ರುವ uint16_t *i16, ಧ್ರುವ uint32_t *i32, int32_t len,
                  ಧ್ರುವ ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), length(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t getSize() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ length; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ಕೀಲಿ_And_ಮೌಲ್ಯ(int32_t i, ಧ್ರುವ ವರ್ಣ *&ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಧ್ರುವ;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool find_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಧ್ರುವ;

ಗುಪ್ತ:
    ಧ್ರುವ uint16_t *keys16;
    ಧ್ರುವ int32_t *keys32;
    ಧ್ರುವ uint16_t *items16;
    ಧ್ರುವ uint32_t *items32;
    int32_t length;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    ಆಭಾಸ ~Resource_ಮೌಲ್ಯ();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    ಆಭಾಸ UResType getType() ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    ಆಭಾಸ ಧ್ರುವ char16_t *getString(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ = 0;

    ಅನ್ತಃಸ್ಥ UnicodeString getUnicodeString(UErrorCode &errorCode) ಧ್ರುವ {
        int32_t len = 0;
        ಧ್ರುವ char16_t *r = getString(len, errorCode);
        ಪ್ರತ್ಯಾಗಮ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    ಆಭಾಸ ಧ್ರುವ char16_t *getAliasString(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ = 0;

    ಅನ್ತಃಸ್ಥ UnicodeString getAliasUnicodeString(UErrorCode &errorCode) ಧ್ರುವ {
        int32_t len = 0;
        ಧ್ರುವ char16_t *r = getAliasString(len, errorCode);
        ಪ್ರತ್ಯಾಗಮ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    ಆಭಾಸ int32_t getInt(UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    ಆಭಾಸ uint32_t getUInt(UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    ಆಭಾಸ ಧ್ರುವ int32_t *getIntVector(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    ಆಭಾಸ ಧ್ರುವ uint8_t *get_ದ್ವಿಮಾನ(int32_t &length, UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    ಆಭಾಸ ResourceArray getArray(UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    ಆಭಾಸ ResourceTable getTable(UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    ಆಭಾಸ UBool isNoInheritanceMarker() ಧ್ರುವ = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    ಆಭಾಸ int32_t getStringArray(UnicodeString *dest, int32_t capacity,
                                   UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Same as
     * <pre>
     * if (getType() == URES_STRING) {
     *     return new String[] { getString(); }
     * } else {
     *     return getStringArray();
     * }
     * </pre>
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is
     *     neither a string resource nor an array resource containing strings
     * @see getString()
     * @see getStringArray()
     */
    ಆಭಾಸ int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                  UErrorCode &errorCode) ಧ್ರುವ = 0;

    /**
     * Same as
     * <pre>
     * if (getType() == URES_STRING) {
     *     return getString();
     * } else {
     *     return getStringArray()[0];
     * }
     * </pre>
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is
     *     neither a string resource nor an array resource containing strings
     * @see getString()
     * @see getStringArray()
     */
    ಆಭಾಸ UnicodeString getStringOrFirstOfArray(UErrorCode &errorCode) ಧ್ರುವ = 0;

ರಕ್ಷಿತ:
    Resource_ಮೌಲ್ಯ() {}

ಗುಪ್ತ:
    Resource_ಮೌಲ್ಯ(ಧ್ರುವ Resource_ಮೌಲ್ಯ &);  // no copy constructor
    Resource_ಮೌಲ್ಯ &operator=(ಧ್ರುವ Resource_ಮೌಲ್ಯ &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    ResourceSink() {}
    ಆಭಾಸ ~ResourceSink();

    /**
     * Called once for each bundle (child-parent-...-root).
     * The value is normally an array or table resource,
     * and implementations of this method normally iterate over the
     * tree of resource items stored there.
     *
     * @param key The key string of the enumeration-start resource.
     *     Empty if the enumeration starts at the top level of the bundle.
     * @param value Call getArray() or getTable() as appropriate. Then reuse for
     *     output values from Array and Table getters. Note: ResourceTable and
     *     ResourceArray instances must outlive the ResourceValue instance for
     *     ResourceTracer to be happy.
     * @param noFallback true if the bundle has no parent;
     *     that is, its top-level table has the nofallback attribute,
     *     or it is the root bundle of a locale tree.
     */
    ಆಭಾಸ ಶೂನ್ಯ put(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2, UBool noFallback,
                     UErrorCode &errorCode) = 0;

ಗುಪ್ತ:
    ResourceSink(ಧ್ರುವ ResourceSink &) = delete;  // no copy constructor
    ResourceSink &operator=(ಧ್ರುವ ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_END

#endif
