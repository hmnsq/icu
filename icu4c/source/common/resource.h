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

#جيڪڏهن_وصف_نه_ٿيل __URESOURCE_H__
#وصف_ڪريو __URESOURCE_H__

/**
 * \file
 * \brief ICU resource bundle key and value types.
 */

// Note: Ported from ICU4J class UResource and its nested classes,
// but the C++ classes are separate, not nested.

// We use the Resource prefix for C++ classes, as usual.
// The UResource prefix would be used for C types.

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/ures.h"
#ضمن "restrace.h"

بنية Resource_معلومات;

U_NAMESPACE_BEGIN

class Resource_قدر;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
عام:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), ڊيگهائي(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(ثابت uint16_t *i16, ثابت uint32_t *i32, int32_t len,
                  ثابت ResourceTracer& traceInfo) :
            items16(i16), items32(i32), ڊيگهائي(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_سائيز() ثابت { أرجع ڊيگهائي; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_قدر(int32_t i, Resource_قدر &قدر_2) ثابت;

    /** Only for implementation use. @internal */
    uint32_t اندروني_Get_Resource(ثابت Resource_معلومات *p_Res_معلومات, int32_t i) ثابت;

خاص:
    ثابت uint16_t *items16;
    ثابت uint32_t *items32;
    int32_t ڊيگهائي;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API Resource_جدول {
عام:
    /** Constructs an empty table object. */
    Resource_جدول() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), ڊيگهائي(0) {}

    /** Only for implementation use. @internal */
    Resource_جدول(ثابت uint16_t *k16, ثابت int32_t *k32,
                  ثابت uint16_t *i16, ثابت uint32_t *i32, int32_t len,
                  ثابت ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), ڊيگهائي(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_سائيز() ثابت { أرجع ڊيگهائي; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_ڪنجي_And_قدر(int32_t i, ثابت حرف *&ڪنجي, Resource_قدر &قدر_2) ثابت;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool ڳوليو_قدر(ثابت حرف *ڪنجي, Resource_قدر &قدر_2) ثابت;

خاص:
    ثابت uint16_t *keys16;
    ثابت int32_t *keys32;
    ثابت uint16_t *items16;
    ثابت uint32_t *items32;
    int32_t ڊيگهائي;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_قدر : عام UObject {
عام:
    مجازي ~Resource_قدر();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    مجازي UResType getType() ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    مجازي ثابت char16_t *getString(int32_t &ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    مضمن UnicodeString getUnicodeString(UError_ڪوڊ &error_ڪوڊ) ثابت {
        int32_t len = 0;
        ثابت char16_t *r = getString(len, error_ڪوڊ);
        أرجع UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    مجازي ثابت char16_t *getAliasString(int32_t &ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    مضمن UnicodeString getAliasUnicodeString(UError_ڪوڊ &error_ڪوڊ) ثابت {
        int32_t len = 0;
        ثابت char16_t *r = getAliasString(len, error_ڪوڊ);
        أرجع UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    مجازي int32_t getInt(UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    مجازي uint32_t getUInt(UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    مجازي ثابت int32_t *get_Int_ويڪٽر(int32_t &ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    مجازي ثابت uint8_t *get_ٻٽو(int32_t &ڊيگهائي, UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    مجازي ResourceArray getArray(UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    مجازي Resource_جدول get_جدول(UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    مجازي UBool isNoInheritanceMarker() ثابت = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    مجازي int32_t getStringArray(UnicodeString *dest, int32_t صلاحيت,
                                   UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

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
    مجازي int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t صلاحيت,
                                                  UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

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
    مجازي UnicodeString get_String_Or_پهريون_جو_Array(UError_ڪوڊ &error_ڪوڊ) ثابت = 0;

محمي:
    Resource_قدر() {}

خاص:
    Resource_قدر(ثابت Resource_قدر &);  // no copy constructor
    Resource_قدر &عامل=(ثابت Resource_قدر &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : عام UObject {
عام:
    ResourceSink() {}
    مجازي ~ResourceSink();

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
    مجازي فراغ put(ثابت حرف *ڪنجي, Resource_قدر &قدر_2, UBool noFallback,
                     UError_ڪوڊ &error_ڪوڊ) = 0;

خاص:
    ResourceSink(ثابت ResourceSink &) = delete;  // no copy constructor
    ResourceSink &عامل=(ثابت ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
