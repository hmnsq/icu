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

#اگر_غیر_تعریف_شدہ __URESOURCE_H__
#تعریف_کرو __URESOURCE_H__

/**
 * \file
 * \brief ICU resource bundle key and value types.
 */

// Note: Ported from ICU4J class UResource and its nested classes,
// but the C++ classes are separate, not nested.

// We use the Resource prefix for C++ classes, as usual.
// The UResource prefix would be used for C types.

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/ures.h"
#شامل "restrace.h"

ساخت Resource_ڈیٹا;

U_NAMESPACE_BEGIN

class Resource_قدر;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
عوامی:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), لمبائی(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(ثابت uint16_t *i16, ثابت uint32_t *i32, int32_t len,
                  ثابت ResourceTracer& traceInfo) :
            items16(i16), items32(i32), لمبائی(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_سائز() ثابت { واپس لمبائی; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_قدر(int32_t i, Resource_قدر &قدر_2) ثابت;

    /** Only for implementation use. @internal */
    uint32_t اندرونی_Get_Resource(ثابت Resource_ڈیٹا *p_Res_ڈیٹا, int32_t i) ثابت;

نجی:
    ثابت uint16_t *items16;
    ثابت uint32_t *items32;
    int32_t لمبائی;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API Resource_جدول {
عوامی:
    /** Constructs an empty table object. */
    Resource_جدول() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), لمبائی(0) {}

    /** Only for implementation use. @internal */
    Resource_جدول(ثابت uint16_t *k16, ثابت int32_t *k32,
                  ثابت uint16_t *i16, ثابت uint32_t *i32, int32_t len,
                  ثابت ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), لمبائی(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_سائز() ثابت { واپس لمبائی; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_کلید_And_قدر(int32_t i, ثابت حرف *&کلید, Resource_قدر &قدر_2) ثابت;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool ڈھونڈو_قدر(ثابت حرف *کلید, Resource_قدر &قدر_2) ثابت;

نجی:
    ثابت uint16_t *keys16;
    ثابت int32_t *keys32;
    ثابت uint16_t *items16;
    ثابت uint32_t *items32;
    int32_t لمبائی;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_قدر : عوامی UObject {
عوامی:
    مجازی ~Resource_قدر();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    مجازی UResType getType() ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    مجازی ثابت char16_t *getString(int32_t &لمبائی, UError_رمز &error_رمز) ثابت = 0;

    اندرونی UnicodeString getUnicodeString(UError_رمز &error_رمز) ثابت {
        int32_t len = 0;
        ثابت char16_t *r = getString(len, error_رمز);
        واپس UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    مجازی ثابت char16_t *getAliasString(int32_t &لمبائی, UError_رمز &error_رمز) ثابت = 0;

    اندرونی UnicodeString getAliasUnicodeString(UError_رمز &error_رمز) ثابت {
        int32_t len = 0;
        ثابت char16_t *r = getAliasString(len, error_رمز);
        واپس UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    مجازی int32_t getInt(UError_رمز &error_رمز) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    مجازی uint32_t getUInt(UError_رمز &error_رمز) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    مجازی ثابت int32_t *get_Int_سمتیہ(int32_t &لمبائی, UError_رمز &error_رمز) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    مجازی ثابت uint8_t *get_ثنائی(int32_t &لمبائی, UError_رمز &error_رمز) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    مجازی ResourceArray getArray(UError_رمز &error_رمز) ثابت = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    مجازی Resource_جدول get_جدول(UError_رمز &error_رمز) ثابت = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    مجازی UBool isNoInheritanceMarker() ثابت = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    مجازی int32_t getStringArray(UnicodeString *dest, int32_t استعداد,
                                   UError_رمز &error_رمز) ثابت = 0;

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
    مجازی int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t استعداد,
                                                  UError_رمز &error_رمز) ثابت = 0;

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
    مجازی UnicodeString get_String_Or_پہلا_کا_Array(UError_رمز &error_رمز) ثابت = 0;

محفوظ:
    Resource_قدر() {}

نجی:
    Resource_قدر(ثابت Resource_قدر &);  // no copy constructor
    Resource_قدر &عامل=(ثابت Resource_قدر &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : عوامی UObject {
عوامی:
    ResourceSink() {}
    مجازی ~ResourceSink();

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
    مجازی خالی put(ثابت حرف *کلید, Resource_قدر &قدر_2, UBool noFallback,
                     UError_رمز &error_رمز) = 0;

نجی:
    ResourceSink(ثابت ResourceSink &) = delete;  // no copy constructor
    ResourceSink &عامل=(ثابت ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_انتہا

#اگر_ختم
