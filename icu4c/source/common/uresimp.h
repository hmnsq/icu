// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/

#اگر_غیر_تعریف_شدہ URESIMP_H
#تعریف_کرو URESIMP_H

#شامل "unicode/ures.h"
#شامل "unicode/utypes.h"

#شامل "uresdata.h"

#تعریف_کرو k_جڑ_Locale_Name         "root"
#تعریف_کرو kPoolBundleName         "pool"

/*
 The default minor version and the version separator must be exactly one
 character long.
*/

#تعریف_کرو k_Default_Minor_نسخہ    "0"
#تعریف_کرو k_نسخہ_Separator       "."
#تعریف_کرو k_نسخہ_لیبل             "Version"

#تعریف_کرو MAGIC1 19700503
#تعریف_کرو MAGIC2 19641227

#تعریف_کرو URES_MAX_ALIAS_LEVEL 256

#تعریف_کرو خالی_خانے_سیٹ 0x2205

ساخت UResource_ڈیٹا_اندراج;
قسمتعریف ساخت UResource_ڈیٹا_اندراج UResource_ڈیٹا_اندراج;

/*
 * Note: If we wanted to make this structure smaller, then we could try
 * to use one UResourceDataEntry pointer for fAlias and fPool, with a separate
 * flag to distinguish whether this struct is for a real bundle with a pool,
 * or for an alias entry for which we won't use the pool after loading.
 */
ساخت UResource_ڈیٹا_اندراج {
    حرف *fName; /* name of the locale for bundle - still to decide whether it is original or fallback */
    حرف *f_راستہ; /* path to bundle - used for distinguishing between resources with the same name */
    UResource_ڈیٹا_اندراج *f_والد; /*next resource in fallback chain*/
    UResource_ڈیٹا_اندراج *fAlias;
    UResource_ڈیٹا_اندراج *fPool;
    Resource_ڈیٹا f_ڈیٹا; /* data for low level access */
    حرف f_Name_بفر[3]; /* A small buffer of free space for fName. The free space is due to struct padding. */
    uint32_t f_گنتی_Existing; /* how much is this resource used */
    UError_رمز fBogus;
    /* int32_t fHashKey;*/ /* for faster access in the hashtable */
};

#تعریف_کرو RES_BUFSIZE 64
#تعریف_کرو RES_راستہ_SEPARATOR   '/'
#تعریف_کرو RES_راستہ_SEPARATOR_S   "/"

U_CAPI خالی U_EXPORT2 ures_init_ڈھیر_Object(UResourceBundle* resB);

#اگر_تعریف_شدہ __cplusplus

ساخت UResourceBundle {
    ثابت حرف *f_کلید; /*لیبل*/
    /**
     * The dataEntry for the actual locale in which this item lives.
     * Used for accessing the item's data.
     * Non-const pointer for reference counting via entryIncrease().
     */
    UResource_ڈیٹا_اندراج *f_ڈیٹا; /*for low-level access*/
    حرف *f_نسخہ;
    /**
     * The dataEntry for the valid locale.
     * Used for /LOCALE/path alias resolution that starts back from the valid locale,
     * rather than from the actual locale of this item which might live in
     * an ancestor bundle.
     */
    UResource_ڈیٹا_اندراج *f_Valid_Locale_ڈیٹا_اندراج;
    حرف *f_Res_راستہ; /* full path to the resource: "zh_TW/CollationElements/Sequence" */
    حرف fResBuf[RES_BUFSIZE];
    int32_t f_Res_راستہ_Len;
    Resource fRes;
    UBool fHasFallback;
    UBool f_Is_چوٹی_Level;
    uint32_t fMagic1;   /* For determining if it's a stack object */
    uint32_t fMagic2;   /* For determining if it's a stack object */
    int32_t f_اشاریہ;
    int32_t f_سائز;

    اندرونی ثابت Resource_ڈیٹا &get_Res_ڈیٹا() ثابت { واپس f_ڈیٹا->f_ڈیٹا; }
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
class U_COMMON_API ڈھیر_UResource_Bundle {
عوامی:
    // No heap allocation. Use only on the stack.
    ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;
    ساکن خالی* U_EXPORT2 عامل new(size_t, خالی*) noexcept = delete;

    ڈھیر_UResource_Bundle();
    ~ڈھیر_UResource_Bundle();

    UResourceBundle* getAlias() U_LIFETIME_BOUND { واپس &bundle; }

    UResourceBundle& ref() U_LIFETIME_BOUND { واپس bundle; }
    ثابت UResourceBundle& ref() ثابت U_LIFETIME_BOUND { واپس bundle; }

    ڈھیر_UResource_Bundle(ثابت ڈھیر_UResource_Bundle&) = delete;
    ڈھیر_UResource_Bundle& عامل=(ثابت ڈھیر_UResource_Bundle&) = delete;

    ڈھیر_UResource_Bundle(ڈھیر_UResource_Bundle&&) = delete;
    ڈھیر_UResource_Bundle& عامل=(ڈھیر_UResource_Bundle&&) = delete;

نجی:
    UResourceBundle bundle;
};

U_NAMESPACE_انتہا

#اگر_ختم  /* __cplusplus */

/**
 * Opens a resource bundle for the locale;
 * if there is not even a base language bundle, then loads the root bundle;
 * never falls back to the default locale.
 *
 * This is used for algorithms that have good pan-Unicode default behavior,
 * such as case mappings, collation, and segmentation (BreakIterator).
 */
U_CAPI UResourceBundle* U_EXPORT2
ures_openNoDefault(ثابت حرف* راستہ, ثابت حرف* localeID, UError_رمز* حالت_2);

/* Some getters used by the copy constructor */
U_CFUNC ثابت حرف* ures_getName(ثابت UResourceBundle* resB U_LIFETIME_BOUND);
#اگر_تعریف_شدہ URES_DEBUG
U_CFUNC ثابت حرف* ures_get_راستہ(ثابت UResourceBundle* resB U_LIFETIME_BOUND);
/**
 * If anything was in the RB cache, dump it to the screen.
 * @return true if there was anything into the cache
 */
U_CAPI UBool U_EXPORT2 ures_dumpCacheContents(خالی);
#اگر_ختم

/* Candidates for export */
U_CFUNC UResourceBundle*
ures_copyResb(UResourceBundle* r U_LIFETIME_BOUND,
              ثابت UResourceBundle* original,
              UError_رمز* حالت_2);

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
ures_ڈھونڈو_Resource(ثابت حرف* راستہ_مقصد_Resource,
                  UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                  UError_رمز* حالت_2);

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
ures_ڈھونڈو_Sub_Resource(ثابت UResourceBundle* resB,
                     حرف* راستہ_مقصد_Resource,
                     UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                     UError_رمز* حالت_2);

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
ures_getFunctionalEquivalent(حرف *نتیجہ, int32_t نتیجہ_استعداد, 
                             ثابت حرف *راستہ, ثابت حرف *resName, ثابت حرف *keyword, ثابت حرف *locid,
                             UBool *is_دستیاب, UBool omitDefault, UError_رمز *حالت_2);

/**
 * Given a tree path and keyword, return a string enumeration of all possible values for that keyword.
 * @param path path to the tree, or NULL for ICU data
 * @param keyword a particular keyword to consider, must match a top level resource name 
 * within the tree.
 * @param status error code
 */
U_CAPI UEnumeration* U_EXPORT2
ures_get_Keyword_اقدار(ثابت حرف *راستہ, ثابت حرف *keyword, UError_رمز *حالت_2);


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
ures_get_By_کلید_With_Fallback(ثابت UResourceBundle* resB,
                          ثابت حرف* اندر_کلید,
                          UResourceBundle* fill_اندر U_LIFETIME_BOUND,
                          UError_رمز* حالت_2);

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
U_CAPI ثابت UChar* U_EXPORT2
ures_get_String_By_کلید_With_Fallback(ثابت UResourceBundle* resB U_LIFETIME_BOUND,
                                ثابت حرف* اندر_کلید,
                                int32_t* len,
                                UError_رمز* حالت_2);

#اگر_تعریف_شدہ __cplusplus

U_CAPI خالی U_EXPORT2
ures_get_قدر_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                          UResourceBundle *عارضی_Fill_اندر,
                          icu::Resource_ڈیٹا_قدر &قدر_2, UError_رمز &error_رمز);

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
U_CAPI خالی U_EXPORT2
ures_get_All_اشیاء_With_Fallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                             icu::ResourceSink &sink, UError_رمز &error_رمز);

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
U_CAPI خالی U_EXPORT2
ures_getAllChildrenWithFallback(ثابت UResourceBundle *bundle, ثابت حرف *راستہ,
                                icu::ResourceSink &sink, UError_رمز &error_رمز);

#اگر_ختم  /* __cplusplus */

/**
 * Get a version number by key
 * @param resB bundle containing version number
 * @param key the key for the version number
 * @param ver fillin for the version number
 * @param status error code
 */
U_CAPI خالی U_EXPORT2
ures_get_نسخہ_By_کلید(ثابت UResourceBundle *resB,
                     ثابت حرف *کلید,
                     UVersionInfo ver,
                     UError_رمز *حالت_2);


/**
 * Internal function.
 * Return the version number associated with this ResourceBundle as a string.
 *
 * @param resourceBundle The resource bundle for which the version is checked.
 * @return  A version number string as specified in the resource bundle or its parent.
 *          The caller does not own this string.
 * @see ures_getVersion
 */
U_CAPI ثابت حرف* U_EXPORT2
ures_get_نسخہ_نمبر_اندرونی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND);

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
U_CAPI ثابت حرف* U_EXPORT2
ures_get_Locale_اندرونی(ثابت UResourceBundle* resourceBundle U_LIFETIME_BOUND, UError_رمز* حالت_2);

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
U_CAPI خالی U_EXPORT2
ures_open_براہ_راست_Fill_اندر(UResourceBundle *r,
                      ثابت حرف *packageName,
                      ثابت حرف *locale,
                      UError_رمز *حالت_2);

#اگر_ختم /*URESIMP_H*/
