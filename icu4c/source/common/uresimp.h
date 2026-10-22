// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#यदि_अपरिभाषितम् URESIMP_H
#परिभाषय URESIMP_H

#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/utypes.h"

#अन्तर्भाव "uresdata.h"

#परिभाषय k_मूलम्_Locale_Name         "root"
#परिभाषय kPoolBundleName         "pool"

/*
 The default minor version and the version separator must be exactly one
 character long.
*/

#परिभाषय k_Default_Minor_संस्करणम्    "0"
#परिभाषय k_संस्करणम्_Separator       "."
#परिभाषय k_संस्करणम्_चिह्नकम्             "Version"

#परिभाषय MAGIC1 19700503
#परिभाषय MAGIC2 19641227

#परिभाषय URES_MAX_ALIAS_LEVEL 256

#परिभाषय रिक्तानि_समुच्चयः 0x2205

संरचना UResource_दत्त_प्रविष्टिः;
प्रकारनिर्देश संरचना UResource_दत्त_प्रविष्टिः UResource_दत्त_प्रविष्टिः;

/*
 * Note: If we wanted to make this structure smaller, then we could try
 * to use one UResourceDataEntry pointer for fAlias and fPool, with a separate
 * flag to distinguish whether this struct is for a real bundle with a pool,
 * or for an alias entry for which we won't use the pool after loading.
 */
संरचना UResource_दत्त_प्रविष्टिः {
    वर्ण *fName; /* name of the locale for bundle - still to decide whether it is original or fallback */
    वर्ण *f_मार्गः; /* path to bundle - used for distinguishing between resources with the same name */
    UResource_दत्त_प्रविष्टिः *fParent; /*next resource in fallback chain*/
    UResource_दत्त_प्रविष्टिः *fAlias;
    UResource_दत्त_प्रविष्टिः *fPool;
    Resource_दत्त f_दत्त; /* data for low level access */
    वर्ण f_Name_बफर[3]; /* A small buffer of free space for fName. The free space is due to struct padding. */
    uint32_t f_गणनम्_Existing; /* how much is this resource used */
    UError_संहिता fBogus;
    /* int32_t fHashKey;*/ /* for faster access in the hashtable */
};

#परिभाषय RES_BUFSIZE 64
#परिभाषय RES_मार्गः_SEPARATOR   '/'
#परिभाषय RES_मार्गः_SEPARATOR_S   "/"

U_CAPI शून्य U_EXPORT2 ures_init_राशिः_Object(UResourceBundle* resB);

#यदि_परिभाषितम् __cplusplus

संरचना UResourceBundle {
    ध्रुव वर्ण *f_कुञ्चिका; /*चिह्नकम्*/
    /**
     * The dataEntry for the actual locale in which this item lives.
     * Used for accessing the item's data.
     * Non-const pointer for reference counting via entryIncrease().
     */
    UResource_दत्त_प्रविष्टिः *f_दत्त; /*for low-level access*/
    वर्ण *f_संस्करणम्;
    /**
     * The dataEntry for the valid locale.
     * Used for /LOCALE/path alias resolution that starts back from the valid locale,
     * rather than from the actual locale of this item which might live in
     * an ancestor bundle.
     */
    UResource_दत्त_प्रविष्टिः *f_Valid_Locale_दत्त_प्रविष्टिः;
    वर्ण *f_Res_मार्गः; /* full path to the resource: "zh_TW/CollationElements/Sequence" */
    वर्ण fResBuf[RES_BUFSIZE];
    int32_t f_Res_मार्गः_Len;
    Resource fRes;
    UBool fHasFallback;
    UBool f_Is_शिखरम्_Level;
    uint32_t fMagic1;   /* For determining if it's a stack object */
    uint32_t fMagic2;   /* For determining if it's a stack object */
    int32_t f_सूचकांक;
    int32_t f_आकार;

    अन्तःस्थ ध्रुव Resource_दत्त &get_Res_दत्त() ध्रुव { प्रत्यागम f_दत्त->f_दत्त; }
};

U_NAMESPACE_BEGIN

/**
 * \class StackUResourceBundle
 * "Smart pointer" like class, closes a UResourceBundle via ures_close().
 *
 * This code:
 *
 *   StackUResourceBundle bundle;
 *   foo(bundle.getAlias());
 *
 * Is equivalent to this code:
 *
 *   UResourceBundle bundle;
 *   ures_initStackObject(&bundle);
 *   foo(&bundle);
 *   ures_close(&bundle);
 *
 * @see LocalUResourceBundlePointer
 * @internal
 */
class U_COMMON_API राशिः_UResource_Bundle {
सार्वजनिक:
    // No heap allocation. Use only on the stack.
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new[](size_t) noexcept = delete;
    स्थिर शून्य* U_EXPORT2 संक्रियकः new(size_t, शून्य*) noexcept = delete;

    राशिः_UResource_Bundle();
    ~राशिः_UResource_Bundle();

    UResourceBundle* getAlias() U_LIFETIME_BOUND { प्रत्यागम &bundle; }

    UResourceBundle& ref() U_LIFETIME_BOUND { प्रत्यागम bundle; }
    ध्रुव UResourceBundle& ref() ध्रुव U_LIFETIME_BOUND { प्रत्यागम bundle; }

    राशिः_UResource_Bundle(ध्रुव राशिः_UResource_Bundle&) = delete;
    राशिः_UResource_Bundle& संक्रियकः=(ध्रुव राशिः_UResource_Bundle&) = delete;

    राशिः_UResource_Bundle(राशिः_UResource_Bundle&&) = delete;
    राशिः_UResource_Bundle& संक्रियकः=(राशिः_UResource_Bundle&&) = delete;

गुप्त:
    UResourceBundle bundle;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः  /* __cplusplus */

/**
 * Opens a resource bundle for the locale;
 * if there is not even a base language bundle, then loads the root bundle;
 * never falls back to the default locale.
 *
 * This is used for algorithms that have good pan-Unicode default behavior,
 * such as case mappings, collation, and segmentation (BreakIterator).
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ध्रुव वर्ण* मार्गः, ध्रुव वर्ण* localeID, UError_संहिता* स्थितिः);

/* Some getters used by the copy constructor */
U_CFUNC ध्रुव वर्ण* ures_getName(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND);
#यदि_परिभाषितम् URES_DEBUG
U_CFUNC ध्रुव वर्ण* ures_get_मार्गः(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND);
/**
 * If anything was in the RB cache, dump it to the screen.
 * @return true if there was anything into the cache
 */
U_CAPI UBool U_EXPORT2 ures_dumpCacheContents(शून्य);
#यदि_अन्तः

/* Candidates for export */
U_CFUNC UResourceBundle*
ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
              ध्रुव UResourceBundle* original,
              UError_संहिता* स्थितिः);

/**
 * Returns a resource that can be located using the pathToResource argument. One needs optional package, locale
 * and path inside the locale, for example: "/myData/en/zoneStrings/3". Keys and indexes are supported. Keys
 * need to reference data in named structures, while indexes can reference both named and anonymous resources.
 * Features a fill-in parameter. 
 * 
 * Note, this function does NOT have a syntax for specifying items within a tree.  May want to consider a
 * syntax that delineates between package/tree and resource.  
 *
 * @param pathToResource    a path that will lead to the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_findResource(ध्रुव वर्ण* मार्गः_गन्तव्यम्_Resource,
                  UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                  UError_संहिता* स्थितिः);

/**
 * Returns a sub resource that can be located using the pathToResource argument. One needs a path inside 
 * the supplied resource, for example, if you have "en_US" resource bundle opened, you might ask for
 * "zoneStrings/3". Keys and indexes are supported. Keys
 * need to reference data in named structures, while indexes can reference both 
 * named and anonymous resources.
 * Features a fill-in parameter. 
 *
 * @param resourceBundle    a resource
 * @param pathToResource    a path that will lead to the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status            fills in the outgoing error code.
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_findSubResource(ध्रुव UResourceBundle* resB,
                     वर्ण* मार्गः_गन्तव्यम्_Resource,
                     UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                     UError_संहिता* स्थितिः);

/**
 * Returns a functionally equivalent locale (considering keywords) for the specified keyword.
 * @param result fillin for the equivalent locale
 * @param resultCapacity capacity of the fillin buffer
 * @param path path to the tree, or NULL for ICU data
 * @param resName top level resource. Example: "collations"
 * @param keyword locale keyword. Example: "collation"
 * @param locid The requested locale
 * @param isAvailable If non-null, pointer to fillin parameter that indicates whether the 
 * requested locale was available. The locale is defined as 'available' if it physically 
 * exists within the specified tree.
 * @param omitDefault if true, omit keyword and value if default. 'de_DE\@collation=standard' -> 'de_DE'
 * @param status error code
 * @return  the actual buffer size needed for the full locale.  If it's greater 
 * than resultCapacity, the returned full name will be truncated and an error code will be returned.
 */
U_CAPI int32_t U_EXPORT2
ures_getFunctionalEquivalent(वर्ण *परिणाम, int32_t परिणाम_क्षमता, 
                             ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *resName, ध्रुव वर्ण *keyword, ध्रुव वर्ण *locid,
                             UBool *is_उपलब्धम्, UBool omitDefault, UError_संहिता *स्थितिः);

/**
 * Given a tree path and keyword, return a string enumeration of all possible values for that keyword.
 * @param path path to the tree, or NULL for ICU data
 * @param keyword a particular keyword to consider, must match a top level resource name 
 * within the tree.
 * @param status error code
 */
U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_मानानि(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *keyword, UError_संहिता *स्थितिः);


/**
 * Get a resource with multi-level fallback. Normally only the top level resources will
 * fallback to its parent. This performs fallback on subresources. For example, when a table
 * is defined in a resource bundle and a parent resource bundle, normally no fallback occurs
 * on the sub-resources because the table is defined in the current resource bundle, but this
 * function can perform fallback on the sub-resources of the table.
 * @param resB              a resource
 * @param inKey             a key associated with the requested resource
 * @param fillIn            if NULL a new UResourceBundle struct is allocated and must be deleted by the caller.
 *                          Alternatively, you can supply a struct to be filled by this function.
 * @param status: fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error 
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return                  a pointer to a UResourceBundle struct. If fill in param was NULL, caller must delete it
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_get_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB,
                          ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                          UResourceBundle* fill_अन्तर् U_LIFETIME_BOUND,
                          UError_संहिता* स्थितिः);

/**
 * Get a String with multi-level fallback. Normally only the top level resources will
 * fallback to its parent. This performs fallback on subresources. For example, when a table
 * is defined in a resource bundle and a parent resource bundle, normally no fallback occurs
 * on the sub-resources because the table is defined in the current resource bundle, but this
 * function can perform fallback on the sub-resources of the table.
 * @param resB              a resource
 * @param inKey             a key associated with the requested resource
 * @param len               if not NULL, used to return the length of the string
 * @param status: fills in the outgoing error code
 *                could be <TT>U_MISSING_RESOURCE_ERROR</TT> if the key is not found
 *                could be a non-failing error 
 *                e.g.: <TT>U_USING_FALLBACK_WARNING</TT>,<TT>U_USING_DEFAULT_WARNING </TT>
 * @return returns a pointer to a zero-terminated UChar array which lives in a
 *         memory mapped/DLL file.
 */
U_CAPI ध्रुव UChar* U_EXPORT2
ures_get_String_By_कुञ्चिका_With_Fallback(ध्रुव UResourceBundle* resB U_LIFETIME_BOUND,
                                ध्रुव वर्ण* अन्तर्_कुञ्चिका,
                                int32_t* len,
                                UError_संहिता* स्थितिः);

#यदि_परिभाषितम् __cplusplus

U_CAPI शून्य U_EXPORT2
ures_get_मानम्_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                          UResourceBundle *अस्थायी_Fill_अन्तर्,
                          icu::Resource_दत्त_मानम् &मानम्_2, UError_संहिता &error_संहिता);

/**
 * Locates the resource specified by `path` in the resource bundle specified by `bundle` (performing any
 * necessary fallback and following any aliases) and calls the specified `sink`'s `put()` method with that
 * resource.  Then walks the bundle's parent chain, calling `put()` on the sink for each item in the
 * parent chain.
 * @param bundle The bundle to search
 * @param path The path of the desired resource
 * @param sink A `ResourceSink` that gets called for each resource in the parent chain
 * @param errorCode The error code
 */
U_CAPI शून्य U_EXPORT2
ures_get_All_वस्तूनि_With_Fallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                             icu::ResourceSink &sink, UError_संहिता &error_संहिता);

/**
 * Locates the resource specified by `path` in the resource bundle specified by `bundle` (performing any
 * necessary fallback and following any aliases) and, if the resource is a table resource, iterates over its
 * immediate child resources (again, following any aliases to get the individual resource values), and calls the specified
 * `sink`'s `put()` method for each child resource (passing it that resource's key and either its actual value or,
 * if that value is an alias, the value you get by following the alias).  Then walks back over the bundle's
 * parent chain, similarly iterating over each parent table resource's child resources.
 * Does not descend beyond one level of table children.
 * @param bundle The bundle to search
 * @param path The path of the desired resource
 * @param sink A `ResourceSink` that gets called for each child resource of the specified resource (and each child
 * of the resources in its parent chain).
 * @param errorCode The error code.  This will be U_RESOURCE_TYPE_MISMATCH if the resource the caller
 * is asking for isn't a table resource.
 */
U_CAPI शून्य U_EXPORT2
ures_getAllChildrenWithFallback(ध्रुव UResourceBundle *bundle, ध्रुव वर्ण *मार्गः,
                                icu::ResourceSink &sink, UError_संहिता &error_संहिता);

#यदि_अन्तः  /* __cplusplus */

/**
 * Get a version number by key
 * @param resB bundle containing version number
 * @param key the key for the version number
 * @param ver fillin for the version number
 * @param status error code
 */
U_CAPI शून्य U_EXPORT2
ures_get_संस्करणम्_By_कुञ्चिका(ध्रुव UResourceBundle *resB,
                     ध्रुव वर्ण *कुञ्चिका,
                     UVersionInfo ver,
                     UError_संहिता *स्थितिः);


/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_संस्करणम्_सङ्ख्या_आन्तरिकम्(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND);

/**
 * Return the name of the Locale associated with this ResourceBundle. This API allows
 * you to query for the real locale of the resource. For example, if you requested 
 * "en_US_CALIFORNIA" and only "en_US" bundle exists, "en_US" will be returned. 
 * For subresources, the locale where this resource comes from will be returned.
 * If fallback has occurred, getLocale will reflect this.
 *
 * This internal version avoids deprecated-warnings in ICU code.
 *
 * @param resourceBundle resource bundle in question
 * @param status just for catching illegal arguments
 * @return  A Locale name
 */
U_CAPI ध्रुव वर्ण* U_EXPORT2
ures_get_Locale_आन्तरिकम्(ध्रुव UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_संहिता* स्थितिः);

/**
 * Same as ures_openDirect() but uses the fill-in parameter instead of allocating a new bundle.
 * 
 * @param r The existing UResourceBundle to fill in. If NULL then status will be
 *          set to U_ILLEGAL_ARGUMENT_ERROR.
 * @param packageName   The packageName and locale together point to an ICU udata object,
 *                      as defined by <code> udata_open( packageName, "res", locale, err) </code>
 *                      or equivalent.  Typically, packageName will refer to a (.dat) file, or to
 *                      a package registered with udata_setAppData(). Using a full file or directory
 *                      pathname for packageName is deprecated. If NULL, ICU data will be used.
 * @param locale  specifies the locale for which we want to open the resource
 *                if NULL, the default locale will be used. If strlen(locale) == 0
 *                root locale will be used.
 * @param status The error code.
 * @see ures_openDirect
 * @internal
 */
U_CAPI शून्य U_EXPORT2
ures_open_प्रत्यक्षम्_Fill_अन्तर्(UResourceBundle *r,
                      ध्रुव वर्ण *packageName,
                      ध्रुव वर्ण *locale,
                      UError_संहिता *स्थितिः);

#यदि_अन्तः /*URESIMP_H*/
