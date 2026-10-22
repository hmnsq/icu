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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/ures.h"
#અન્તર્ભાવ "restrace.h"

સંરચના ResourceData;

U_NAMESPACE_BEGIN

class Resource_કિંમત;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
જાહેર:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), length(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(અચલ uint16_t *i16, અચલ uint32_t *i32, int32_t len,
                  અચલ ResourceTracer& traceInfo) :
            items16(i16), items32(i32), length(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t getSize() અચલ { પાછા length; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_કિંમત(int32_t i, Resource_કિંમત &કિંમત_2) અચલ;

    /** Only for implementation use. @internal */
    uint32_t internalGetResource(અચલ ResourceData *pResData, int32_t i) અચલ;

ખાનગી:
    અચલ uint16_t *items16;
    અચલ uint32_t *items32;
    int32_t length;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API ResourceTable {
જાહેર:
    /** Constructs an empty table object. */
    ResourceTable() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), length(0) {}

    /** Only for implementation use. @internal */
    ResourceTable(અચલ uint16_t *k16, અચલ int32_t *k32,
                  અચલ uint16_t *i16, અચલ uint32_t *i32, int32_t len,
                  અચલ ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), length(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t getSize() અચલ { પાછા length; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ચાવી_And_કિંમત(int32_t i, અચલ અક્ષર *&ચાવી, Resource_કિંમત &કિંમત_2) અચલ;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool find_કિંમત(અચલ અક્ષર *ચાવી, Resource_કિંમત &કિંમત_2) અચલ;

ખાનગી:
    અચલ uint16_t *keys16;
    અચલ int32_t *keys32;
    અચલ uint16_t *items16;
    અચલ uint32_t *items32;
    int32_t length;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_કિંમત : જાહેર UObject {
જાહેર:
    આભાસી ~Resource_કિંમત();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    આભાસી UResType getType() અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    આભાસી અચલ char16_t *getString(int32_t &length, UErrorCode &errorCode) અચલ = 0;

    અંતઃરેખ UnicodeString getUnicodeString(UErrorCode &errorCode) અચલ {
        int32_t len = 0;
        અચલ char16_t *r = getString(len, errorCode);
        પાછા UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    આભાસી અચલ char16_t *getAliasString(int32_t &length, UErrorCode &errorCode) અચલ = 0;

    અંતઃરેખ UnicodeString getAliasUnicodeString(UErrorCode &errorCode) અચલ {
        int32_t len = 0;
        અચલ char16_t *r = getAliasString(len, errorCode);
        પાછા UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    આભાસી int32_t getInt(UErrorCode &errorCode) અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    આભાસી uint32_t getUInt(UErrorCode &errorCode) અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    આભાસી અચલ int32_t *getIntVector(int32_t &length, UErrorCode &errorCode) અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    આભાસી અચલ uint8_t *get_દ્વિઅંકી(int32_t &length, UErrorCode &errorCode) અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    આભાસી ResourceArray getArray(UErrorCode &errorCode) અચલ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    આભાસી ResourceTable getTable(UErrorCode &errorCode) અચલ = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    આભાસી UBool isNoInheritanceMarker() અચલ = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    આભાસી int32_t getStringArray(UnicodeString *dest, int32_t capacity,
                                   UErrorCode &errorCode) અચલ = 0;

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
    આભાસી int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t capacity,
                                                  UErrorCode &errorCode) અચલ = 0;

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
    આભાસી UnicodeString getStringOrFirstOfArray(UErrorCode &errorCode) અચલ = 0;

સુરક્ષિત:
    Resource_કિંમત() {}

ખાનગી:
    Resource_કિંમત(અચલ Resource_કિંમત &);  // no copy constructor
    Resource_કિંમત &operator=(અચલ Resource_કિંમત &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : જાહેર UObject {
જાહેર:
    ResourceSink() {}
    આભાસી ~ResourceSink();

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
    આભાસી શૂન્ય put(અચલ અક્ષર *ચાવી, Resource_કિંમત &કિંમત_2, UBool noFallback,
                     UErrorCode &errorCode) = 0;

ખાનગી:
    ResourceSink(અચલ ResourceSink &) = delete;  // no copy constructor
    ResourceSink &operator=(અચલ ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_END

#endif
