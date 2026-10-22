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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#ଅନ୍ୟଥା

#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/strenum.h"

#ଅନ୍ତର୍ଭାଵ "hash.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

#ଅନ୍ତର୍ଭାଵ "serv.h"
#ଅନ୍ତର୍ଭାଵ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_ଚାବି;
class Locale_ଚାବି_Factory;
class Simple_Locale_ଚାବି_Factory;
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

class U_COMMON_API Locale_ଚାବି : ସର୍ବସାଧାରଣ ICUService_ଚାବି {
  ବ୍ୟକ୍ତିଗତ: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  ସର୍ବସାଧାରଣ:
    ଗଣନା {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ସ୍ଥିର Locale_ଚାବି* createWithCanonicalFallback(ଅଚଳ UnicodeString* primaryID, 
                                                  ଅଚଳ UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ସ୍ଥିର Locale_ଚାବି* createWithCanonicalFallback(ଅଚଳ UnicodeString* primaryID, 
                                                  ଅଚଳ UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  ସୁରକ୍ଷିତ:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_ଚାବି(ଅଚଳ UnicodeString& primaryID, 
              ଅଚଳ UnicodeString& canonicalPrimaryID, 
              ଅଚଳ UnicodeString* canonicalFallbackID, 
              int32_t kind);

 ସର୍ବସାଧାରଣ:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    ଆଭାସୀ UnicodeString& prefix(UnicodeString& result) ଅଚଳ override;

    /**
     * Return the kind code associated with this key.
     */
    ଆଭାସୀ int32_t kind() ଅଚଳ;

    /**
     * Return the canonicalID.
     */
    ଆଭାସୀ UnicodeString& canonicalID(UnicodeString& result) ଅଚଳ override;

    /**
     * Return the currentID.
     */
    ଆଭାସୀ UnicodeString& currentID(UnicodeString& result) ଅଚଳ override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    ଆଭାସୀ UnicodeString& currentDescriptor(UnicodeString& result) ଅଚଳ override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    ଆଭାସୀ Locale& canonicalLocale(Locale& result) ଅଚଳ;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    ଆଭାସୀ Locale& currentLocale(Locale& result) ଅଚଳ;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    ଆଭାସୀ UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    ଆଭାସୀ UBool isFallbackOf(ଅଚଳ UnicodeString& id) ଅଚଳ override;
    
 ସର୍ବସାଧାରଣ:
    /**
     * UObject boilerplate.
     */
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

    /**
     * Destructor.
     */
    ଆଭାସୀ ~Locale_ଚାବି();

#ifdef SERVICE_DEBUG
 ସର୍ବସାଧାରଣ:
    ଆଭାସୀ UnicodeString& debug(UnicodeString& result) ଅଚଳ;
    ଆଭାସୀ UnicodeString& debugClass(UnicodeString& result) ଅଚଳ;
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
class U_COMMON_API Locale_ଚାବି_Factory : ସର୍ବସାଧାରଣ ICUServiceFactory {
ସୁରକ୍ଷିତ:
    ଅଚଳ UnicodeString _name;
    ଅଚଳ int32_t _coverage;

ସର୍ବସାଧାରଣ:
    ଗଣନା {
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
    ଆଭାସୀ ~Locale_ଚାବି_Factory();

ସୁରକ୍ଷିତ:
    /**
     * Constructor used by subclasses.
     */
    Locale_ଚାବି_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_ଚାବି_Factory(int32_t coverage, ଅଚଳ UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
ସର୍ବସାଧାରଣ:
    ଆଭାସୀ UObject* create(ଅଚଳ ICUService_ଚାବି& ଚାବି, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ override;

ସୁରକ୍ଷିତ:
    ଆଭାସୀ UBool handles_ଚାବି(ଅଚଳ ICUService_ଚାବି& ଚାବି, UErrorCode& status) ଅଚଳ;

ସର୍ବସାଧାରଣ:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ଆଭାସୀ ଶୂନ୍ୟ updateVisibleIDs(Hashtable& result, UErrorCode& status) ଅଚଳ override;

    /**
     * Return a localized name for the locale represented by id.
     */
    ଆଭାସୀ UnicodeString& getDisplayName(ଅଚଳ UnicodeString& id, ଅଚଳ Locale& locale, UnicodeString& result) ଅଚଳ override;

ସୁରକ୍ଷିତ:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    ଆଭାସୀ UObject* handleCreate(ଅଚଳ Locale& loc, int32_t kind, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ;

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
    ଆଭାସୀ ଅଚଳ Hashtable* getSupportedIDs(UErrorCode& status) ଅଚଳ;

ସର୍ବସାଧାରଣ:
    /**
     * UObject boilerplate.
     */
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

#ifdef SERVICE_DEBUG
 ସର୍ବସାଧାରଣ:
    ଆଭାସୀ UnicodeString& debug(UnicodeString& result) ଅଚଳ;
    ଆଭାସୀ UnicodeString& debugClass(UnicodeString& result) ଅଚଳ;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_ଚାବି_Factory : ସର୍ବସାଧାରଣ Locale_ଚାବି_Factory {
 ବ୍ୟକ୍ତିଗତ:
    UObject* _obj;
    UnicodeString _id;
    ଅଚଳ int32_t _kind;

 ସର୍ବସାଧାରଣ:
    Simple_Locale_ଚାବି_Factory(UObject* objToAdopt, 
                           ଅଚଳ UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_ଚାବି_Factory(UObject* objToAdopt, 
                           ଅଚଳ Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    ଆଭାସୀ ~Simple_Locale_ଚାବି_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    ଆଭାସୀ UObject* create(ଅଚଳ ICUService_ଚାବି& ଚାବି, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ଆଭାସୀ ଶୂନ୍ୟ updateVisibleIDs(Hashtable& result, UErrorCode& status) ଅଚଳ override;

 ସୁରକ୍ଷିତ:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


ସର୍ବସାଧାରଣ:
    /**
     * UObject boilerplate.
     */
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();

    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;

#ifdef SERVICE_DEBUG
 ସର୍ବସାଧାରଣ:
    ଆଭାସୀ UnicodeString& debug(UnicodeString& result) ଅଚଳ;
    ଆଭାସୀ UnicodeString& debugClass(UnicodeString& result) ଅଚଳ;
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
class U_COMMON_API ICUResourceBundleFactory : ସର୍ବସାଧାରଣ Locale_ଚାବି_Factory 
{
 ସୁରକ୍ଷିତ:
    UnicodeString _bundleName;

 ସର୍ବସାଧାରଣ:
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
    ICUResourceBundleFactory(ଅଚଳ UnicodeString& bundleName);

    /**
     * Destructor
     */
    ଆଭାସୀ ~ICUResourceBundleFactory();

ସୁରକ୍ଷିତ:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    ଆଭାସୀ ଅଚଳ Hashtable* getSupportedIDs(UErrorCode& status) ଅଚଳ override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    ଆଭାସୀ UObject* handleCreate(ଅଚଳ Locale& loc, int32_t kind, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ override;

ସର୍ବସାଧାରଣ:
    /**
     * UObject boilerplate.
     */
    ସ୍ଥିର UClassID U_EXPORT2 getStaticClassID();
    ଆଭାସୀ UClassID getDynamicClassID() ଅଚଳ override;


#ifdef SERVICE_DEBUG
 ସର୍ବସାଧାରଣ:
    ଆଭାସୀ UnicodeString& debug(UnicodeString& result) ଅଚଳ;
    ଆଭାସୀ UnicodeString& debugClass(UnicodeString& result) ଅଚଳ;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : ସର୍ବସାଧାରଣ ICUService
{
 ବ୍ୟକ୍ତିଗତ:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 ସର୍ବସାଧାରଣ:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ଅଚଳ UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API ଆଭାସୀ ~ICULocaleService();

#ଯଦି 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ଅଚଳ UnicodeString& descriptor, UErrorCode& status) ଅଚଳ {
    ଫେରନ୍ତୁ ICUService::get(descriptor, status);
  }

  UObject* get(ଅଚଳ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ଅଚଳ {
    ଫେରନ୍ତୁ ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ଅଚଳ Locale& locale, UErrorCode& status) ଅଚଳ;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ଅଚଳ Locale& locale, int32_t kind, UErrorCode& status) ଅଚଳ;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ଅଚଳ Locale& locale, Locale* actualReturn, UErrorCode& status) ଅଚଳ;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ଅଚଳ Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ଅଚଳ;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API ଆଭାସୀ URegistry_ଚାବି registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API ଆଭାସୀ URegistry_ଚାବି registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API ଆଭାସୀ URegistry_ଚାବି registerInstance(UObject* objToAdopt, ଅଚଳ Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API ଆଭାସୀ URegistry_ଚାବି registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API ଆଭାସୀ StringEnumeration* getAvailableLocales() ଅଚଳ U_LIFETIME_BOUND;

 ସୁରକ୍ଷିତ:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ଅଚଳ UnicodeString& validateFallbackLocale() ଅଚଳ;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API ଆଭାସୀ ICUService_ଚାବି* create_ଚାବି(ଅଚଳ UnicodeString* id, UErrorCode& status) ଅଚଳ override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API ଆଭାସୀ ICUService_ଚାବି* create_ଚାବି(ଅଚଳ UnicodeString* id, int32_t kind, UErrorCode& status) ଅଚଳ;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

