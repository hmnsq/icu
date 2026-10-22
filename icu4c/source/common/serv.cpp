// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/localpointer.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "serv.h"
#അന്തര്ഭാവ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

സ്ഥിരം char16_t ICUService_താക്കോൽ::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_താക്കോൽ::ICUService_താക്കോൽ(സ്ഥിരം UnicodeString& id) 
: _id(id) {
}

ICUService_താക്കോൽ::~ICUService_താക്കോൽ() 
{
}

സ്ഥിരം UnicodeString& 
ICUService_താക്കോൽ::getID() സ്ഥിരം 
{
    തിരികെ _id;
}

UnicodeString& 
ICUService_താക്കോൽ::canonicalID(UnicodeString& result) സ്ഥിരം 
{
    തിരികെ result.append(_id);
}

UnicodeString& 
ICUService_താക്കോൽ::currentID(UnicodeString& result) സ്ഥിരം 
{
    തിരികെ canonicalID(result);
}

UnicodeString& 
ICUService_താക്കോൽ::currentDescriptor(UnicodeString& result) സ്ഥിരം 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    തിരികെ currentID(result);
}

UBool 
ICUService_താക്കോൽ::fallback() 
{
    തിരികെ അസത്യം;
}

UBool 
ICUService_താക്കോൽ::isFallbackOf(സ്ഥിരം UnicodeString& id) സ്ഥിരം 
{
    തിരികെ id == _id;
}

UnicodeString& 
ICUService_താക്കോൽ::prefix(UnicodeString& result) സ്ഥിരം 
{
    തിരികെ result;
}

UnicodeString& 
ICUService_താക്കോൽ::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    എങ്കിൽ (n < 0) {
        n = 0;
    }
    result.remove(n);
    തിരികെ result;
}

UnicodeString& 
ICUService_താക്കോൽ::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    എങ്കിൽ (n >= 0) {
        result.remove(0, n+1);
    }
    തിരികെ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_താക്കോൽ::debug(UnicodeString& result) സ്ഥിരം 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    തിരികെ result;
}

UnicodeString& 
ICUService_താക്കോൽ::debugClass(UnicodeString& result) സ്ഥിരം 
{
    തിരികെ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_താക്കോൽ)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, സ്ഥിരം UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(സ്ഥിരം ICUService_താക്കോൽ& താക്കോൽ, സ്ഥിരം ICUService* service, UErrorCode& status) സ്ഥിരം 
{
    എങ്കിൽ (U_SUCCESS(status)) {
        UnicodeString temp;
        എങ്കിൽ (_id == താക്കോൽ.currentID(temp)) {
            തിരികെ service->cloneInstance(_instance); 
        }
    }
    തിരികെ nullptr;
}

ശൂന്യം 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) സ്ഥിരം 
{
    എങ്കിൽ (_visible) {
        result.put(_id, (ശൂന്യം*)ഇത്, status); // cast away const
    } അല്ലെങ്കിൽ {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(സ്ഥിരം UnicodeString& id, സ്ഥിരം Locale& /* locale */, UnicodeString& result) സ്ഥിരം 
{
    എങ്കിൽ (_visible && _id == id) {
        result = _id;
    } അല്ലെങ്കിൽ {
        result.setToBogus();
    }
    തിരികെ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) സ്ഥിരം 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    തിരികെ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) സ്ഥിരം 
{
    തിരികെ toAppendTo.append((UnicodeString)"SimpleFactory");
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
വർഗ്ഗം CacheEntry : പൊതു UMemory {
സ്വകാര്യ:
    int32_t refcount;

പൊതു:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(സ്ഥിരം UnicodeString& _actualDescriptor, UObject* _service) 
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
        തിരികെ ഇത്;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        എങ്കിൽ ((--refcount) == 0) {
            delete ഇത്;
            തിരികെ nullptr;
        }
        തിരികെ ഇത്;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() സ്ഥിരം {
        തിരികെ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV
cacheDeleter(ശൂന്യം* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

വർഗ്ഗം DNCache : പൊതു UMemory {
പൊതു:
    Hashtable cache;
    സ്ഥിരം Locale locale;

    DNCache(സ്ഥിരം Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(സ്ഥിരം UnicodeString& displayName, 
                   സ്ഥിരം UnicodeString& id,
                   UErrorCode& status)
{
    എങ്കിൽ (U_SUCCESS(status)) {
        StringPair* sp = പുതിയ StringPair(displayName, id);
        എങ്കിൽ (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            തിരികെ nullptr;
        }
        തിരികെ sp;
    }
    തിരികെ nullptr;
}

UBool 
StringPair::isBogus() സ്ഥിരം {
    തിരികെ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(സ്ഥിരം UnicodeString& _displayName, 
                       സ്ഥിരം UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം U_CALLCONV
userv_deleteStringPair(ശൂന്യം *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

സ്ഥിതം UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(സ്ഥിരം UnicodeString& newName) 
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
ICUService::get(സ്ഥിരം UnicodeString& descriptor, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(സ്ഥിരം UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) സ്ഥിരം 
{
    UObject* result = nullptr;
    ICUService_താക്കോൽ* താക്കോൽ = create_താക്കോൽ(&descriptor, status);
    എങ്കിൽ (താക്കോൽ) {
        result = get_താക്കോൽ(*താക്കോൽ, actualReturn, status);
        delete താക്കോൽ;
    }
    തിരികെ result;
}

UObject* 
ICUService::get_താക്കോൽ(ICUService_താക്കോൽ& താക്കോൽ, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ get_താക്കോൽ(താക്കോൽ, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_താക്കോൽ(ICUService_താക്കോൽ& താക്കോൽ, UnicodeString* actualReturn, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ get_താക്കോൽ(താക്കോൽ, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
വർഗ്ഗം XMutex : പൊതു UMemory {
പൊതു:
    വരിയിൽ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        എങ്കിൽ (fActive) umtx_lock(fMutex);
    }
    വരിയിൽ ~XMutex() {
        എങ്കിൽ (fActive) umtx_unlock(fMutex);
    }

സ്വകാര്യ:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_താക്കോൽ(ICUService_താക്കോൽ& താക്കോൽ, UnicodeString* actualReturn, സ്ഥിരം ICUServiceFactory* factory, UErrorCode& status) സ്ഥിരം 
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }

    എങ്കിൽ (isDefault()) {
        തിരികെ handleDefault(താക്കോൽ, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(ഇത്); // cast away semantic const

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

        എങ്കിൽ (serviceCache == nullptr) {
            ncthis->serviceCache = പുതിയ Hashtable(status);
            എങ്കിൽ (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ nullptr;
            }
            എങ്കിൽ (U_FAILURE(status)) {
                delete serviceCache;
                തിരികെ nullptr;
            }
            serviceCache->set_മൂല്യം_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = അസത്യം;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = സത്യം;

        എങ്കിൽ (factory != nullptr) {
            വേണ്ടി (int32_t i = 0; i < limit; ++i) {
                എങ്കിൽ (factory == static_cast<സ്ഥിരം ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    നിർത്തുക;
                }
            }
            എങ്കിൽ (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                തിരികെ nullptr;
            }
            cacheResult = അസത്യം;
        }

        ചെയ്യുക {
            currentDescriptor.remove();
            താക്കോൽ.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            എങ്കിൽ (result != nullptr) {
                നിർത്തുക;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = സത്യം;

            int32_t index = startIndex;
            ആകുമ്പോൾ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(താക്കോൽ, ഇത്, status));
                എങ്കിൽ (U_FAILURE(status)) {
                    തിരികെ nullptr;
                }
                എങ്കിൽ (service.isValid()) {
                    result = പുതിയ CacheEntry(currentDescriptor, service.getAlias());
                    എങ്കിൽ (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        തിരികെ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    പോകുക outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            എങ്കിൽ (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(പുതിയ UVector(uprv_deleteUObject, nullptr, 5, status), status);
                എങ്കിൽ (U_FAILURE(status)) {
                    തിരികെ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(പുതിയ UnicodeString(currentDescriptor), status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ nullptr;
            }
            എങ്കിൽ (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ nullptr;
            }
        } ആകുമ്പോൾ (താക്കോൽ.fallback());
outerEnd:

        എങ്കിൽ (result != nullptr) {
            എങ്കിൽ (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                എങ്കിൽ (U_FAILURE(status)) {
                    തിരികെ nullptr;
                }

                എങ്കിൽ (cacheDescriptorList.isValid()) {
                    വേണ്ടി (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        എങ്കിൽ (U_FAILURE(status)) {
                            തിരികെ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            എങ്കിൽ (actualReturn != nullptr) {
                // strip null prefix
                എങ്കിൽ (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } അല്ലെങ്കിൽ {
                    *actualReturn = result->actualDescriptor;
                }

                എങ്കിൽ (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    തിരികെ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            എങ്കിൽ (putInCache && !cacheResult) {
                delete result;
            }
            തിരികെ service;
        }
    }

    തിരികെ handleDefault(താക്കോൽ, actualReturn, status);
}

UObject* 
ICUService::handleDefault(സ്ഥിരം ICUService_താക്കോൽ& /*താക്കോൽ*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) സ്ഥിരം 
{
    തിരികെ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) സ്ഥിരം {
    തിരികെ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, സ്ഥിരം UnicodeString* matchID, UErrorCode& status) സ്ഥിരം 
{
    result.removeAllElements();

    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        സ്ഥിരം Hashtable* map = getVisibleIDMap(status);
        എങ്കിൽ (map != nullptr) {
            ICUService_താക്കോൽ* fallback_താക്കോൽ = create_താക്കോൽ(matchID, status);

            വേണ്ടി (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                സ്ഥിരം UHashElement* e = map->nextElement(pos);
                എങ്കിൽ (e == nullptr) {
                    നിർത്തുക;
                }

                സ്ഥിരം UnicodeString* id = static_cast<സ്ഥിരം UnicodeString*>(e->താക്കോൽ.pointer);
                എങ്കിൽ (fallback_താക്കോൽ != nullptr) {
                    എങ്കിൽ (!fallback_താക്കോൽ->isFallbackOf(*id)) {
                        തുടരുക;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_താക്കോൽ;
        }
    }
    എങ്കിൽ (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    തിരികെ result;
}

സ്ഥിരം Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) തിരികെ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ഇത്); // cast away semantic const
    എങ്കിൽ (idCache == nullptr) {
        ncthis->idCache = പുതിയ Hashtable(status);
        എങ്കിൽ (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ എങ്കിൽ (factories != nullptr) {
            വേണ്ടി (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            എങ്കിൽ (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    തിരികെ idCache;
}


UnicodeString& 
ICUService::getDisplayName(സ്ഥിരം UnicodeString& id, UnicodeString& result) സ്ഥിരം 
{
    തിരികെ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(സ്ഥിരം UnicodeString& id, UnicodeString& result, സ്ഥിരം Locale& locale) സ്ഥിരം 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        സ്ഥിരം Hashtable* map = getVisibleIDMap(status);
        എങ്കിൽ (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            എങ്കിൽ (f != nullptr) {
                f->getDisplayName(id, locale, result);
                തിരികെ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_താക്കോൽ* fallback_താക്കോൽ = create_താക്കോൽ(&id, status);
            ആകുമ്പോൾ (fallback_താക്കോൽ != nullptr && fallback_താക്കോൽ->fallback()) {
                UnicodeString us;
                fallback_താക്കോൽ->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                എങ്കിൽ (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_താക്കോൽ;
                    തിരികെ result;
                }
            }
            delete fallback_താക്കോൽ;
        }
    }
    result.setToBogus();
    തിരികെ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, സ്ഥിരം Locale& locale, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            സ്ഥിരം Locale& locale, 
                            സ്ഥിരം UnicodeString* matchID, 
                            UErrorCode& status) സ്ഥിരം 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    എങ്കിൽ (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(ഇത്); // cast away semantic const
        Mutex mutex(&lock);

        എങ്കിൽ (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        എങ്കിൽ (dnCache == nullptr) {
            സ്ഥിരം Hashtable* m = getVisibleIDMap(status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ result;
            }
            ncthis->dnCache = പുതിയ DNCache(locale); 
            എങ്കിൽ (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ result;
            }

            int32_t pos = UHASH_FIRST;
            സ്ഥിരം UHashElement* entry = nullptr;
            ആകുമ്പോൾ ((entry = m->nextElement(pos)) != nullptr) {
                സ്ഥിരം UnicodeString* id = static_cast<സ്ഥിരം UnicodeString*>(entry->താക്കോൽ.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                എങ്കിൽ (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } അല്ലെങ്കിൽ {
                    dnCache->cache.put(dname, (ശൂന്യം*)id, status); // share pointer with visibleIDMap
                    എങ്കിൽ (U_SUCCESS(status)) {
                        തുടരുക;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                തിരികെ result;
            }
        }
    }

    ICUService_താക്കോൽ* match_താക്കോൽ = create_താക്കോൽ(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    സ്ഥിരം UHashElement *entry = nullptr;
    ആകുമ്പോൾ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        സ്ഥിരം UnicodeString* id = static_cast<സ്ഥിരം UnicodeString*>(entry->value.pointer);
        എങ്കിൽ (match_താക്കോൽ != nullptr && !match_താക്കോൽ->isFallbackOf(*id)) {
            തുടരുക;
        }
        സ്ഥിരം UnicodeString* dn = static_cast<സ്ഥിരം UnicodeString*>(entry->താക്കോൽ.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        എങ്കിൽ (U_FAILURE(status)) {
            result.removeAllElements();
            നിർത്തുക;
        }
    }
    delete match_താക്കോൽ;

    തിരികെ result;
}

URegistry_താക്കോൽ
ICUService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& id, UErrorCode& status) 
{
    തിരികെ registerInstance(objToAdopt, id, സത്യം, status);
}

URegistry_താക്കോൽ
ICUService::registerInstance(UObject* objToAdopt, സ്ഥിരം UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_താക്കോൽ* താക്കോൽ = create_താക്കോൽ(&id, status);
    എങ്കിൽ (താക്കോൽ != nullptr) {
        UnicodeString canonicalID;
        താക്കോൽ->canonicalID(canonicalID);
        delete താക്കോൽ;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        എങ്കിൽ (f != nullptr) {
            തിരികെ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    തിരികെ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, സ്ഥിരം UnicodeString& id, UBool visible, UErrorCode& status)
{
    എങ്കിൽ (U_SUCCESS(status)) {
        എങ്കിൽ ((objToAdopt != nullptr) && (!id.isBogus())) {
            തിരികെ പുതിയ SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    തിരികെ nullptr;
}

URegistry_താക്കോൽ
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    എങ്കിൽ (U_FAILURE(status) || factoryToAdopt == nullptr) {
        തിരികെ nullptr;
    }
    {
        Mutex mutex(&lock);

        എങ്കിൽ (factories == nullptr) {
            LocalPointer<UVector> lpFactories(പുതിയ UVector(uprv_deleteUObject, nullptr, status), status);
            എങ്കിൽ (U_FAILURE(status)) {
                തിരികെ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->തിരുകുക_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        എങ്കിൽ (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    എങ്കിൽ (U_SUCCESS(status)) {
        notifyChanged();
        തിരികെ (URegistry_താക്കോൽ)factoryToAdopt;
    } അല്ലെങ്കിൽ {
        തിരികെ nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_താക്കോൽ rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = അസത്യം;
    എങ്കിൽ (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        എങ്കിൽ (factories->removeElement(factory)) {
            clearCaches();
            result = സത്യം;
        } അല്ലെങ്കിൽ {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    എങ്കിൽ (result) {
        notifyChanged();
    }
    തിരികെ result;
}

ശൂന്യം 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

ശൂന്യം 
ICUService::reInitializeFactories() 
{
    എങ്കിൽ (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() സ്ഥിരം 
{
    തിരികെ എണ്ണം_Factories() == 0;
}

ICUService_താക്കോൽ* 
ICUService::create_താക്കോൽ(സ്ഥിരം UnicodeString* id, UErrorCode& status) സ്ഥിരം 
{
    തിരികെ (U_FAILURE(status) || id == nullptr) ? nullptr : പുതിയ ICUService_താക്കോൽ(*id);
}

ശൂന്യം 
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

ശൂന്യം 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(സ്ഥിരം EventListener& l) സ്ഥിരം 
{
    തിരികെ dynamic_cast<സ്ഥിരം ServiceListener*>(&l) != nullptr;
}

ശൂന്യം 
ICUService::notifyListener(EventListener& l) സ്ഥിരം 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*ഇത്);
}

UnicodeString&
ICUService::getName(UnicodeString& result) സ്ഥിരം 
{
    തിരികെ result.append(name);
}

int32_t 
ICUService::എണ്ണം_Factories() സ്ഥിരം 
{
    തിരികെ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() സ്ഥിരം
{
    തിരികെ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
