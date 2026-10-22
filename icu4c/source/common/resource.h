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

#నిర్వచితం_కాకపోతే __URESOURCE_H__
#నిర్వచించు __URESOURCE_H__

/**
 * \file
 * \brief ICU resource bundle key and value types.
 */

// Note: Ported from ICU4J class UResource and its nested classes,
// but the C++ classes are separate, not nested.

// We use the Resource prefix for C++ classes, as usual.
// The UResource prefix would be used for C types.

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "restrace.h"

నిర్మాణం Resource_దత్తాంశం;

U_NAMESPACE_BEGIN

class Resource_విలువ;

// Note: In C++, we use const char * pointers for keys,
// rather than an abstraction like Java UResource.Key.

/**
 * Interface for iterating over a resource bundle array resource.
 */
class U_COMMON_API ResourceArray {
బహిరంగ:
    /** Constructs an empty array object. */
    ResourceArray() : items16(nullptr), items32(nullptr), నిడివి(0) {}

    /** Only for implementation use. @internal */
    ResourceArray(స్థిరరాశి uint16_t *i16, స్థిరరాశి uint32_t *i32, int32_t len,
                  స్థిరరాశి ResourceTracer& traceInfo) :
            items16(i16), items32(i32), నిడివి(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_కొలత() స్థిరరాశి { తిరిగి నిడివి; }
    /**
     * @param i Array item index.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_విలువ(int32_t i, Resource_విలువ &విలువ_2) స్థిరరాశి;

    /** Only for implementation use. @internal */
    uint32_t అంతర్గత_Get_Resource(స్థిరరాశి Resource_దత్తాంశం *p_Res_దత్తాంశం, int32_t i) స్థిరరాశి;

అంతరంగ:
    స్థిరరాశి uint16_t *items16;
    స్థిరరాశి uint32_t *items32;
    int32_t నిడివి;
    ResourceTracer fTraceInfo;
};

/**
 * Interface for iterating over a resource bundle table resource.
 */
class U_COMMON_API Resource_పట్టిక {
బహిరంగ:
    /** Constructs an empty table object. */
    Resource_పట్టిక() : keys16(nullptr), keys32(nullptr), items16(nullptr), items32(nullptr), నిడివి(0) {}

    /** Only for implementation use. @internal */
    Resource_పట్టిక(స్థిరరాశి uint16_t *k16, స్థిరరాశి int32_t *k32,
                  స్థిరరాశి uint16_t *i16, స్థిరరాశి uint32_t *i32, int32_t len,
                  స్థిరరాశి ResourceTracer& traceInfo) :
            keys16(k16), keys32(k32), items16(i16), items32(i32), నిడివి(len),
            fTraceInfo(traceInfo) {}

    /**
     * @return The number of items in the array resource.
     */
    int32_t get_కొలత() స్థిరరాశి { తిరిగి నిడివి; }
    /**
     * @param i Table item index.
     * @param key Output-only, receives the key of the i'th item.
     * @param value Output-only, receives the value of the i'th item.
     * @return true if i is non-negative and less than getSize().
     */
    UBool get_కీలకం_And_విలువ(int32_t i, స్థిరరాశి అక్షరం *&కీలకం, Resource_విలువ &విలువ_2) స్థిరరాశి;

    /**
     * @param key Key string to find in the table.
     * @param value Output-only, receives the value of the item with that key.
     * @return true if the table contains the key.
     */
    UBool కనుగొను_విలువ(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &విలువ_2) స్థిరరాశి;

అంతరంగ:
    స్థిరరాశి uint16_t *keys16;
    స్థిరరాశి int32_t *keys32;
    స్థిరరాశి uint16_t *items16;
    స్థిరరాశి uint32_t *items32;
    int32_t నిడివి;
    ResourceTracer fTraceInfo;
};

/**
 * Represents a resource bundle item's value.
 * Avoids object creations as much as possible.
 * Mutable, not thread-safe.
 */
class U_COMMON_API Resource_విలువ : బహిరంగ UObject {
బహిరంగ:
    మాయా ~Resource_విలువ();

    /**
     * @return ICU resource type, for example, URES_STRING
     */
    మాయా UResType getType() స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a string resource.
     *
     * @see ures_getString()
     */
    మాయా స్థిరరాశి char16_t *getString(int32_t &నిడివి, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    అంతఃవరుస UnicodeString getUnicodeString(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
        int32_t len = 0;
        స్థిరరాశి char16_t *r = getString(len, error_సంకేతం);
        తిరిగి UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an alias resource.
     */
    మాయా స్థిరరాశి char16_t *getAliasString(int32_t &నిడివి, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    అంతఃవరుస UnicodeString getAliasUnicodeString(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
        int32_t len = 0;
        స్థిరరాశి char16_t *r = getAliasString(len, error_సంకేతం);
        తిరిగి UnicodeString(true, r, len);
    }

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getInt()
     */
    మాయా int32_t getInt(UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an integer resource.
     *
     * @see ures_getUInt()
     */
    మాయా uint32_t getUInt(UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an intvector resource.
     *
     * @see ures_getIntVector()
     */
    మాయా స్థిరరాశి int32_t *get_Int_సదిశ(int32_t &నిడివి, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a binary-blob resource.
     *
     * @see ures_getBinary()
     */
    మాయా స్థిరరాశి uint8_t *get_ద్విమాన(int32_t &నిడివి, UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     */
    మాయా ResourceArray getArray(UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not a table resource
     */
    మాయా Resource_పట్టిక get_పట్టిక(UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

    /**
     * Is this a no-fallback/no-inheritance marker string?
     * Such a marker is used for
     * CLDR no-fallback data values of (three empty-set symbols)=={2205, 2205, 2205}
     * when enumerating tables with fallback from the specific resource bundle to root.
     *
     * @return true if this is a no-inheritance marker string
     */
    మాయా UBool isNoInheritanceMarker() స్థిరరాశి = 0;

    /**
     * Sets the dest strings from the string values in this array resource.
     *
     * @return the number of strings in this array resource.
     *     If greater than capacity, then an overflow error is set.
     *
     * Sets U_RESOURCE_TYPE_MISMATCH if this is not an array resource
     *     or if any of the array items is not a string
     */
    మాయా int32_t getStringArray(UnicodeString *dest, int32_t సామర్థ్యం,
                                   UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

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
    మాయా int32_t getStringArrayOrStringAsArray(UnicodeString *dest, int32_t సామర్థ్యం,
                                                  UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

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
    మాయా UnicodeString get_String_Or_మొదటి_యొక్క_Array(UError_సంకేతం &error_సంకేతం) స్థిరరాశి = 0;

రక్షిత:
    Resource_విలువ() {}

అంతరంగ:
    Resource_విలువ(స్థిరరాశి Resource_విలువ &);  // no copy constructor
    Resource_విలువ &క్రియాకారకం=(స్థిరరాశి Resource_విలువ &);  // no assignment operator
};

/**
 * Sink for ICU resource bundle contents.
 */
class U_COMMON_API ResourceSink : బహిరంగ UObject {
బహిరంగ:
    ResourceSink() {}
    మాయా ~ResourceSink();

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
    మాయా శూన్య put(స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &విలువ_2, UBool noFallback,
                     UError_సంకేతం &error_సంకేతం) = 0;

అంతరంగ:
    ResourceSink(స్థిరరాశి ResourceSink &) = delete;  // no copy constructor
    ResourceSink &క్రియాకారకం=(స్థిరరాశి ResourceSink &) = delete;  // no assignment operator
};

U_NAMESPACE_చివర

#అయితే_ముగింపు
