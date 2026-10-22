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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ICULSERV_H
#ವ್ಯಾಖ್ಯಾನಿಸು ICULSERV_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ UCONFIG_NO_SERVICE

U_NAMESPACE_BEGIN

/*
 * Allow the declaration of APIs with pointers to ICUService
 * even when service is removed from the build.
 */
class ICULocaleService;

U_NAMESPACE_ಕೊನೆ

#ಇಲ್ಲದಿದ್ದರೆ

#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"

#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ಅನ್ತರ್ಭಾವ "serv.h"
#ಅನ್ತರ್ಭಾವ "locutil.h"

U_NAMESPACE_BEGIN

class ICULocaleService;

class Locale_ಕೀಲಿ;
class Locale_ಕೀಲಿ_Factory;
class Simple_Locale_ಕೀಲಿ_Factory;
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

class U_COMMON_API Locale_ಕೀಲಿ : ಸಾರ್ವಜನಿಕ ICUService_ಕೀಲಿ {
  ಖಾಸಗಿ: 
    int32_t ಬಗೆ_2;
    UnicodeString _primaryID;
    UnicodeString _fallbackID;
    UnicodeString _currentID;

  ಸಾರ್ವಜನಿಕ:
    ಎಣಿಕೆ {
        ಬಗೆ_ANY = -1
    };

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ಸ್ಥಿರ Locale_ಕೀಲಿ* createWithCanonicalFallback(ಸ್ಥಿರಾಂಕ UnicodeString* primaryID, 
                                                  ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID,
                                                  UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

    /**
     * Create a LocaleKey with canonical primary and fallback IDs.
     */
    ಸ್ಥಿರ Locale_ಕೀಲಿ* createWithCanonicalFallback(ಸ್ಥಿರಾಂಕ UnicodeString* primaryID, 
                                                  ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID, 
                                                  int32_t ಬಗೆ,
                                                  UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

  ಸಂರಕ್ಷಿತ:
    /**
     * PrimaryID is the user's requested locale string,
     * canonicalPrimaryID is this string in canonical form,
     * fallbackID is the current default locale's string in
     * canonical form.
     */
    Locale_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString& primaryID, 
              ಸ್ಥಿರಾಂಕ UnicodeString& canonicalPrimaryID, 
              ಸ್ಥಿರಾಂಕ UnicodeString* canonicalFallbackID, 
              int32_t ಬಗೆ);

 ಸಾರ್ವಜನಿಕ:
    /**
     * Append the prefix associated with the kind, or nothing if the kind is KIND_ANY.
     */
    ವಾಸ್ತವಿಕ UnicodeString& ಪೂರ್ವಪ್ರತ್ಯಯ(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ override;

    /**
     * Return the kind code associated with this key.
     */
    ವಾಸ್ತವಿಕ int32_t ಬಗೆ() ಸ್ಥಿರಾಂಕ;

    /**
     * Return the canonicalID.
     */
    ವಾಸ್ತವಿಕ UnicodeString& canonicalID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ override;

    /**
     * Return the currentID.
     */
    ವಾಸ್ತವಿಕ UnicodeString& currentID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ override;

    /**
     * Return the (canonical) current descriptor, or null if no current id.
     */
    ವಾಸ್ತವಿಕ UnicodeString& currentDescriptor(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ override;

    /**
     * Convenience method to return the locale corresponding to the (canonical) original ID.
     */
    ವಾಸ್ತವಿಕ Locale& canonicalLocale(Locale& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;

    /**
     * Convenience method to return the locale corresponding to the (canonical) current ID.
     */
    ವಾಸ್ತವಿಕ Locale& currentLocale(Locale& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;

    /**
     * If the key has a fallback, modify the key and return true,
     * otherwise return false.</p>
     *
     * <p>First falls back through the primary ID, then through
     * the fallbackID.  The final fallback is the empty string,
     * unless the primary id was the empty string, in which case
     * there is no fallback.  
     */
    ವಾಸ್ತವಿಕ UBool fallback() override;

    /**
     * Return true if a key created from id matches, or would eventually
     * fallback to match, the canonical ID of this key.  
     */
    ವಾಸ್ತವಿಕ UBool is_Fallback_ದ(ಸ್ಥಿರಾಂಕ UnicodeString& id) ಸ್ಥಿರಾಂಕ override;
    
 ಸಾರ್ವಜನಿಕ:
    /**
     * UObject boilerplate.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

    /**
     * Destructor.
     */
    ವಾಸ್ತವಿಕ ~Locale_ಕೀಲಿ();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
 ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ UnicodeString& debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ UnicodeString& debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ

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
class U_COMMON_API Locale_ಕೀಲಿ_Factory : ಸಾರ್ವಜನಿಕ ICUServiceFactory {
ಸಂರಕ್ಷಿತ:
    ಸ್ಥಿರಾಂಕ UnicodeString _name;
    ಸ್ಥಿರಾಂಕ int32_t _coverage;

ಸಾರ್ವಜನಿಕ:
    ಎಣಿಕೆ {
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
    ವಾಸ್ತವಿಕ ~Locale_ಕೀಲಿ_Factory();

ಸಂರಕ್ಷಿತ:
    /**
     * Constructor used by subclasses.
     */
    Locale_ಕೀಲಿ_Factory(int32_t coverage);

    /**
     * Constructor used by subclasses.
     */
    Locale_ಕೀಲಿ_Factory(int32_t coverage, ಸ್ಥಿರಾಂಕ UnicodeString& name);

    /**
     * Implement superclass abstract method.  This checks the currentID of
     * the key against the supported IDs, and passes the canonicalLocale and
     * kind off to handleCreate (which subclasses must implement).
     */
ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ UObject* create(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

ಸಂರಕ್ಷಿತ:
    ವಾಸ್ತವಿಕ UBool handles_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

ಸಾರ್ವಜನಿಕ:
    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ updateVisibleIDs(Hashtable& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

    /**
     * Return a localized name for the locale represented by id.
     */
    ವಾಸ್ತವಿಕ UnicodeString& getDisplayName(ಸ್ಥಿರಾಂಕ UnicodeString& id, ಸ್ಥಿರಾಂಕ Locale& locale, UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ override;

ಸಂರಕ್ಷಿತ:
    /**
     * Utility method used by create(ICUServiceKey, ICUService).  Subclasses can implement
     * this instead of create.  The default returns nullptr.
     */
    ವಾಸ್ತವಿಕ UObject* handleCreate(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t ಬಗೆ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

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
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ Hashtable* getSupportedIDs(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

ಸಾರ್ವಜನಿಕ:
    /**
     * UObject boilerplate.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
 ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ UnicodeString& debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ UnicodeString& debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ

};

/*
 ******************************************************************
 */

/**
 * A LocaleKeyFactory that just returns a single object for a kind/locale.
 */

class U_COMMON_API Simple_Locale_ಕೀಲಿ_Factory : ಸಾರ್ವಜನಿಕ Locale_ಕೀಲಿ_Factory {
 ಖಾಸಗಿ:
    UObject* _obj;
    UnicodeString _id;
    ಸ್ಥಿರಾಂಕ int32_t ಬಗೆ_2;

 ಸಾರ್ವಜನಿಕ:
    Simple_Locale_ಕೀಲಿ_Factory(UObject* obj_ಗಮ್ಯ_Adopt, 
                           ಸ್ಥಿರಾಂಕ UnicodeString& locale, 
                           int32_t ಬಗೆ, 
                           int32_t coverage);

    Simple_Locale_ಕೀಲಿ_Factory(UObject* obj_ಗಮ್ಯ_Adopt, 
                           ಸ್ಥಿರಾಂಕ Locale& locale, 
                           int32_t ಬಗೆ, 
                           int32_t coverage);

    /**
     * Destructor.
     */
    ವಾಸ್ತವಿಕ ~Simple_Locale_ಕೀಲಿ_Factory();

    /**
     * Override of superclass method.  Returns the service object if kind/locale match.  Service is not used.
     */
    ವಾಸ್ತವಿಕ UObject* create(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

    /**
     * Override of superclass method.  This adjusts the result based
     * on the coverage rule for this factory.
     */
    ವಾಸ್ತವಿಕ ಶೂನ್ಯ updateVisibleIDs(Hashtable& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

 ಸಂರಕ್ಷಿತ:
    /**
     * Return true if this id is equal to the locale name.
     */
    //virtual UBool isSupportedID(const UnicodeString& id, UErrorCode& status) const;


ಸಾರ್ವಜನಿಕ:
    /**
     * UObject boilerplate.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();

    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
 ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ UnicodeString& debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ UnicodeString& debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ

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
class U_COMMON_API ICUResourceBundleFactory : ಸಾರ್ವಜನಿಕ Locale_ಕೀಲಿ_Factory 
{
 ಸಂರಕ್ಷಿತ:
    UnicodeString _bundleName;

 ಸಾರ್ವಜನಿಕ:
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
    ICUResourceBundleFactory(ಸ್ಥಿರಾಂಕ UnicodeString& bundleName);

    /**
     * Destructor
     */
    ವಾಸ್ತವಿಕ ~ICUResourceBundleFactory();

ಸಂರಕ್ಷಿತ:
    /**
     * Return the supported IDs.  This is the set of all locale names in ICULocaleData.
     */
    ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ Hashtable* getSupportedIDs(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

    /**
     * Create the service.  The default implementation returns the resource bundle
     * for the locale, ignoring kind, and service.
     */
    ವಾಸ್ತವಿಕ UObject* handleCreate(ಸ್ಥಿರಾಂಕ Locale& loc, int32_t ಬಗೆ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

ಸಾರ್ವಜನಿಕ:
    /**
     * UObject boilerplate.
     */
    ಸ್ಥಿರ UClassID U_EXPORT2 getStaticClassID();
    ವಾಸ್ತವಿಕ UClassID getDynamicClassID() ಸ್ಥಿರಾಂಕ override;


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
 ಸಾರ್ವಜನಿಕ:
    ವಾಸ್ತವಿಕ UnicodeString& debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
    ವಾಸ್ತವಿಕ UnicodeString& debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ

};

/*
 ******************************************************************
 */

class U_COMMON_API_CLASS ICULocaleService : ಸಾರ್ವಜನಿಕ ICUService
{
 ಖಾಸಗಿ:
  Locale fallbackLocale;
  UnicodeString fallbackLocaleName;

 ಸಾರ್ವಜನಿಕ:
  /**
   * Construct an ICULocaleService.
   */
  U_COMMON_API ICULocaleService();

  /**
   * Construct an ICULocaleService with a name (useful for debugging).
   */
  U_COMMON_API ICULocaleService(ಸ್ಥಿರಾಂಕ UnicodeString& name);

  /**
   * Destructor.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ ~ICULocaleService();

#ಆದರೆ 0
  // redeclare because of overload resolution rules?
  // no, causes ambiguities since both UnicodeString and Locale have constructors that take a const char*
  // need some compiler flag to remove warnings 
  UObject* get(ಸ್ಥಿರಾಂಕ UnicodeString& descriptor, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ICUService::get(descriptor, ಸ್ಥಿತಿ_2);
  }

  UObject* get(ಸ್ಥಿರಾಂಕ UnicodeString& descriptor, UnicodeString* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ICUService::get(descriptor, actualReturn, ಸ್ಥಿತಿ_2);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with KIND_ANY for kind and null for
   * actualReturn.
   */
  U_COMMON_API UObject* get(ಸ್ಥಿರಾಂಕ Locale& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

  /**
   * Convenience override for callers using locales.  This calls
   * get(Locale, int, Locale[]) with a null actualReturn.
   */
  U_COMMON_API UObject* get(ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

  /**
   * Convenience override for callers using locales. This calls
   * get(Locale, String, Locale[]) with a null kind.
   */
  U_COMMON_API UObject* get(ಸ್ಥಿರಾಂಕ Locale& locale, Locale* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;
                   
  /**
   * Convenience override for callers using locales.  This uses
   * createKey(Locale.toString(), kind) to create a key, calls getKey, and then
   * if actualReturn is not null, returns the actualResult from
   * getKey (stripping any prefix) into a Locale.  
   */
  U_COMMON_API UObject* get(ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, Locale* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

  /**
   * Convenience override for callers using locales.  This calls
   * registerObject(Object, Locale, int32_t kind, int coverage)
   * passing KIND_ANY for the kind, and VISIBLE for the coverage.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ URegistry_ಕೀಲಿ ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

  /**
   * Convenience function for callers using locales.  This calls
   * registerObject(Object, Locale, int kind, int coverage)
   * passing VISIBLE for the coverage.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ URegistry_ಕೀಲಿ ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);

  /**
   * Convenience function for callers using locales.  This  instantiates
   * a SimpleLocaleKeyFactory, and registers the factory.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ URegistry_ಕೀಲಿ ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ Locale& locale, int32_t ಬಗೆ, int32_t coverage, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2);


  /**
   * (Stop compiler from complaining about hidden overrides.)
   * Since both UnicodeString and Locale have constructors that take const char*, adding a public
   * method that takes UnicodeString causes ambiguity at call sites that use const char*.
   * We really need a flag that is understood by all compilers that will suppress the warning about
   * hidden overrides.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ URegistry_ಕೀಲಿ ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& locale, UBool visible, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) override;

  /**
   * Convenience method for callers using locales.  This returns the standard
   * service ID enumeration.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ StringEnumeration* get_ಲಭ್ಯ_Locales() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

 ಸಂರಕ್ಷಿತ:

  /**
   * Return the name of the current fallback locale.  If it has changed since this was
   * last accessed, the service cache is cleared.
   */
  U_COMMON_API ಸ್ಥಿರಾಂಕ UnicodeString& validateFallbackLocale() ಸ್ಥಿರಾಂಕ;

  /**
   * Override superclass createKey method.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ ICUService_ಕೀಲಿ* create_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString* id, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

  /**
   * Additional createKey that takes a kind.
   */
  U_COMMON_API ವಾಸ್ತವಿಕ ICUService_ಕೀಲಿ* create_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString* id, int32_t ಬಗೆ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ;

  ಮಿತ್ರ class ServiceEnumeration;
};

U_NAMESPACE_ಕೊನೆ

    /* UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ

    /* ICULSERV_H */
#ಆದರೆ_ಅಂತ್ಯ

