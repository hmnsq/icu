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

#અન્તર્ભાવ "unicode/utypes.h"

#જો UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#નહીંતર

#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/strenum.h"

#અન્તર્ભાવ "hash.h"
#અન્તર્ભાવ "uvector.h"

#અન્તર્ભાવ "serv.h"
#અન્તર્ભાવ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_ચાવી;
class Locale_ચાવી_Factory;
class Simple_Locale_ચાવી_Factory;
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

class U_COMMON_API Locale_ચાવી : જાહેર ICUService_ચાવી {
  ખાનગી: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  જાહેર:
    ગણના {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    સ્થિર Locale_ચાવી* createWithCanonicalFallback(અચલ UnicodeString* primaryID, 
                                                  અચલ UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    સ્થિર Locale_ચાવી* createWithCanonicalFallback(અચલ UnicodeString* primaryID, 
                                                  અચલ UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  સુરક્ષિત:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_ચાવી(અચલ UnicodeString& primaryID, 
              અચલ UnicodeString& canonicalPrimaryID, 
              અચલ UnicodeString* canonicalFallbackID, 
              int32_t kind);

 જાહેર:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    આભાસી UnicodeString& prefix(UnicodeString& result) અચલ override;

    /**
     * Return the kind code associated with this key.
     */
    આભાસી int32_t kind() અચલ;

    /**
     * Return the canonicalID.
     */
    આભાસી UnicodeString& canonicalID(UnicodeString& result) અચલ override;

    /**
     * Return the currentID.
     */
    આભાસી UnicodeString& currentID(UnicodeString& result) અચલ override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    આભાસી UnicodeString& currentDescriptor(UnicodeString& result) અચલ override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    આભાસી Locale& canonicalLocale(Locale& result) અચલ;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    આભાસી Locale& currentLocale(Locale& result) અચલ;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    આભાસી UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    આભાસી UBool isFallbackOf(અચલ UnicodeString& id) અચલ override;
    
 જાહેર:
    /**
     * UObject boilerplate.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    આભાસી UClassID getDynamicClassID() અચલ override;

    /**
     * Destructor.
     */
    આભાસી ~Locale_ચાવી();

#ifdef SERVICE_DEBUG
 જાહેર:
    આભાસી UnicodeString& debug(UnicodeString& result) અચલ;
    આભાસી UnicodeString& debugClass(UnicodeString& result) અચલ;
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
class U_COMMON_API Locale_ચાવી_Factory : જાહેર ICUServiceFactory {
સુરક્ષિત:
    અચલ UnicodeString _name;
    અચલ int32_t _coverage;

જાહેર:
    ગણના {
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
    આભાસી ~Locale_ચાવી_Factory();

સુરક્ષિત:
    /**
     * Constructor used by subclasses.
     */
    Locale_ચાવી_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_ચાવી_Factory(int32_t coverage, અચલ UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
જાહેર:
    આભાસી UObject* create(અચલ ICUService_ચાવી& ચાવી, અચલ ICUService* service, UErrorCode& status) અચલ override;

સુરક્ષિત:
    આભાસી UBool handles_ચાવી(અચલ ICUService_ચાવી& ચાવી, UErrorCode& status) અચલ;

જાહેર:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    આભાસી શૂન્ય updateVisibleIDs(Hashtable& result, UErrorCode& status) અચલ override;

    /**
     * Return a localized name for the locale represented by id.
     */
    આભાસી UnicodeString& getDisplayName(અચલ UnicodeString& id, અચલ Locale& locale, UnicodeString& result) અચલ override;

સુરક્ષિત:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    આભાસી UObject* handleCreate(અચલ Locale& loc, int32_t kind, અચલ ICUService* service, UErrorCode& status) અચલ;

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
    આભાસી અચલ Hashtable* getSupportedIDs(UErrorCode& status) અચલ;

જાહેર:
    /**
     * UObject boilerplate.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    આભાસી UClassID getDynamicClassID() અચલ override;

#ifdef SERVICE_DEBUG
 જાહેર:
    આભાસી UnicodeString& debug(UnicodeString& result) અચલ;
    આભાસી UnicodeString& debugClass(UnicodeString& result) અચલ;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_ચાવી_Factory : જાહેર Locale_ચાવી_Factory {
 ખાનગી:
    UObject* _obj;
    UnicodeString _id;
    અચલ int32_t _kind;

 જાહેર:
    Simple_Locale_ચાવી_Factory(UObject* objToAdopt, 
                           અચલ UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_ચાવી_Factory(UObject* objToAdopt, 
                           અચલ Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    આભાસી ~Simple_Locale_ચાવી_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    આભાસી UObject* create(અચલ ICUService_ચાવી& ચાવી, અચલ ICUService* service, UErrorCode& status) અચલ override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    આભાસી શૂન્ય updateVisibleIDs(Hashtable& result, UErrorCode& status) અચલ override;

 સુરક્ષિત:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


જાહેર:
    /**
     * UObject boilerplate.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();

    આભાસી UClassID getDynamicClassID() અચલ override;

#ifdef SERVICE_DEBUG
 જાહેર:
    આભાસી UnicodeString& debug(UnicodeString& result) અચલ;
    આભાસી UnicodeString& debugClass(UnicodeString& result) અચલ;
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
class U_COMMON_API ICUResourceBundleFactory : જાહેર Locale_ચાવી_Factory 
{
 સુરક્ષિત:
    UnicodeString _bundleName;

 જાહેર:
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
    ICUResourceBundleFactory(અચલ UnicodeString& bundleName);

    /**
     * Destructor
     */
    આભાસી ~ICUResourceBundleFactory();

સુરક્ષિત:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    આભાસી અચલ Hashtable* getSupportedIDs(UErrorCode& status) અચલ override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    આભાસી UObject* handleCreate(અચલ Locale& loc, int32_t kind, અચલ ICUService* service, UErrorCode& status) અચલ override;

જાહેર:
    /**
     * UObject boilerplate.
     */
    સ્થિર UClassID U_EXPORT2 getStaticClassID();
    આભાસી UClassID getDynamicClassID() અચલ override;


#ifdef SERVICE_DEBUG
 જાહેર:
    આભાસી UnicodeString& debug(UnicodeString& result) અચલ;
    આભાસી UnicodeString& debugClass(UnicodeString& result) અચલ;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : જાહેર ICUService
{
 ખાનગી:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 જાહેર:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(અચલ UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API આભાસી ~ICULocaleService();

#જો 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(અચલ UnicodeString& descriptor, UErrorCode& status) અચલ {
    પાછા ICUService::get(descriptor, status);
  }

  UObject* get(અચલ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) અચલ {
    પાછા ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(અચલ Locale& locale, UErrorCode& status) અચલ;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(અચલ Locale& locale, int32_t kind, UErrorCode& status) અચલ;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(અચલ Locale& locale, Locale* actualReturn, UErrorCode& status) અચલ;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(અચલ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) અચલ;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API આભાસી URegistry_ચાવી registerInstance(UObject* objToAdopt, અચલ Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API આભાસી URegistry_ચાવી registerInstance(UObject* objToAdopt, અચલ Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API આભાસી URegistry_ચાવી registerInstance(UObject* objToAdopt, અચલ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API આભાસી URegistry_ચાવી registerInstance(UObject* objToAdopt, અચલ UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API આભાસી StringEnumeration* getAvailableLocales() અચલ U_LIFETIME_BOUND;

 સુરક્ષિત:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API અચલ UnicodeString& validateFallbackLocale() અચલ;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API આભાસી ICUService_ચાવી* create_ચાવી(અચલ UnicodeString* id, UErrorCode& status) અચલ override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API આભાસી ICUService_ચાવી* create_ચાવી(અચલ UnicodeString* id, int32_t kind, UErrorCode& status) અચલ;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

