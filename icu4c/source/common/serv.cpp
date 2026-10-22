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

#ಯದಿ !UCONFIG_NO_SERVICE

#ಅನ್ತರ್ಭಾವ "serv.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ಧ್ರುವ char16_t ICUService_ಕೀಲಿ::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_ಕೀಲಿ::ICUService_ಕೀಲಿ(ಧ್ರುವ UnicodeString& id) 
: _id(id) {
}

ICUService_ಕೀಲಿ::~ICUService_ಕೀಲಿ() 
{
}

ಧ್ರುವ UnicodeString& 
ICUService_ಕೀಲಿ::getID() ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ _id;
}

UnicodeString& 
ICUService_ಕೀಲಿ::canonicalID(UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ result.append(_id);
}

UnicodeString& 
ICUService_ಕೀಲಿ::currentID(UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ canonicalID(result);
}

UnicodeString& 
ICUService_ಕೀಲಿ::currentDescriptor(UnicodeString& result) ಧ್ರುವ 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ಪ್ರತ್ಯಾಗಮ currentID(result);
}

UBool 
ICUService_ಕೀಲಿ::fallback() 
{
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

UBool 
ICUService_ಕೀಲಿ::isFallbackOf(ಧ್ರುವ UnicodeString& id) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ id == _id;
}

UnicodeString& 
ICUService_ಕೀಲಿ::prefix(UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString& 
ICUService_ಕೀಲಿ::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ಯದಿ (n < 0) {
        n = 0;
    }
    result.remove(n);
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString& 
ICUService_ಕೀಲಿ::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ಯದಿ (n >= 0) {
        result.remove(0, n+1);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_ಕೀಲಿ::debug(UnicodeString& result) ಧ್ರುವ 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ಪ್ರತ್ಯಾಗಮ result;
}

UnicodeString& 
ICUService_ಕೀಲಿ::debugClass(UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ಕೀಲಿ)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ಧ್ರುವ UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ಧ್ರುವ ICUService_ಕೀಲಿ& ಕೀಲಿ, ಧ್ರುವ ICUService* service, UErrorCode& status) ಧ್ರುವ 
{
    ಯದಿ (U_SUCCESS(status)) {
        UnicodeString temp;
        ಯದಿ (_id == ಕೀಲಿ.currentID(temp)) {
            ಪ್ರತ್ಯಾಗಮ service->cloneInstance(_instance); 
        }
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಶೂನ್ಯ 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ಧ್ರುವ 
{
    ಯದಿ (_visible) {
        result.put(_id, (ಶೂನ್ಯ*)ಏತತ್, status); // cast away const
    } ಅನ್ಯಥಾ {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ಧ್ರುವ UnicodeString& id, ಧ್ರುವ Locale& /* locale */, UnicodeString& result) ಧ್ರುವ 
{
    ಯದಿ (_visible && _id == id) {
        result = _id;
    } ಅನ್ಯಥಾ {
        result.setToBogus();
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ಧ್ರುವ 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ಪ್ರತ್ಯಾಗಮ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ toAppendTo.append((UnicodeString)"SimpleFactory");
}
#endif

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
ವರ್ಗ CacheEntry : ಸಾರ್ವಜನಿಕ UMemory {
ಗುಪ್ತ:
    int32_t refcount;

ಸಾರ್ವಜನಿಕ:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ಧ್ರುವ UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    CacheEntry* ref() {
        ++refcount;
        ಪ್ರತ್ಯಾಗಮ ಏತತ್;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        ಯದಿ ((--refcount) == 0) {
            delete ಏತತ್;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ ಏತತ್;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
cacheDeleter(ಶೂನ್ಯ* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

ವರ್ಗ DNCache : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    Hashtable cache;
    ಧ್ರುವ Locale locale;

    DNCache(ಧ್ರುವ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ಧ್ರುವ UnicodeString& displayName, 
                   ಧ್ರುವ UnicodeString& id,
                   UErrorCode& status)
{
    ಯದಿ (U_SUCCESS(status)) {
        StringPair* sp = ನೂತನ StringPair(displayName, id);
        ಯದಿ (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ಪ್ರತ್ಯಾಗಮ nullptr;
        }
        ಪ್ರತ್ಯಾಗಮ sp;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

UBool 
StringPair::isBogus() ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ಧ್ರುವ UnicodeString& _displayName, 
                       ಧ್ರುವ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
userv_deleteStringPair(ಶೂನ್ಯ *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

ಸ್ಥಿರ UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ಧ್ರುವ UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&lock);
        clearCaches();
        delete factories;
        factories = nullptr;
    }
}

UObject* 
ICUService::get(ಧ್ರುವ UnicodeString& descriptor, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ಧ್ರುವ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ಧ್ರುವ 
{
    UObject* result = nullptr;
    ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&descriptor, status);
    ಯದಿ (ಕೀಲಿ) {
        result = get_ಕೀಲಿ(*ಕೀಲಿ, actualReturn, status);
        delete ಕೀಲಿ;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ get_ಕೀಲಿ(ಕೀಲಿ, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* actualReturn, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ get_ಕೀಲಿ(ಕೀಲಿ, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
ವರ್ಗ XMutex : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಅನ್ತಃಸ್ಥ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        ಯದಿ (fActive) umtx_lock(fMutex);
    }
    ಅನ್ತಃಸ್ಥ ~XMutex() {
        ಯದಿ (fActive) umtx_unlock(fMutex);
    }

ಗುಪ್ತ:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ಕೀಲಿ(ICUService_ಕೀಲಿ& ಕೀಲಿ, UnicodeString* actualReturn, ಧ್ರುವ ICUServiceFactory* factory, UErrorCode& status) ಧ್ರುವ 
{
    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ (isDefault()) {
        ಪ್ರತ್ಯಾಗಮ handleDefault(ಕೀಲಿ, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(ಏತತ್); // cast away semantic const

    CacheEntry* result = nullptr;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&lock, factory != nullptr);

        ಯದಿ (serviceCache == nullptr) {
            ncthis->serviceCache = ನೂತನ Hashtable(status);
            ಯದಿ (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ಯದಿ (U_FAILURE(status)) {
                delete serviceCache;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            serviceCache->set_ಮೌಲ್ಯ_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = ಅಸತ್ಯ;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = ಸತ್ಯ;

        ಯದಿ (factory != nullptr) {
            ಪ್ರತಿ (int32_t i = 0; i < limit; ++i) {
                ಯದಿ (factory == static_cast<ಧ್ರುವ ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    ಭಙ್ಗ;
                }
            }
            ಯದಿ (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            cacheResult = ಅಸತ್ಯ;
        }

        ಕುರು {
            currentDescriptor.remove();
            ಕೀಲಿ.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            ಯದಿ (result != nullptr) {
                ಭಙ್ಗ;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = ಸತ್ಯ;

            int32_t index = startIndex;
            ಯಾವತ್ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(ಕೀಲಿ, ಏತತ್, status));
                ಯದಿ (U_FAILURE(status)) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
                ಯದಿ (service.isValid()) {
                    result = ನೂತನ CacheEntry(currentDescriptor, service.getAlias());
                    ಯದಿ (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ಪ್ರತ್ಯಾಗಮ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    ಗಚ್ಛ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ಯದಿ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(ನೂತನ UVector(uprv_deleteUObject, nullptr, 5, status), status);
                ಯದಿ (U_FAILURE(status)) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(ನೂತನ UnicodeString(currentDescriptor), status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            ಯದಿ (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
        } ಯಾವತ್ (ಕೀಲಿ.fallback());
outerEnd:

        ಯದಿ (result != nullptr) {
            ಯದಿ (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                ಯದಿ (U_FAILURE(status)) {
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }

                ಯದಿ (cacheDescriptorList.isValid()) {
                    ಪ್ರತಿ (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        ಯದಿ (U_FAILURE(status)) {
                            ಪ್ರತ್ಯಾಗಮ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ಯದಿ (actualReturn != nullptr) {
                // strip null prefix
                ಯದಿ (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } ಅನ್ಯಥಾ {
                    *actualReturn = result->actualDescriptor;
                }

                ಯದಿ (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ಪ್ರತ್ಯಾಗಮ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            ಯದಿ (putInCache && !cacheResult) {
                delete result;
            }
            ಪ್ರತ್ಯಾಗಮ service;
        }
    }

    ಪ್ರತ್ಯಾಗಮ handleDefault(ಕೀಲಿ, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ಧ್ರುವ ICUService_ಕೀಲಿ& /*ಕೀಲಿ*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ಧ್ರುವ UnicodeString* matchID, UErrorCode& status) ಧ್ರುವ 
{
    result.removeAllElements();

    ಯದಿ (U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ಧ್ರುವ Hashtable* map = getVisibleIDMap(status);
        ಯದಿ (map != nullptr) {
            ICUService_ಕೀಲಿ* fallback_ಕೀಲಿ = create_ಕೀಲಿ(matchID, status);

            ಪ್ರತಿ (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ಧ್ರುವ UHashElement* e = map->nextElement(pos);
                ಯದಿ (e == nullptr) {
                    ಭಙ್ಗ;
                }

                ಧ್ರುವ UnicodeString* id = static_cast<ಧ್ರುವ UnicodeString*>(e->ಕೀಲಿ.pointer);
                ಯದಿ (fallback_ಕೀಲಿ != nullptr) {
                    ಯದಿ (!fallback_ಕೀಲಿ->isFallbackOf(*id)) {
                        ಅನುವರ್ತ;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_ಕೀಲಿ;
        }
    }
    ಯದಿ (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ಪ್ರತ್ಯಾಗಮ result;
}

ಧ್ರುವ Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(status)) ಪ್ರತ್ಯಾಗಮ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ಏತತ್); // cast away semantic const
    ಯದಿ (idCache == nullptr) {
        ncthis->idCache = ನೂತನ Hashtable(status);
        ಯದಿ (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ ಯದಿ (factories != nullptr) {
            ಪ್ರತಿ (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            ಯದಿ (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ idCache;
}


UnicodeString& 
ICUService::getDisplayName(ಧ್ರುವ UnicodeString& id, UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ಧ್ರುವ UnicodeString& id, UnicodeString& result, ಧ್ರುವ Locale& locale) ಧ್ರುವ 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ಧ್ರುವ Hashtable* map = getVisibleIDMap(status);
        ಯದಿ (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ಯದಿ (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ಪ್ರತ್ಯಾಗಮ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_ಕೀಲಿ* fallback_ಕೀಲಿ = create_ಕೀಲಿ(&id, status);
            ಯಾವತ್ (fallback_ಕೀಲಿ != nullptr && fallback_ಕೀಲಿ->fallback()) {
                UnicodeString us;
                fallback_ಕೀಲಿ->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ಯದಿ (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_ಕೀಲಿ;
                    ಪ್ರತ್ಯಾಗಮ result;
                }
            }
            delete fallback_ಕೀಲಿ;
        }
    }
    result.setToBogus();
    ಪ್ರತ್ಯಾಗಮ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ಧ್ರುವ Locale& locale, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ಧ್ರುವ Locale& locale, 
                            ಧ್ರುವ UnicodeString* matchID, 
                            UErrorCode& status) ಧ್ರುವ 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    ಯದಿ (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(ಏತತ್); // cast away semantic const
        Mutex mutex(&lock);

        ಯದಿ (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        ಯದಿ (dnCache == nullptr) {
            ಧ್ರುವ Hashtable* m = getVisibleIDMap(status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ result;
            }
            ncthis->dnCache = ನೂತನ DNCache(locale); 
            ಯದಿ (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ result;
            }

            int32_t pos = UHASH_FIRST;
            ಧ್ರುವ UHashElement* entry = nullptr;
            ಯಾವತ್ ((entry = m->nextElement(pos)) != nullptr) {
                ಧ್ರುವ UnicodeString* id = static_cast<ಧ್ರುವ UnicodeString*>(entry->ಕೀಲಿ.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ಯದಿ (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ಅನ್ಯಥಾ {
                    dnCache->cache.put(dname, (ಶೂನ್ಯ*)id, status); // share pointer with visibleIDMap
                    ಯದಿ (U_SUCCESS(status)) {
                        ಅನುವರ್ತ;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ಪ್ರತ್ಯಾಗಮ result;
            }
        }
    }

    ICUService_ಕೀಲಿ* match_ಕೀಲಿ = create_ಕೀಲಿ(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ಧ್ರುವ UHashElement *entry = nullptr;
    ಯಾವತ್ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ಧ್ರುವ UnicodeString* id = static_cast<ಧ್ರುವ UnicodeString*>(entry->value.pointer);
        ಯದಿ (match_ಕೀಲಿ != nullptr && !match_ಕೀಲಿ->isFallbackOf(*id)) {
            ಅನುವರ್ತ;
        }
        ಧ್ರುವ UnicodeString* dn = static_cast<ಧ್ರುವ UnicodeString*>(entry->ಕೀಲಿ.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        ಯದಿ (U_FAILURE(status)) {
            result.removeAllElements();
            ಭಙ್ಗ;
        }
    }
    delete match_ಕೀಲಿ;

    ಪ್ರತ್ಯಾಗಮ result;
}

URegistry_ಕೀಲಿ
ICUService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& id, UErrorCode& status) 
{
    ಪ್ರತ್ಯಾಗಮ registerInstance(objToAdopt, id, ಸತ್ಯ, status);
}

URegistry_ಕೀಲಿ
ICUService::registerInstance(UObject* objToAdopt, ಧ್ರುವ UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_ಕೀಲಿ* ಕೀಲಿ = create_ಕೀಲಿ(&id, status);
    ಯದಿ (ಕೀಲಿ != nullptr) {
        UnicodeString canonicalID;
        ಕೀಲಿ->canonicalID(canonicalID);
        delete ಕೀಲಿ;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        ಯದಿ (f != nullptr) {
            ಪ್ರತ್ಯಾಗಮ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ಧ್ರುವ UnicodeString& id, UBool visible, UErrorCode& status)
{
    ಯದಿ (U_SUCCESS(status)) {
        ಯದಿ ((objToAdopt != nullptr) && (!id.isBogus())) {
            ಪ್ರತ್ಯಾಗಮ ನೂತನ SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

URegistry_ಕೀಲಿ
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    ಯದಿ (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    {
        Mutex mutex(&lock);

        ಯದಿ (factories == nullptr) {
            LocalPointer<UVector> lpFactories(ನೂತನ UVector(uprv_deleteUObject, nullptr, status), status);
            ಯದಿ (U_FAILURE(status)) {
                ಪ್ರತ್ಯಾಗಮ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->ಸೇರಿಸು_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        ಯದಿ (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ಯದಿ (U_SUCCESS(status)) {
        notifyChanged();
        ಪ್ರತ್ಯಾಗಮ (URegistry_ಕೀಲಿ)factoryToAdopt;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_ಕೀಲಿ rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = ಅಸತ್ಯ;
    ಯದಿ (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        ಯದಿ (factories->removeElement(factory)) {
            clearCaches();
            result = ಸತ್ಯ;
        } ಅನ್ಯಥಾ {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    ಯದಿ (result) {
        notifyChanged();
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

ಶೂನ್ಯ 
ICUService::reInitializeFactories() 
{
    ಯದಿ (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_Factories() == 0;
}

ICUService_ಕೀಲಿ* 
ICUService::create_ಕೀಲಿ(ಧ್ರುವ UnicodeString* id, UErrorCode& status) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ (U_FAILURE(status) || id == nullptr) ? nullptr : ನೂತನ ICUService_ಕೀಲಿ(*id);
}

ಶೂನ್ಯ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    delete dnCache;
    dnCache = nullptr;
    delete idCache;
    idCache = nullptr;
    delete serviceCache; serviceCache = nullptr;
}

ಶೂನ್ಯ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ಧ್ರುವ EventListener& l) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ dynamic_cast<ಧ್ರುವ ServiceListener*>(&l) != nullptr;
}

ಶೂನ್ಯ 
ICUService::notifyListener(EventListener& l) ಧ್ರುವ 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*ಏತತ್);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ result.append(name);
}

int32_t 
ICUService::ಎಣಿಕೆ_Factories() ಧ್ರುವ 
{
    ಪ್ರತ್ಯಾಗಮ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ಧ್ರುವ
{
    ಪ್ರತ್ಯಾಗಮ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
