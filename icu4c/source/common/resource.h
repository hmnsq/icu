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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __URESOURCE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __URESOURCE_H__

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

ರಚನೆ Resource_ದತ್ತಾಂಶ;

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
    ResourceArray() : items16(nullptr), items32(nullptr), ಉದ್ದಳತೆ(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(ಸ್ಥಿರಾಂಕ uint16_t *i16, ಸ್ಥಿರಾಂಕ uint32_t *i32, int32_t len,
                  ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo) :
            items16(i16), items32(i32), ಉದ್ದಳತೆ(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ಮೌಲ್ಯ(int32_t i, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;

    /** Only for implementation use. @internal */
    uint32_t ಆಂತರಿಕ_Get_Resource(ಸ್ಥಿರಾಂಕ Resource_ದತ್ತಾಂಶ *p_Res_ದತ್ತಾಂಶ, int32_t i) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ uint16_t *items16;
    ಸ್ಥಿರಾಂಕ uint32_t *items32;
    int32_t ಉದ್ದಳತೆ;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API Resource_ಕೋಷ್ಟಕ {
ಸಾರ್ವಜನಿಕ:
    /** Constructs an empty table object. */
    Resource_ಕೋಷ್ಟಕ() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), ಉದ್ದಳತೆ(0) {}

    /** Only for implementation use. @internal */
    Resource_ಕೋಷ್ಟಕ(ಸ್ಥಿರಾಂಕ uint16_t *k16, ಸ್ಥಿರಾಂಕ int32_t *k32,
                  ಸ್ಥಿರಾಂಕ uint16_t *i16, ಸ್ಥಿರಾಂಕ uint32_t *i32, int32_t len,
                  ಸ್ಥಿರಾಂಕ ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), ಉದ್ದಳತೆ(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ಕೀಲಿ_And_ಮೌಲ್ಯ(int32_t i, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *&ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ uint16_t *keys16;
    ಸ್ಥಿರಾಂಕ int32_t *keys32;
    ಸ್ಥಿರಾಂಕ uint16_t *items16;
    ಸ್ಥಿರಾಂಕ uint32_t *items32;
    int32_t ಉದ್ದಳತೆ;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_ಮೌಲ್ಯ : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ ~Resource_ಮೌಲ್ಯ();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    ವಾಸ್ತವಿಕ UResType getType() ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t *getString(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    ಸಾಲೊಳಗೆ UnicodeString getUnicodeString(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
        int32_t len = 0;
        ಸ್ಥಿರಾಂಕ char16_t *r = getString(len, error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ char16_t *getAliasString(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    ಸಾಲೊಳಗೆ UnicodeString getAliasUnicodeString(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
        int32_t len = 0;
        ಸ್ಥಿರಾಂಕ char16_t *r = getAliasString(len, error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    ವಾಸ್ತವಿಕ int32_t getInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    ವಾಸ್ತವಿಕ uint32_t getUInt(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ int32_t *get_Int_ಸದಿಶ(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ uint8_t *get_ದ್ವಿಮಾನ(int32_t &ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    ವಾಸ್ತವಿಕ ResourceArray getArray(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    ವಾಸ್ತವಿಕ Resource_ಕೋಷ್ಟಕ get_ಕೋಷ್ಟಕ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    ವಾಸ್ತವಿಕ UBool isNoInheritanceMarker() ಸ್ಥಿರಾಂಕ = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    ವಾಸ್ತವಿಕ int32_t getStringArray(UnicodeString *dest, int32_t ಸಾಮರ್ಥ್ಯ,
                                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

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
    ವಾಸ್ತವಿಕ int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t ಸಾಮರ್ಥ್ಯ,
                                                  UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

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
    ವಾಸ್ತವಿಕ UnicodeString get_String_Or_ಮೊದಲ_ದ_Array(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ = 0;

ಸಂರಕ್ಷಿತ:
    Resource_ಮೌಲ್ಯ() {}

ಖಾಸಗಿ:
    Resource_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ Resource_ಮೌಲ್ಯ &);  // no copy constructor
    Resource_ಮೌಲ್ಯ &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Resource_ಮೌಲ್ಯ &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : ಸಾರ್ವಜನಿಕ UObject {
ಸಾರ್ವಜನಿಕ:
    ResourceSink() {}
    ವಾಸ್ತವಿಕ ~ResourceSink();

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
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ put(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &ಮೌಲ್ಯ_2, UBool noFallback,
                     UError_ಸಂಕೇತ &error_ಸಂಕೇತ) = 0;

ಖಾಸಗಿ:
    ResourceSink(ಸ್ಥಿರಾಂಕ ResourceSink &) = delete;  // no copy constructor
    ResourceSink &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
