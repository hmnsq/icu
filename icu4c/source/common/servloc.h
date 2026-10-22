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

#ضمن "unicode/utypes.h"

#إذا UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_END

#وإلا

#ضمن "unicode/unistr.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/strenum.h"

#ضمن "hash.h"
#ضمن "uvector.h"

#ضمن "serv.h"
#ضمن "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_ئاچقۇچ;
class Locale_ئاچقۇچ_Factory;
class Simple_Locale_ئاچقۇچ_Factory;
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

class U_COMMON_API Locale_ئاچقۇچ : عام ICUService_ئاچقۇچ {
  خاص: 
    int32_t _kind;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  عام:
    تعداد {
        KIND_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساكن Locale_ئاچقۇچ* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID,
                                                  UErrorCode& status);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ساكن Locale_ئاچقۇچ* createWithCanonicalFallback(ثابت UnicodeString* primaryID, 
                                                  ثابت UnicodeString* canonicalFallbackID, 
                                                  int32_t kind,
                                                  UErrorCode& status);

  محمي:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_ئاچقۇچ(ثابت UnicodeString& primaryID, 
              ثابت UnicodeString& canonicalPrimaryID, 
              ثابت UnicodeString* canonicalFallbackID, 
              int32_t kind);

 عام:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    virtual UnicodeString& prefix(UnicodeString& result) ثابت override;

    /**
     * Return the kind code associated with this key.
     */
    virtual int32_t kind() ثابت;

    /**
     * Return the canonicalID.
     */
    virtual UnicodeString& canonicalID(UnicodeString& result) ثابت override;

    /**
     * Return the currentID.
     */
    virtual UnicodeString& currentID(UnicodeString& result) ثابت override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    virtual UnicodeString& currentDescriptor(UnicodeString& result) ثابت override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    virtual Locale& canonicalLocale(Locale& result) ثابت;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    virtual Locale& currentLocale(Locale& result) ثابت;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    virtual UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    virtual UBool isFallbackOf(ثابت UnicodeString& id) ثابت override;
    
 عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    virtual UClassID getDynamicClassID() ثابت override;

    /**
     * Destructor.
     */
    virtual ~Locale_ئاچقۇچ();

#ifdef SERVICE_DEBUG
 عام:
    virtual UnicodeString& debug(UnicodeString& result) ثابت;
    virtual UnicodeString& debugClass(UnicodeString& result) ثابت;
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
class U_COMMON_API Locale_ئاچقۇچ_Factory : عام ICUServiceFactory {
محمي:
    ثابت UnicodeString _name;
    ثابت int32_t _coverage;

عام:
    تعداد {
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
    virtual ~Locale_ئاچقۇچ_Factory();

محمي:
    /**
     * Constructor used by subclasses.
     */
    Locale_ئاچقۇچ_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_ئاچقۇچ_Factory(int32_t coverage, ثابت UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
عام:
    virtual UObject* create(ثابت ICUService_ئاچقۇچ& ئاچقۇچ, ثابت ICUService* service, UErrorCode& status) ثابت override;

محمي:
    virtual UBool handles_ئاچقۇچ(ثابت ICUService_ئاچقۇچ& ئاچقۇچ, UErrorCode& status) ثابت;

عام:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    virtual فراغ updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت override;

    /**
     * Return a localized name for the locale represented by id.
     */
    virtual UnicodeString& getDisplayName(ثابت UnicodeString& id, ثابت Locale& locale, UnicodeString& result) ثابت override;

محمي:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    virtual UObject* handleCreate(ثابت Locale& loc, int32_t kind, ثابت ICUService* service, UErrorCode& status) ثابت;

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
    virtual ثابت Hashtable* getSupportedIDs(UErrorCode& status) ثابت;

عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    virtual UClassID getDynamicClassID() ثابت override;

#ifdef SERVICE_DEBUG
 عام:
    virtual UnicodeString& debug(UnicodeString& result) ثابت;
    virtual UnicodeString& debugClass(UnicodeString& result) ثابت;
#endif

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_ئاچقۇچ_Factory : عام Locale_ئاچقۇچ_Factory {
 خاص:
    UObject* _obj;
    UnicodeString _id;
    ثابت int32_t _kind;

 عام:
    Simple_Locale_ئاچقۇچ_Factory(UObject* objToAdopt, 
                           ثابت UnicodeString& locale, 
                           int32_t kind, 
                           int32_t coverage);

    Simple_Locale_ئاچقۇچ_Factory(UObject* objToAdopt, 
                           ثابت Locale& locale, 
                           int32_t kind, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    virtual ~Simple_Locale_ئاچقۇچ_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    virtual UObject* create(ثابت ICUService_ئاچقۇچ& ئاچقۇچ, ثابت ICUService* service, UErrorCode& status) ثابت override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    virtual فراغ updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت override;

 محمي:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();

    virtual UClassID getDynamicClassID() ثابت override;

#ifdef SERVICE_DEBUG
 عام:
    virtual UnicodeString& debug(UnicodeString& result) ثابت;
    virtual UnicodeString& debugClass(UnicodeString& result) ثابت;
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
class U_COMMON_API ICUResourceBundleFactory : عام Locale_ئاچقۇچ_Factory 
{
 محمي:
    UnicodeString _bundleName;

 عام:
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
    ICUResourceBundleFactory(ثابت UnicodeString& bundleName);

    /**
     * Destructor
     */
    virtual ~ICUResourceBundleFactory();

محمي:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    virtual ثابت Hashtable* getSupportedIDs(UErrorCode& status) ثابت override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    virtual UObject* handleCreate(ثابت Locale& loc, int32_t kind, ثابت ICUService* service, UErrorCode& status) ثابت override;

عام:
    /**
     * UObject boilerplate.
     */
    ساكن UClassID U_EXPORT2 getStaticClassID();
    virtual UClassID getDynamicClassID() ثابت override;


#ifdef SERVICE_DEBUG
 عام:
    virtual UnicodeString& debug(UnicodeString& result) ثابت;
    virtual UnicodeString& debugClass(UnicodeString& result) ثابت;
#endif

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : عام ICUService
{
 خاص:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 عام:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ثابت UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API virtual ~ICULocaleService();

#إذا 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ثابت UnicodeString& descriptor, UErrorCode& status) ثابت {
    أرجع ICUService::get(descriptor, status);
  }

  UObject* get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ثابت {
    أرجع ICUService::get(descriptor, actualReturn, status);
  }
#endif

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, UErrorCode& status) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t kind, UErrorCode& status) ثابت;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, Locale* actualReturn, UErrorCode& status) ثابت;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ثابت Locale& locale, int32_t kind, Locale* actualReturn, UErrorCode& status) ثابت;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API virtual URegistry_ئاچقۇچ registerInstance(UObject* objToAdopt, ثابت Locale& locale, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API virtual URegistry_ئاچقۇچ registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, UErrorCode& status);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API virtual URegistry_ئاچقۇچ registerInstance(UObject* objToAdopt, ثابت Locale& locale, int32_t kind, int32_t coverage, UErrorCode& status);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API virtual URegistry_ئاچقۇچ registerInstance(UObject* objToAdopt, ثابت UnicodeString& locale, UBool visible, UErrorCode& status) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API virtual StringEnumeration* getAvailableLocales() ثابت U_LIFETIME_BOUND;

 محمي:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ثابت UnicodeString& validateFallbackLocale() ثابت;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API virtual ICUService_ئاچقۇچ* create_ئاچقۇچ(ثابت UnicodeString* id, UErrorCode& status) ثابت override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API virtual ICUService_ئاچقۇچ* create_ئاچقۇچ(ثابت UnicodeString* id, int32_t kind, UErrorCode& status) ثابت;

  friend class ServiceEnumeration;
};

U_NAMESPACE_END

    /* UCONFIG_NO_SERVICE */
#endif

    /* ICULSERV_H */
#endif

