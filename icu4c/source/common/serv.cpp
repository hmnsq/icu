// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"

#ಆದರೆ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "serv.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ಸ್ಥಿರಾಂಕ char16_t ICUService_ಕೀಲಿ::ಪೂರ್ವಪ್ರತ್ಯಯ_DELIMITER = 0x002F;   /* '/' */

ICUService_ಕೀಲಿ::ICUService_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString& id) 
: _id(id) {
}

ICUService_ಕೀಲಿ::~ICUService_ಕೀಲಿ() 
{
}

ಸ್ಥಿರಾಂಕ UnicodeString& 
ICUService_ಕೀಲಿ::getID() ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ _id;
}

UnicodeString& 
ICUService_ಕೀಲಿ::canonicalID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(_id);
}

UnicodeString& 
ICUService_ಕೀಲಿ::currentID(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ canonicalID(ಫಲಿತಾಂಶ);
}

UnicodeString& 
ICUService_ಕೀಲಿ::currentDescriptor(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಪೂರ್ವಪ್ರತ್ಯಯ(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append(ಪೂರ್ವಪ್ರತ್ಯಯ_DELIMITER);
    ಹಿಂತಿರುಗಿ currentID(ಫಲಿತಾಂಶ);
}

UBool 
ICUService_ಕೀಲಿ::fallback() 
{
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UBool 
ICUService_ಕೀಲಿ::is_Fallback_ದ(ಸ್ಥಿರಾಂಕ UnicodeString& id) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ id == _id;
}

UnicodeString& 
ICUService_ಕೀಲಿ::ಪೂರ್ವಪ್ರತ್ಯಯ(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString& 
ICUService_ಕೀಲಿ::parse_ಪೂರ್ವಪ್ರತ್ಯಯ(UnicodeString& ಫಲಿತಾಂಶ) 
{
    int32_t n = ಫಲಿತಾಂಶ.ಸೂಚ್ಯಂಕ_ದ(ಪೂರ್ವಪ್ರತ್ಯಯ_DELIMITER);
    ಆದರೆ (n < 0) {
        n = 0;
    }
    ಫಲಿತಾಂಶ.remove(n);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString& 
ICUService_ಕೀಲಿ::parseSuffix(UnicodeString& ಫಲಿತಾಂಶ) 
{
    int32_t n = ಫಲಿತಾಂಶ.ಸೂಚ್ಯಂಕ_ದ(ಪೂರ್ವಪ್ರತ್ಯಯ_DELIMITER);
    ಆದರೆ (n >= 0) {
        ಫಲಿತಾಂಶ.remove(0, n+1);
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString& 
ICUService_ಕೀಲಿ::debug(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    debugClass(ಫಲಿತಾಂಶ);
    ಫಲಿತಾಂಶ.append((UnicodeString)" id: ");
    ಫಲಿತಾಂಶ.append(_id);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString& 
ICUService_ಕೀಲಿ::debugClass(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append((UnicodeString)"ICUServiceKey");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ಕೀಲಿ)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& id, UBool visible) 
: _instance(instance_ಗಮ್ಯ_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    ಅಳಿಸು _instance;
}

UObject* 
SimpleFactory::create(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ICUService* service, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        UnicodeString ತಾತ್ಕಾಲಿಕ;
        ಆದರೆ (_id == ಕೀಲಿ.currentID(ತಾತ್ಕಾಲಿಕ)) {
            ಹಿಂತಿರುಗಿ service->cloneInstance(_instance); 
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ 
SimpleFactory::updateVisibleIDs(Hashtable& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಆದರೆ (_visible) {
        ಫಲಿತಾಂಶ.put(_id, (ಶೂನ್ಯ*)ಇದು, ಸ್ಥಿತಿ_2); // cast away const
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ಸ್ಥಿರಾಂಕ UnicodeString& id, ಸ್ಥಿರಾಂಕ Locale& /* locale */, UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಆದರೆ (_visible && _id == id) {
        ಫಲಿತಾಂಶ = _id;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& ಗಮ್ಯ_Append_ಗಮ್ಯ) ಸ್ಥಿರಾಂಕ 
{
    debugClass(ಗಮ್ಯ_Append_ಗಮ್ಯ);
    ಗಮ್ಯ_Append_ಗಮ್ಯ.append((UnicodeString)" id: ");
    ಗಮ್ಯ_Append_ಗಮ್ಯ.append(_id);
    ಗಮ್ಯ_Append_ಗಮ್ಯ.append((UnicodeString)", visible: ");
    ಗಮ್ಯ_Append_ಗಮ್ಯ.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_Append_ಗಮ್ಯ;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& ಗಮ್ಯ_Append_ಗಮ್ಯ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_Append_ಗಮ್ಯ.append((UnicodeString)"SimpleFactory");
}
#ಆದರೆ_ಅಂತ್ಯ

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(SimpleFactory)

/*
******************************************************************
*/

ServiceListener::~ServiceListener() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ServiceListener)

/*
******************************************************************
*/

// Record the actual id for this service in the cache, so we can return it
// even if we succeed later with a different id.
ವರ್ಗ Cache_ನಮೂದು : ಸಾರ್ವಜನಿಕ UMemory {
ಖಾಸಗಿ:
    int32_t refcount;

ಸಾರ್ವಜನಿಕ:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_ನಮೂದು() {
        ಅಳಿಸು service;
    }

    Cache_ನಮೂದು(ಸ್ಥಿರಾಂಕ UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_ನಮೂದು* ref() {
        ++refcount;
        ಹಿಂತಿರುಗಿ ಇದು;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_ನಮೂದು* unref() {
        ಆದರೆ ((--refcount) == 0) {
            ಅಳಿಸು ಇದು;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ ಇದು;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
cacheDeleter(ಶೂನ್ಯ* obj) {
    U_NAMESPACE_USE ((Cache_ನಮೂದು*)obj)->unref();
}

U_CDECL_ಕೊನೆ

/*
******************************************************************
*/

ವರ್ಗ DNCache : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    Hashtable cache;
    ಸ್ಥಿರಾಂಕ Locale locale;

    DNCache(ಸ್ಥಿರಾಂಕ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ಸ್ಥಿರಾಂಕ UnicodeString& displayName, 
                   ಸ್ಥಿರಾಂಕ UnicodeString& id,
                   UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        StringPair* sp = ಹೊಸ StringPair(displayName, id);
        ಆದರೆ (sp == ಶೂನ್ಯಸೂಚಕ || sp->isBogus()) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಅಳಿಸು sp;
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
        ಹಿಂತಿರುಗಿ sp;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

UBool 
StringPair::isBogus() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ಸ್ಥಿರಾಂಕ UnicodeString& _displayName, 
                       ಸ್ಥಿರಾಂಕ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
userv_deleteStringPair(ಶೂನ್ಯ *obj) {
    U_NAMESPACE_USE ಅಳಿಸು (StringPair*) obj;
}
U_CDECL_ಕೊನೆ

/*
******************************************************************
*/

ಸ್ಥಿರ UMutex ಬೀಗ;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(ಶೂನ್ಯಸೂಚಕ)
, serviceCache(ಶೂನ್ಯಸೂಚಕ)
, idCache(ಶೂನ್ಯಸೂಚಕ)
, dnCache(ಶೂನ್ಯಸೂಚಕ)
{
}

ICUService::ICUService(ಸ್ಥಿರಾಂಕ UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(ಶೂನ್ಯಸೂಚಕ)
, serviceCache(ಶೂನ್ಯಸೂಚಕ)
, idCache(ಶೂನ್ಯಸೂಚಕ)
, dnCache(ಶೂನ್ಯಸೂಚಕ)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&ಬೀಗ);
        clearCaches();
        ಅಳಿಸು factories;
        factories = ಶೂನ್ಯಸೂಚಕ;
    }
}

UObject* 
ICUService::get(ಸ್ಥಿರಾಂಕ UnicodeString& descriptor, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ get(descriptor, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

UObject* 
ICUService::get(ಸ್ಥಿರಾಂಕ UnicodeString& descriptor, UnicodeString* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    UObject* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&descriptor, ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಕೀಲಿ) {
        ಫಲಿತಾಂಶ = get_ಕೀಲಿ(*ಕೀಲಿ, actualReturn, ಸ್ಥಿತಿ_2);
        ಅಳಿಸು ಕೀಲಿ;
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ get_ಕೀಲಿ(ಕೀಲಿ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* actualReturn, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ get_ಕೀಲಿ(ಕೀಲಿ, actualReturn, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
ವರ್ಗ XMutex : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸಾಲೊಳಗೆ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_ಸಕ್ರಿಯ(!reentering) 
    {
        ಆದರೆ (f_ಸಕ್ರಿಯ) umtx_ಬೀಗ(fMutex);
    }
    ಸಾಲೊಳಗೆ ~XMutex() {
        ಆದರೆ (f_ಸಕ್ರಿಯ) umtx_unlock(fMutex);
    }

ಖಾಸಗಿ:
    UMutex  *fMutex;
    UBool f_ಸಕ್ರಿಯ;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* actualReturn, ಸ್ಥಿರಾಂಕ ICUServiceFactory* factory, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (isDefault()) {
        ಹಿಂತಿರುಗಿ handleDefault(ಕೀಲಿ, actualReturn, ಸ್ಥಿತಿ_2);
    }

    ICUService* ncthis = const_cast<ICUService*>(ಇದು); // cast away semantic const

    Cache_ನಮೂದು* ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&ಬೀಗ, factory != ಶೂನ್ಯಸೂಚಕ);

        ಆದರೆ (serviceCache == ಶೂನ್ಯಸೂಚಕ) {
            ncthis->serviceCache = ಹೊಸ Hashtable(ಸ್ಥಿತಿ_2);
            ಆದರೆ (ncthis->serviceCache == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು serviceCache;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            serviceCache->ಗಣ_ಮೌಲ್ಯ_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_ಸೂಚಕ<UVector> cacheDescriptorList;
        UBool put_ಒಳ_Cache = ಸುಳ್ಳು;

        int32_t ಆರಂಭ_ಸೂಚ್ಯಂಕ = 0;
        int32_t ಎಲ್ಲೆ = factories->size();
        UBool cache_ಫಲಿತಾಂಶ = ನಿಜ;

        ಆದರೆ (factory != ಶೂನ್ಯಸೂಚಕ) {
            ಗಾಗಿ (int32_t i = 0; i < ಎಲ್ಲೆ; ++i) {
                ಆದರೆ (factory == static_cast<ಸ್ಥಿರಾಂಕ ICUServiceFactory*>(factories->elementAt(i))) {
                    ಆರಂಭ_ಸೂಚ್ಯಂಕ = i + 1;
                    ನಿಲ್ಲಿಸು;
                }
            }
            ಆದರೆ (ಆರಂಭ_ಸೂಚ್ಯಂಕ == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            cache_ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
        }

        ಮಾಡು {
            currentDescriptor.remove();
            ಕೀಲಿ.currentDescriptor(currentDescriptor);
            ಫಲಿತಾಂಶ = static_cast<Cache_ನಮೂದು*>(serviceCache->get(currentDescriptor));
            ಆದರೆ (ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
                ನಿಲ್ಲಿಸು;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_ಒಳ_Cache = ನಿಜ;

            int32_t ಸೂಚ್ಯಂಕ = ಆರಂಭ_ಸೂಚ್ಯಂಕ;
            ಆಗ (ಸೂಚ್ಯಂಕ < ಎಲ್ಲೆ) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(ಸೂಚ್ಯಂಕ++));
                Local_ಸೂಚಕ<UObject> service(f->create(ಕೀಲಿ, ಇದು, ಸ್ಥಿತಿ_2));
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
                ಆದರೆ (service.isValid()) {
                    ಫಲಿತಾಂಶ = ಹೊಸ Cache_ನಮೂದು(currentDescriptor, service.getAlias());
                    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
                        ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    }
                    service.orphan(); // result now owns service.

                    ಹೋಗು outer_ಕೊನೆ;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ಆದರೆ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_ಸಂಕೇತ(ಹೊಸ UVector(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, 5, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
            }

            Local_ಸೂಚಕ<UnicodeString> id_ಗಮ್ಯ_Cache(ಹೊಸ UnicodeString(currentDescriptor), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            ಆದರೆ (id_ಗಮ್ಯ_Cache->isBogus()) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            cacheDescriptorList->adoptElement(id_ಗಮ್ಯ_Cache.orphan(), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
        } ಆಗ (ಕೀಲಿ.fallback());
outer_ಕೊನೆ:

        ಆದರೆ (ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            ಆದರೆ (put_ಒಳ_Cache && cache_ಫಲಿತಾಂಶ) {
                serviceCache->put(ಫಲಿತಾಂಶ->actualDescriptor, ಫಲಿತಾಂಶ, ಸ್ಥಿತಿ_2);
                ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }

                ಆದರೆ (cacheDescriptorList.isValid()) {
                    ಗಾಗಿ (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, ಫಲಿತಾಂಶ, ಸ್ಥಿತಿ_2);
                        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                        }

                        ಫಲಿತಾಂಶ->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ಆದರೆ (actualReturn != ಶೂನ್ಯಸೂಚಕ) {
                // strip null prefix
                ಆದರೆ (ಫಲಿತಾಂಶ->actualDescriptor.ಸೂಚ್ಯಂಕ_ದ(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(ಫಲಿತಾಂಶ->actualDescriptor, 
                        1, 
                        ಫಲಿತಾಂಶ->actualDescriptor.length() - 1);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    *actualReturn = ಫಲಿತಾಂಶ->actualDescriptor;
                }

                ಆದರೆ (actualReturn->isBogus()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಅಳಿಸು ಫಲಿತಾಂಶ;
                    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                }
            }

            UObject* service = cloneInstance(ಫಲಿತಾಂಶ->service);
            ಆದರೆ (put_ಒಳ_Cache && !cache_ಫಲಿತಾಂಶ) {
                ಅಳಿಸು ಫಲಿತಾಂಶ;
            }
            ಹಿಂತಿರುಗಿ service;
        }
    }

    ಹಿಂತಿರುಗಿ handleDefault(ಕೀಲಿ, actualReturn, ಸ್ಥಿತಿ_2);
}

UObject* 
ICUService::handleDefault(ಸ್ಥಿರಾಂಕ ICUService_ಕೀಲಿ& /*ಕೀಲಿ*/, UnicodeString* /* actualIDReturn */, UError_ಸಂಕೇತ& /* status */) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

UVector& 
ICUService::getVisibleIDs(UVector& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ getVisibleIDs(ಫಲಿತಾಂಶ, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

UVector& 
ICUService::getVisibleIDs(UVector& ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ UnicodeString* ಹೊಂದಿಕೆ_ID, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಫಲಿತಾಂಶ.removeAllElements();

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    UObjectDeleter *savedDeleter = ಫಲಿತಾಂಶ.ಗಣ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&ಬೀಗ);
        ಸ್ಥಿರಾಂಕ Hashtable* map = getVisibleIDMap(ಸ್ಥಿತಿ_2);
        ಆದರೆ (map != ಶೂನ್ಯಸೂಚಕ) {
            ICUService_ಕೀಲಿ* fallback_ಕೀಲಿ = create_ಕೀಲಿ(ಹೊಂದಿಕೆ_ID, ಸ್ಥಿತಿ_2);

            ಗಾಗಿ (int32_t pos = UHASH_ಮೊದಲ; U_SUCCESS(ಸ್ಥಿತಿ_2); ) {
                ಸ್ಥಿರಾಂಕ UHashElement* e = map->ಮುಂದಿನ_Element(pos);
                ಆದರೆ (e == ಶೂನ್ಯಸೂಚಕ) {
                    ನಿಲ್ಲಿಸು;
                }

                ಸ್ಥಿರಾಂಕ UnicodeString* id = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(e->ಕೀಲಿ.ಸೂಚಕ);
                ಆದರೆ (fallback_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
                    ಆದರೆ (!fallback_ಕೀಲಿ->is_Fallback_ದ(*id)) {
                        ಮುಂದುವರಿ;
                    }
                }

                Local_ಸೂಚಕ<UnicodeString> idClone(id->clone(), ಸ್ಥಿತಿ_2);
                ಫಲಿತಾಂಶ.adoptElement(idClone.orphan(), ಸ್ಥಿತಿ_2);
            }
            ಅಳಿಸು fallback_ಕೀಲಿ;
        }
    }
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಫಲಿತಾಂಶ.removeAllElements();
    }
    ಫಲಿತಾಂಶ.ಗಣ_Deleter(savedDeleter);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ Hashtable* 
ICUService::getVisibleIDMap(UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ಇದು); // cast away semantic const
    ಆದರೆ (idCache == ಶೂನ್ಯಸೂಚಕ) {
        ncthis->idCache = ಹೊಸ Hashtable(ಸ್ಥಿತಿ_2);
        ಆದರೆ (idCache == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (factories != ಶೂನ್ಯಸೂಚಕ) {
            ಗಾಗಿ (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, ಸ್ಥಿತಿ_2);
            }
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಅಳಿಸು idCache;
                ncthis->idCache = ಶೂನ್ಯಸೂಚಕ;
            }
        }
    }

    ಹಿಂತಿರುಗಿ idCache;
}


UnicodeString& 
ICUService::getDisplayName(ಸ್ಥಿರಾಂಕ UnicodeString& id, UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ getDisplayName(id, ಫಲಿತಾಂಶ, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ಸ್ಥಿರಾಂಕ UnicodeString& id, UnicodeString& ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ Locale& locale) ಸ್ಥಿರಾಂಕ 
{
    {
        UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
        Mutex mutex(&ಬೀಗ);
        ಸ್ಥಿರಾಂಕ Hashtable* map = getVisibleIDMap(ಸ್ಥಿತಿ_2);
        ಆದರೆ (map != ಶೂನ್ಯಸೂಚಕ) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ಆದರೆ (f != ಶೂನ್ಯಸೂಚಕ) {
                f->getDisplayName(id, locale, ಫಲಿತಾಂಶ);
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }

            // fallback
            ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
            ICUService_ಕೀಲಿ* fallback_ಕೀಲಿ = create_ಕೀಲಿ(&id, ಸ್ಥಿತಿ_2);
            ಆಗ (fallback_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ && fallback_ಕೀಲಿ->fallback()) {
                UnicodeString us;
                fallback_ಕೀಲಿ->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ಆದರೆ (f != ಶೂನ್ಯಸೂಚಕ) {
                    f->getDisplayName(id, locale, ಫಲಿತಾಂಶ);
                    ಅಳಿಸು fallback_ಕೀಲಿ;
                    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
                }
            }
            ಅಳಿಸು fallback_ಕೀಲಿ;
        }
    }
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UVector& 
ICUService::get_Display_ಹೆಸರುಗಳು(UVector& ಫಲಿತಾಂಶ, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ get_Display_ಹೆಸರುಗಳು(ಫಲಿತಾಂಶ, Locale::getDefault(), ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}


UVector& 
ICUService::get_Display_ಹೆಸರುಗಳು(UVector& ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ Locale& locale, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ get_Display_ಹೆಸರುಗಳು(ಫಲಿತಾಂಶ, locale, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2);
}

UVector& 
ICUService::get_Display_ಹೆಸರುಗಳು(UVector& ಫಲಿತಾಂಶ, 
                            ಸ್ಥಿರಾಂಕ Locale& locale, 
                            ಸ್ಥಿರಾಂಕ UnicodeString* ಹೊಂದಿಕೆ_ID, 
                            UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಫಲಿತಾಂಶ.removeAllElements();
    ಫಲಿತಾಂಶ.ಗಣ_Deleter(userv_deleteStringPair);
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ICUService* ncthis = const_cast<ICUService*>(ಇದು); // cast away semantic const
        Mutex mutex(&ಬೀಗ);

        ಆದರೆ (dnCache != ಶೂನ್ಯಸೂಚಕ && dnCache->locale != locale) {
            ಅಳಿಸು dnCache;
            ncthis->dnCache = ಶೂನ್ಯಸೂಚಕ;
        }

        ಆದರೆ (dnCache == ಶೂನ್ಯಸೂಚಕ) {
            ಸ್ಥಿರಾಂಕ Hashtable* m = getVisibleIDMap(ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
            ncthis->dnCache = ಹೊಸ DNCache(locale); 
            ಆದರೆ (dnCache == ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }

            int32_t pos = UHASH_ಮೊದಲ;
            ಸ್ಥಿರಾಂಕ UHashElement* ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;
            ಆಗ ((ನಮೂದು = m->ಮುಂದಿನ_Element(pos)) != ಶೂನ್ಯಸೂಚಕ) {
                ಸ್ಥಿರಾಂಕ UnicodeString* id = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ನಮೂದು->ಕೀಲಿ.ಸೂಚಕ);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(ನಮೂದು->value.ಸೂಚಕ);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ಆದರೆ (dname.isBogus()) {
                    ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    dnCache->cache.put(dname, (ಶೂನ್ಯ*)id, ಸ್ಥಿತಿ_2); // share pointer with visibleIDMap
                    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
                        ಮುಂದುವರಿ;
                    }
                }
                ಅಳಿಸು dnCache;
                ncthis->dnCache = ಶೂನ್ಯಸೂಚಕ;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        }
    }

    ICUService_ಕೀಲಿ* ಹೊಂದಿಕೆ_ಕೀಲಿ = create_ಕೀಲಿ(ಹೊಂದಿಕೆ_ID, ಸ್ಥಿತಿ_2);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_ಮೊದಲ; 
    ಸ್ಥಿರಾಂಕ UHashElement *ನಮೂದು = ಶೂನ್ಯಸೂಚಕ;
    ಆಗ ((ನಮೂದು = dnCache->cache.ಮುಂದಿನ_Element(pos)) != ಶೂನ್ಯಸೂಚಕ) {
        ಸ್ಥಿರಾಂಕ UnicodeString* id = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ನಮೂದು->value.ಸೂಚಕ);
        ಆದರೆ (ಹೊಂದಿಕೆ_ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ && !ಹೊಂದಿಕೆ_ಕೀಲಿ->is_Fallback_ದ(*id)) {
            ಮುಂದುವರಿ;
        }
        ಸ್ಥಿರಾಂಕ UnicodeString* dn = static_cast<ಸ್ಥಿರಾಂಕ UnicodeString*>(ನಮೂದು->ಕೀಲಿ.ಸೂಚಕ);
        StringPair* sp = StringPair::create(*id, *dn, ಸ್ಥಿತಿ_2);
        ಫಲಿತಾಂಶ.adoptElement(sp, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಫಲಿತಾಂಶ.removeAllElements();
            ನಿಲ್ಲಿಸು;
        }
    }
    ಅಳಿಸು ಹೊಂದಿಕೆ_ಕೀಲಿ;

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

URegistry_ಕೀಲಿ
ICUService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& id, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) 
{
    ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Instance(obj_ಗಮ್ಯ_Adopt, id, ನಿಜ, ಸ್ಥಿತಿ_2);
}

URegistry_ಕೀಲಿ
ICUService::ರಿಜಿಸ್ಟರ್_Instance(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& id, UBool visible, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) 
{
    ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&id, ಸ್ಥಿತಿ_2);
    ಆದರೆ (ಕೀಲಿ != ಶೂನ್ಯಸೂಚಕ) {
        UnicodeString canonicalID;
        ಕೀಲಿ->canonicalID(canonicalID);
        ಅಳಿಸು ಕೀಲಿ;

        ICUServiceFactory* f = createSimpleFactory(obj_ಗಮ್ಯ_Adopt, canonicalID, visible, ಸ್ಥಿತಿ_2);
        ಆದರೆ (f != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ ರಿಜಿಸ್ಟರ್_Factory(f, ಸ್ಥಿತಿ_2);
        }
    }
    ಅಳಿಸು obj_ಗಮ್ಯ_Adopt;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_ಗಮ್ಯ_Adopt, ಸ್ಥಿರಾಂಕ UnicodeString& id, UBool visible, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        ಆದರೆ ((obj_ಗಮ್ಯ_Adopt != ಶೂನ್ಯಸೂಚಕ) && (!id.isBogus())) {
            ಹಿಂತಿರುಗಿ ಹೊಸ SimpleFactory(obj_ಗಮ್ಯ_Adopt, id, visible);
        }
        ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

URegistry_ಕೀಲಿ
ICUService::ರಿಜಿಸ್ಟರ್_Factory(ICUServiceFactory* factory_ಗಮ್ಯ_Adopt, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2)
{
    Local_ಸೂಚಕ<ICUServiceFactory>lp_Factory_ಗಮ್ಯ_Adopt(factory_ಗಮ್ಯ_Adopt);
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2) || factory_ಗಮ್ಯ_Adopt == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    {
        Mutex mutex(&ಬೀಗ);

        ಆದರೆ (factories == ಶೂನ್ಯಸೂಚಕ) {
            Local_ಸೂಚಕ<UVector> lpFactories(ಹೊಸ UVector(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);
            ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
                ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
            }
            factories = lpFactories.orphan();
        }
        factories->ಸೇರಿಸು_Element_At(lp_Factory_ಗಮ್ಯ_Adopt.orphan(), 0, ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ಆದರೆ (U_SUCCESS(ಸ್ಥಿತಿ_2)) {
        notify_ಬದಲಾಯಿತು();
        ಹಿಂತಿರುಗಿ (URegistry_ಕೀಲಿ)factory_ಗಮ್ಯ_Adopt;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
}

UBool 
ICUService::unregister(URegistry_ಕೀಲಿ rkey, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool ಫಲಿತಾಂಶ = ಸುಳ್ಳು;
    ಆದರೆ (factory != ಶೂನ್ಯಸೂಚಕ && factories != ಶೂನ್ಯಸೂಚಕ) {
        Mutex mutex(&ಬೀಗ);

        ಆದರೆ (factories->removeElement(factory)) {
            clearCaches();
            ಫಲಿತಾಂಶ = ನಿಜ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
            ಅಳಿಸು factory;
        }
    }
    ಆದರೆ (ಫಲಿತಾಂಶ) {
        notify_ಬದಲಾಯಿತು();
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ 
ICUService::reset() 
{
    {
        Mutex mutex(&ಬೀಗ);
        reInitializeFactories();
        clearCaches();
    }
    notify_ಬದಲಾಯಿತು();
}

ಶೂನ್ಯ 
ICUService::reInitializeFactories() 
{
    ಆದರೆ (factories != ಶೂನ್ಯಸೂಚಕ) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_Factories() == 0;
}

ICUService_ಕೀಲಿ* 
ICUService::create_ಕೀಲಿ(ಸ್ಥಿರಾಂಕ UnicodeString* id, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ (U_FAILURE(ಸ್ಥಿತಿ_2) || id == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : ಹೊಸ ICUService_ಕೀಲಿ(*id);
}

ಶೂನ್ಯ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    ಅಳಿಸು dnCache;
    dnCache = ಶೂನ್ಯಸೂಚಕ;
    ಅಳಿಸು idCache;
    idCache = ಶೂನ್ಯಸೂಚಕ;
    ಅಳಿಸು serviceCache; serviceCache = ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    ಅಳಿಸು serviceCache; serviceCache = ಶೂನ್ಯಸೂಚಕ;
}

UBool 
ICUService::ಸ್ವೀಕರಿಸು_Listener(ಸ್ಥಿರಾಂಕ EventListener& l) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ dynamic_cast<ಸ್ಥಿರಾಂಕ ServiceListener*>(&l) != ಶೂನ್ಯಸೂಚಕ;
}

ಶೂನ್ಯ 
ICUService::notifyListener(EventListener& l) ಸ್ಥಿರಾಂಕ 
{
    (static_cast<ServiceListener&>(l)).service_ಬದಲಾಯಿತು(*ಇದು);
}

UnicodeString&
ICUService::getName(UnicodeString& ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(name);
}

int32_t 
ICUService::ಎಣಿಕೆ_Factories() ಸ್ಥಿರಾಂಕ 
{
    ಹಿಂತಿರುಗಿ factories == ಶೂನ್ಯಸೂಚಕ ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ಸ್ಥಿರಾಂಕ
{
    ಹಿಂತಿರುಗಿ timestamp;
}

U_NAMESPACE_ಕೊನೆ

/* UCONFIG_NO_SERVICE */
#ಆದರೆ_ಅಂತ್ಯ
