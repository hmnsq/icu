// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2011, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 *
 *******************************************************************************
 */
#ifndef ICULSERV_H
#define ICULSERV_H

#অন্তর্ভাব "unicode/utypes.h"

#যদি UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#অন্যথা

#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/strenum.h"

#অন্তর্ভাব "hash.h"
#অন্তর্ভাব "uvector.h"

#অন্তর্ভাব "serv.h"
#অন্তর্ভাব "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_চাবি;
class Locale_চাবি_Factory;
class Simple_Locale_চাবি_Factory;
class ServiceListener;

/*
 ******************************************************************
 */

/**
 * A subclass of Key that implements a locale fallback mechanism.
 * The first locale to search for is the locale provided by the
 * client, and the fallback locale to search for is the current
 * default locale.  If a prefix is present, the currentDescriptor
 * includes it before the locale proper, separated by "/".  This
 * is the default key instantiated by ICULocaleService.</p>
 *
 * <p>Canonicalization adjusts the locale string so that the
 * section before the first understore is in lower case, and the rest
 * is in upper case, with no trailing underscores.</p> 
 */

class U_COMMON_API Locale_চাবি : সর্বজনীন ICUService_চাবি {
  ব্যক্তিগত: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  সর্বজনীন:
    গণনা {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    স্থির Locale_চাবি* createWithCanonicalFallback(ধ্রুবক UnicodeString* primaryID, 
                                                  ধ্রুবক UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    স্থির Locale_চাবি* createWithCanonicalFallback(ধ্রুবক UnicodeString* primaryID, 
                                                  ধ্রুবক UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  সুরক্ষিত:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_চাবি(ধ্রুবক UnicodeString& primaryID, 
              ধ্রুবক UnicodeString& canonicalPrimaryID, 
              ধ্রুবক UnicodeString* canonicalFallbackID, 
              int32_t kind);

 সর্বজনীন:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    আভাসী UnicodeString& prefix(UnicodeString& ফলাফল) ধ্রুবক override;

    /**
     * Return the kind code associated with this key.
     */
    আভাসী int32_t kind() ধ্রুবক;

    /**
     * Return the canonicalID.
     */
    আভাসী UnicodeString& canonicalID(UnicodeString& ফলাফল) ধ্রুবক override;

    /**
     * Return the currentID.
     */
    আভাসী UnicodeString& currentID(UnicodeString& ফলাফল) ধ্রুবক override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    আভাসী UnicodeString& currentDescriptor(UnicodeString& ফলাফল) ধ্রুবক override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    আভাসী Locale& canonicalLocale(Locale& ফলাফল) ধ্রুবক;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    আভাসী Locale& currentLocale(Locale& ফলাফল) ধ্রুবক;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    আভাসী UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    আভাসী UBool isFallbackOf(ধ্রুবক UnicodeString& id) ধ্রুবক override;
    
 সর্বজনীন:
    /**
     * UObject boilerplate.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

    /**
     * Destructor.
     */
    আভাসী ~Locale_চাবি();

#ifdef SERVICE_DEBUG
 সর্বজনীন:
    আভাসী UnicodeString& debug(UnicodeString& ফলাফল) ধ্রুবক;
    আভাসী UnicodeString& debugClass(UnicodeString& ফলাফল) ধ্রুবক;
#endif

};

/*
 ******************************************************************
 */

/**
 * A subclass of ICUServiceFactory that uses LocaleKeys, and is able to
 * 'cover' more specific locales with more general locales that it
 * supports.  
 *
 * <p>Coverage may be either of the values VISIBLE or INVISIBLE.
 *
 * <p>'Visible' indicates that the specific locale(s) supported by
 * the factory are registered in getSupportedIDs, 'Invisible'
 * indicates that they are not.
 *
 * <p>Localization of visible ids is handled
 * by the handling factory, regardless of kind.
 */
class U_COMMON_API Locale_চাবি_Factory : সর্বজনীন ICUServiceFactory {
সুরক্ষিত:
    ধ্রুবক UnicodeString _name;
    ধ্রুবক int32_t _coverage;

সর্বজনীন:
    গণনা {
        /**
         * Coverage value indicating that the factory makes
         * its locales visible, and does not cover more specific 
         * locales.
         */
        VISIBLE = 0,

        /**
         * Coverage value indicating that the factory does not make
         * its locales visible, and does not cover more specific
         * locales.
         */
        INVISIBLE = 1
    };

    /**
     * Destructor.
     */
    আভাসী ~Locale_চাবি_Factory();

সুরক্ষিত:
    /**
     * Constructor used by subclasses.
     */
    Locale_চাবি_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_চাবি_Factory(int32_t coverage, ধ্রুবক UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
সর্বজনীন:
    আভাসী UObject* create(ধ্রুবক ICUService_চাবি& চাবি, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক override;

সুরক্ষিত:
    আভাসী UBool handles_চাবি(ধ্রুবক ICUService_চাবি& চাবি, UErrorCode& status) ধ্রুবক;

সর্বজনীন:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    আভাসী শূন্য updateVisibleIDs(Hashtable& ফলাফল, UErrorCode& status) ধ্রুবক override;

    /**
     * Return a localized name for the locale represented by id.
     */
    আভাসী UnicodeString& getDisplayName(ধ্রুবক UnicodeString& id, ধ্রুবক Locale& locale, UnicodeString& ফলাফল) ধ্রুবক override;

সুরক্ষিত:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    আভাসী UObject* handleCreate(ধ্রুবক Locale& loc, int32_t kind, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক;

   /**
     * Return true if this id is one the factory supports (visible or 
     * otherwise).
     */
 //   virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;

   /**
     * Return the set of ids that this factory supports (visible or 
     * otherwise).  This can be called often and might need to be
     * cached if it is expensive to create.
     */
    আভাসী ধ্রুবক Hashtable* getSupportedIDs(UErrorCode& status) ধ্রুবক;

সর্বজনীন:
    /**
     * UObject boilerplate.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

#ifdef SERVICE_DEBUG
 সর্বজনীন:
    আভাসী UnicodeString& debug(UnicodeString& ফলাফল) ধ্রুবক;
    আভাসী UnicodeString& debugClass(UnicodeString& ফলাফল) ধ্রুবক;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_চাবি_Factory : সর্বজনীন Locale_চাবি_Factory {
 ব্যক্তিগত:
    UObject* _obj;
    UnicodeString _id;
    ধ্রুবক int32_t _kind;

 সর্বজনীন:
    Simple_Locale_চাবি_Factory(UObject* objToAdopt, 
                           ধ্রুবক UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_চাবি_Factory(UObject* objToAdopt, 
                           ধ্রুবক Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    আভাসী ~Simple_Locale_চাবি_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    আভাসী UObject* create(ধ্রুবক ICUService_চাবি& চাবি, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    আভাসী শূন্য updateVisibleIDs(Hashtable& ফলাফল, UErrorCode& status) ধ্রুবক override;

 সুরক্ষিত:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


সর্বজনীন:
    /**
     * UObject boilerplate.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();

    আভাসী UClassID getDynamicClassID() ধ্রুবক override;

#ifdef SERVICE_DEBUG
 সর্বজনীন:
    আভাসী UnicodeString& debug(UnicodeString& ফলাফল) ধ্রুবক;
    আভাসী UnicodeString& debugClass(UnicodeString& ফলাফল) ধ্রুবক;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that creates a service based on the ICU locale data.
 * This is a base class for most ICU factories.  Subclasses instantiate it
 * with a constructor that takes a bundle name, which determines the supported
 * IDs.  Subclasses then override handleCreate to create the actual service
 * object.  The default implementation returns a resource bundle.
 */
class U_COMMON_API ICUResourceBundleFactory : সর্বজনীন Locale_চাবি_Factory 
{
 সুরক্ষিত:
    UnicodeString _bundleName;

 সর্বজনীন:
    /**
     * Convenience constructor that uses the main ICU bundle name.
     */
    ICUResourceBundleFactory();

    /**
     * A service factory based on ICU resource data in resources with
     * the given name.  This should be a 'path' that can be passed to
     * ures_openAvailableLocales, such as U_ICUDATA or U_ICUDATA_COLL.
     * The empty string is equivalent to U_ICUDATA.
     */
    ICUResourceBundleFactory(ধ্রুবক UnicodeString& bundleName);

    /**
     * Destructor
     */
    আভাসী ~ICUResourceBundleFactory();

সুরক্ষিত:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    আভাসী ধ্রুবক Hashtable* getSupportedIDs(UErrorCode& status) ধ্রুবক override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    আভাসী UObject* handleCreate(ধ্রুবক Locale& loc, int32_t kind, ধ্রুবক ICUService* service, UErrorCode& status) ধ্রুবক override;

সর্বজনীন:
    /**
     * UObject boilerplate.
     */
    স্থির UClassID U_EXPORT2 getStaticClassID();
    আভাসী UClassID getDynamicClassID() ধ্রুবক override;


#ifdef SERVICE_DEBUG
 সর্বজনীন:
    আভাসী UnicodeString& debug(UnicodeString& ফলাফল) ধ্রুবক;
    আভাসী UnicodeString& debugClass(UnicodeString& ফলাফল) ধ্রুবক;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : সর্বজনীন ICUService
{
 ব্যক্তিগত:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 সর্বজনীন:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ধ্রুবক UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API আভাসী ~ICULocaleService();

#যদি 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ধ্রুবক UnicodeString& descriptor, UErrorCode& status) ধ্রুবক {
    ফেরত ICUService::get(descriptor, status);
  }

  UObject* get(ধ্রুবক UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ধ্রুবক {
    ফেরত ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ধ্রুবক Locale& locale, UErrorCode& status) ধ্রুবক;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ধ্রুবক Locale& locale, int32_t kind, UErrorCode& status) ধ্রুবক;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ধ্রুবক Locale& locale, Locale* actualReturn, UErrorCode& status) ধ্রুবক;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ধ্রুবক Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ধ্রুবক;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API আভাসী URegistry_চাবি registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API আভাসী URegistry_চাবি registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API আভাসী URegistry_চাবি registerInstance(UObject* objToAdopt, ধ্রুবক Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API আভাসী URegistry_চাবি registerInstance(UObject* objToAdopt, ধ্রুবক UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API আভাসী StringEnumeration* getAvailableLocales() ধ্রুবক U_LIFETIME_BOUND;

 সুরক্ষিত:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ধ্রুবক UnicodeString& validateFallbackLocale() ধ্রুবক;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API আভাসী ICUService_চাবি* create_চাবি(ধ্রুবক UnicodeString* id, UErrorCode& status) ধ্রুবক override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API আভাসী ICUService_চাবি* create_চাবি(ধ্রুবক UnicodeString* id, int32_t kind, UErrorCode& status) ধ্রুবক;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

