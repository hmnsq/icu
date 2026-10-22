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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __URESOURCE_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __URESOURCE_H__

/**
 * \file
 * \brief ICU resource bundle key and value types.
 */

// Note: Ported from ICU4J class UResource and its nested classes,
// but the C++ classes are separate, not nested.

// We use the Resource prefix for C++ classes, as usual.
// The UResource prefix would be used for C types.

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "restrace.h"

ਬਣਤਰ Resource_ਅੰਕੜੇ;

U_NAMESPACE_BEGIN

class Resource_ਮੁੱਲ;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
ਜਨਤਕ:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), ਲੰਮਾਈ(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(ਅਚਲ uint16_t *i16, ਅਚਲ uint32_t *i32, int32_t len,
                  ਅਚਲ ResourceTracer& traceInfo) :
            items16(i16), items32(i32), ਲੰਮਾਈ(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਲੰਮਾਈ; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ਮੁੱਲ(int32_t i, Resource_ਮੁੱਲ &ਮੁੱਲ_2) ਅਚਲ;

    /** Only for implementation use. @internal */
    uint32_t ਅੰਦਰੂਨੀ_Get_Resource(ਅਚਲ Resource_ਅੰਕੜੇ *p_Res_ਅੰਕੜੇ, int32_t i) ਅਚਲ;

ਨਿਜੀ:
    ਅਚਲ uint16_t *items16;
    ਅਚਲ uint32_t *items32;
    int32_t ਲੰਮਾਈ;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API Resource_ਸਾਰਣੀ {
ਜਨਤਕ:
    /** Constructs an empty table object. */
    Resource_ਸਾਰਣੀ() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), ਲੰਮਾਈ(0) {}

    /** Only for implementation use. @internal */
    Resource_ਸਾਰਣੀ(ਅਚਲ uint16_t *k16, ਅਚਲ int32_t *k32,
                  ਅਚਲ uint16_t *i16, ਅਚਲ uint32_t *i32, int32_t len,
                  ਅਚਲ ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), ਲੰਮਾਈ(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਲੰਮਾਈ; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ਕੁੰਜੀ_And_ਮੁੱਲ(int32_t i, ਅਚਲ ਅੱਖਰ *&ਕੁੰਜੀ, Resource_ਮੁੱਲ &ਮੁੱਲ_2) ਅਚਲ;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool ਲੱਭੋ_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &ਮੁੱਲ_2) ਅਚਲ;

ਨਿਜੀ:
    ਅਚਲ uint16_t *keys16;
    ਅਚਲ int32_t *keys32;
    ਅਚਲ uint16_t *items16;
    ਅਚਲ uint32_t *items32;
    int32_t ਲੰਮਾਈ;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_ਮੁੱਲ : ਜਨਤਕ UObject {
ਜਨਤਕ:
    ਆਭਾਸੀ ~Resource_ਮੁੱਲ();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    ਆਭਾਸੀ UResType getType() ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    ਆਭਾਸੀ ਅਚਲ char16_t *getString(int32_t &ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    ਸਤਰਅੰਦਰ UnicodeString getUnicodeString(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
        int32_t len = 0;
        ਅਚਲ char16_t *r = getString(len, error_ਕੋਡ);
        ਵਾਪਸ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    ਆਭਾਸੀ ਅਚਲ char16_t *getAliasString(int32_t &ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    ਸਤਰਅੰਦਰ UnicodeString getAliasUnicodeString(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
        int32_t len = 0;
        ਅਚਲ char16_t *r = getAliasString(len, error_ਕੋਡ);
        ਵਾਪਸ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    ਆਭਾਸੀ int32_t getInt(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    ਆਭਾਸੀ uint32_t getUInt(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    ਆਭਾਸੀ ਅਚਲ int32_t *get_Int_ਸਦਿਸ਼(int32_t &ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    ਆਭਾਸੀ ਅਚਲ uint8_t *get_ਦੁਆਧਾਰੀ(int32_t &ਲੰਮਾਈ, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    ਆਭਾਸੀ ResourceArray getArray(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    ਆਭਾਸੀ Resource_ਸਾਰਣੀ get_ਸਾਰਣੀ(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    ਆਭਾਸੀ UBool isNoInheritanceMarker() ਅਚਲ = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    ਆਭਾਸੀ int32_t getStringArray(UnicodeString *dest, int32_t ਸਮਰੱਥਾ,
                                   UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

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
    ਆਭਾਸੀ int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t ਸਮਰੱਥਾ,
                                                  UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

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
    ਆਭਾਸੀ UnicodeString get_String_Or_ਪਹਿਲਾ_ਦਾ_Array(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ = 0;

ਸੁਰੱਖਿਅਤ:
    Resource_ਮੁੱਲ() {}

ਨਿਜੀ:
    Resource_ਮੁੱਲ(ਅਚਲ Resource_ਮੁੱਲ &);  // no copy constructor
    Resource_ਮੁੱਲ &ਸੰਚਾਲਕ=(ਅਚਲ Resource_ਮੁੱਲ &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : ਜਨਤਕ UObject {
ਜਨਤਕ:
    ResourceSink() {}
    ਆਭਾਸੀ ~ResourceSink();

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
    ਆਭਾਸੀ ਖਾਲੀ put(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &ਮੁੱਲ_2, UBool noFallback,
                     UError_ਕੋਡ &error_ਕੋਡ) = 0;

ਨਿਜੀ:
    ResourceSink(ਅਚਲ ResourceSink &) = delete;  // no copy constructor
    ResourceSink &ਸੰਚਾਲਕ=(ਅਚਲ ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
