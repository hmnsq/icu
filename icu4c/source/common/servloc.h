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

#अन्तर्भाव "unicode/utypes.h"

#जर UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#अन्यथा

#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/strenum.h"

#अन्तर्भाव "hash.h"
#अन्तर्भाव "uvector.h"

#अन्तर्भाव "serv.h"
#अन्तर्भाव "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_किल्ली;
class Locale_किल्ली_Factory;
class Simple_Locale_किल्ली_Factory;
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

class U_COMMON_API Locale_किल्ली : सार्वजनिक ICUService_किल्ली {
  खाजगी: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  सार्वजनिक:
    गणना {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    स्थिर Locale_किल्ली* createWithCanonicalFallback(स्थिरांक UnicodeString* primaryID, 
                                                  स्थिरांक UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    स्थिर Locale_किल्ली* createWithCanonicalFallback(स्थिरांक UnicodeString* primaryID, 
                                                  स्थिरांक UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  संरक्षित:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_किल्ली(स्थिरांक UnicodeString& primaryID, 
              स्थिरांक UnicodeString& canonicalPrimaryID, 
              स्थिरांक UnicodeString* canonicalFallbackID, 
              int32_t kind);

 सार्वजनिक:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    आभासी UnicodeString& prefix(UnicodeString& result) स्थिरांक override;

    /**
     * Return the kind code associated with this key.
     */
    आभासी int32_t kind() स्थिरांक;

    /**
     * Return the canonicalID.
     */
    आभासी UnicodeString& canonicalID(UnicodeString& result) स्थिरांक override;

    /**
     * Return the currentID.
     */
    आभासी UnicodeString& currentID(UnicodeString& result) स्थिरांक override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    आभासी UnicodeString& currentDescriptor(UnicodeString& result) स्थिरांक override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    आभासी Locale& canonicalLocale(Locale& result) स्थिरांक;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    आभासी Locale& currentLocale(Locale& result) स्थिरांक;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    आभासी UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    आभासी UBool isFallbackOf(स्थिरांक UnicodeString& id) स्थिरांक override;
    
 सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभासी UClassID getDynamicClassID() स्थिरांक override;

    /**
     * Destructor.
     */
    आभासी ~Locale_किल्ली();

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभासी UnicodeString& debug(UnicodeString& result) स्थिरांक;
    आभासी UnicodeString& debugClass(UnicodeString& result) स्थिरांक;
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
class U_COMMON_API Locale_किल्ली_Factory : सार्वजनिक ICUServiceFactory {
संरक्षित:
    स्थिरांक UnicodeString _name;
    स्थिरांक int32_t _coverage;

सार्वजनिक:
    गणना {
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
    आभासी ~Locale_किल्ली_Factory();

संरक्षित:
    /**
     * Constructor used by subclasses.
     */
    Locale_किल्ली_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_किल्ली_Factory(int32_t coverage, स्थिरांक UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
सार्वजनिक:
    आभासी UObject* create(स्थिरांक ICUService_किल्ली& किल्ली, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक override;

संरक्षित:
    आभासी UBool handles_किल्ली(स्थिरांक ICUService_किल्ली& किल्ली, UErrorCode& status) स्थिरांक;

सार्वजनिक:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    आभासी शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) स्थिरांक override;

    /**
     * Return a localized name for the locale represented by id.
     */
    आभासी UnicodeString& getDisplayName(स्थिरांक UnicodeString& id, स्थिरांक Locale& locale, UnicodeString& result) स्थिरांक override;

संरक्षित:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    आभासी UObject* handleCreate(स्थिरांक Locale& loc, int32_t kind, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक;

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
    आभासी स्थिरांक Hashtable* getSupportedIDs(UErrorCode& status) स्थिरांक;

सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभासी UClassID getDynamicClassID() स्थिरांक override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभासी UnicodeString& debug(UnicodeString& result) स्थिरांक;
    आभासी UnicodeString& debugClass(UnicodeString& result) स्थिरांक;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_किल्ली_Factory : सार्वजनिक Locale_किल्ली_Factory {
 खाजगी:
    UObject* _obj;
    UnicodeString _id;
    स्थिरांक int32_t _kind;

 सार्वजनिक:
    Simple_Locale_किल्ली_Factory(UObject* objToAdopt, 
                           स्थिरांक UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_किल्ली_Factory(UObject* objToAdopt, 
                           स्थिरांक Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    आभासी ~Simple_Locale_किल्ली_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    आभासी UObject* create(स्थिरांक ICUService_किल्ली& किल्ली, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    आभासी शून्य updateVisibleIDs(Hashtable& result, UErrorCode& status) स्थिरांक override;

 संरक्षित:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();

    आभासी UClassID getDynamicClassID() स्थिरांक override;

#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभासी UnicodeString& debug(UnicodeString& result) स्थिरांक;
    आभासी UnicodeString& debugClass(UnicodeString& result) स्थिरांक;
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
class U_COMMON_API ICUResourceBundleFactory : सार्वजनिक Locale_किल्ली_Factory 
{
 संरक्षित:
    UnicodeString _bundleName;

 सार्वजनिक:
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
    ICUResourceBundleFactory(स्थिरांक UnicodeString& bundleName);

    /**
     * Destructor
     */
    आभासी ~ICUResourceBundleFactory();

संरक्षित:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    आभासी स्थिरांक Hashtable* getSupportedIDs(UErrorCode& status) स्थिरांक override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    आभासी UObject* handleCreate(स्थिरांक Locale& loc, int32_t kind, स्थिरांक ICUService* service, UErrorCode& status) स्थिरांक override;

सार्वजनिक:
    /**
     * UObject boilerplate.
     */
    स्थिर UClassID U_EXPORT2 getStaticClassID();
    आभासी UClassID getDynamicClassID() स्थिरांक override;


#ifdef SERVICE_DEBUG
 सार्वजनिक:
    आभासी UnicodeString& debug(UnicodeString& result) स्थिरांक;
    आभासी UnicodeString& debugClass(UnicodeString& result) स्थिरांक;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : सार्वजनिक ICUService
{
 खाजगी:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 सार्वजनिक:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(स्थिरांक UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API आभासी ~ICULocaleService();

#जर 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(स्थिरांक UnicodeString& descriptor, UErrorCode& status) स्थिरांक {
    परत ICUService::get(descriptor, status);
  }

  UObject* get(स्थिरांक UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) स्थिरांक {
    परत ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(स्थिरांक Locale& locale, UErrorCode& status) स्थिरांक;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(स्थिरांक Locale& locale, int32_t kind, UErrorCode& status) स्थिरांक;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(स्थिरांक Locale& locale, Locale* actualReturn, UErrorCode& status) स्थिरांक;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(स्थिरांक Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) स्थिरांक;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API आभासी URegistry_किल्ली registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API आभासी URegistry_किल्ली registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API आभासी URegistry_किल्ली registerInstance(UObject* objToAdopt, स्थिरांक Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API आभासी URegistry_किल्ली registerInstance(UObject* objToAdopt, स्थिरांक UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API आभासी StringEnumeration* getAvailableLocales() स्थिरांक U_LIFETIME_BOUND;

 संरक्षित:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API स्थिरांक UnicodeString& validateFallbackLocale() स्थिरांक;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API आभासी ICUService_किल्ली* create_किल्ली(स्थिरांक UnicodeString* id, UErrorCode& status) स्थिरांक override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API आभासी ICUService_किल्ली* create_किल्ली(स्थिरांक UnicodeString* id, int32_t kind, UErrorCode& status) स्थिरांक;

  मित्र class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

