// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"

#ଯଦି !UCONFIG_NO_SERVICE

#ଅନ୍ତର୍ଭାଵ "serv.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ଅଚଳ char16_t ICUService_ଚାବି::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_ଚାବି::ICUService_ଚାବି(ଅଚଳ UnicodeString& id) 
: _id(id) {
}

ICUService_ଚାବି::~ICUService_ଚାବି() 
{
}

ଅଚଳ UnicodeString& 
ICUService_ଚାବି::getID() ଅଚଳ 
{
    ଫେରନ୍ତୁ _id;
}

UnicodeString& 
ICUService_ଚାବି::canonicalID(UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ result.append(_id);
}

UnicodeString& 
ICUService_ଚାବି::currentID(UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ canonicalID(result);
}

UnicodeString& 
ICUService_ଚାବି::currentDescriptor(UnicodeString& result) ଅଚଳ 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    ଫେରନ୍ତୁ currentID(result);
}

UBool 
ICUService_ଚାବି::fallback() 
{
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

UBool 
ICUService_ଚାବି::isFallbackOf(ଅଚଳ UnicodeString& id) ଅଚଳ 
{
    ଫେରନ୍ତୁ id == _id;
}

UnicodeString& 
ICUService_ଚାବି::prefix(UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ result;
}

UnicodeString& 
ICUService_ଚାବି::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ଯଦି (n < 0) {
        n = 0;
    }
    result.remove(n);
    ଫେରନ୍ତୁ result;
}

UnicodeString& 
ICUService_ଚାବି::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    ଯଦି (n >= 0) {
        result.remove(0, n+1);
    }
    ଫେରନ୍ତୁ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_ଚାବି::debug(UnicodeString& result) ଅଚଳ 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    ଫେରନ୍ତୁ result;
}

UnicodeString& 
ICUService_ଚାବି::debugClass(UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_ଚାବି)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ଅଚଳ UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ଅଚଳ ICUService_ଚାବି& ଚାବି, ଅଚଳ ICUService* service, UErrorCode& status) ଅଚଳ 
{
    ଯଦି (U_SUCCESS(status)) {
        UnicodeString temp;
        ଯଦି (_id == ଚାବି.currentID(temp)) {
            ଫେରନ୍ତୁ service->cloneInstance(_instance); 
        }
    }
    ଫେରନ୍ତୁ nullptr;
}

ଶୂନ୍ୟ 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ଅଚଳ 
{
    ଯଦି (_visible) {
        result.put(_id, (ଶୂନ୍ୟ*)ଏହି, status); // cast away const
    } ଅନ୍ୟଥା {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ଅଚଳ UnicodeString& id, ଅଚଳ Locale& /* locale */, UnicodeString& result) ଅଚଳ 
{
    ଯଦି (_visible && _id == id) {
        result = _id;
    } ଅନ୍ୟଥା {
        result.setToBogus();
    }
    ଫେରନ୍ତୁ result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ଅଚଳ 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    ଫେରନ୍ତୁ toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ଅଚଳ 
{
    ଫେରନ୍ତୁ toAppendTo.append((UnicodeString)"SimpleFactory");
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
ଶ୍ରେଣୀ CacheEntry : ସର୍ବସାଧାରଣ UMemory {
ବ୍ୟକ୍ତିଗତ:
    int32_t refcount;

ସର୍ବସାଧାରଣ:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ଅଚଳ UnicodeString& _actualDescriptor, UObject* _service) 
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
        ଫେରନ୍ତୁ ଏହି;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        ଯଦି ((--refcount) == 0) {
            delete ଏହି;
            ଫେରନ୍ତୁ nullptr;
        }
        ଫେରନ୍ତୁ ଏହି;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ଅଚଳ {
        ଫେରନ୍ତୁ refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
cacheDeleter(ଶୂନ୍ୟ* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

ଶ୍ରେଣୀ DNCache : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    Hashtable cache;
    ଅଚଳ Locale locale;

    DNCache(ଅଚଳ Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ଅଚଳ UnicodeString& displayName, 
                   ଅଚଳ UnicodeString& id,
                   UErrorCode& status)
{
    ଯଦି (U_SUCCESS(status)) {
        StringPair* sp = ନୂତନ StringPair(displayName, id);
        ଯଦି (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            ଫେରନ୍ତୁ nullptr;
        }
        ଫେରନ୍ତୁ sp;
    }
    ଫେରନ୍ତୁ nullptr;
}

UBool 
StringPair::isBogus() ଅଚଳ {
    ଫେରନ୍ତୁ displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ଅଚଳ UnicodeString& _displayName, 
                       ଅଚଳ UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
userv_deleteStringPair(ଶୂନ୍ୟ *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

ସ୍ଥିର UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ଅଚଳ UnicodeString& newName) 
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
ICUService::get(ଅଚଳ UnicodeString& descriptor, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ଅଚଳ UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ଅଚଳ 
{
    UObject* result = nullptr;
    ICUService_ଚାବି* ଚାବି = create_ଚାବି(&descriptor, status);
    ଯଦି (ଚାବି) {
        result = get_ଚାବି(*ଚାବି, actualReturn, status);
        delete ଚାବି;
    }
    ଫେରନ୍ତୁ result;
}

UObject* 
ICUService::get_ଚାବି(ICUService_ଚାବି& ଚାବି, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ get_ଚାବି(ଚାବି, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_ଚାବି(ICUService_ଚାବି& ଚାବି, UnicodeString* actualReturn, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ get_ଚାବି(ଚାବି, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
ଶ୍ରେଣୀ XMutex : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    ଧାଡ଼ିଭିତରେ XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        ଯଦି (fActive) umtx_lock(fMutex);
    }
    ଧାଡ଼ିଭିତରେ ~XMutex() {
        ଯଦି (fActive) umtx_unlock(fMutex);
    }

ବ୍ୟକ୍ତିଗତ:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_ଚାବି(ICUService_ଚାବି& ଚାବି, UnicodeString* actualReturn, ଅଚଳ ICUServiceFactory* factory, UErrorCode& status) ଅଚଳ 
{
    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି (isDefault()) {
        ଫେରନ୍ତୁ handleDefault(ଚାବି, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(ଏହି); // cast away semantic const

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

        ଯଦି (serviceCache == nullptr) {
            ncthis->serviceCache = ନୂତନ Hashtable(status);
            ଯଦି (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
            ଯଦି (U_FAILURE(status)) {
                delete serviceCache;
                ଫେରନ୍ତୁ nullptr;
            }
            serviceCache->set_ମୂଲ୍ୟ_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = ମିଥ୍ୟା;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = ସତ୍ୟ;

        ଯଦି (factory != nullptr) {
            ପାଇଁ (int32_t i = 0; i < limit; ++i) {
                ଯଦି (factory == static_cast<ଅଚଳ ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    ଭାଙ୍ଗ;
                }
            }
            ଯଦି (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
            cacheResult = ମିଥ୍ୟା;
        }

        କର {
            currentDescriptor.remove();
            ଚାବି.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            ଯଦି (result != nullptr) {
                ଭାଙ୍ଗ;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = ସତ୍ୟ;

            int32_t index = startIndex;
            ଯେତେବେଳେ (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(ଚାବି, ଏହି, status));
                ଯଦି (U_FAILURE(status)) {
                    ଫେରନ୍ତୁ nullptr;
                }
                ଯଦି (service.isValid()) {
                    result = ନୂତନ CacheEntry(currentDescriptor, service.getAlias());
                    ଯଦି (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        ଫେରନ୍ତୁ nullptr;
                    }
                    service.orphan(); // result now owns service.

                    ଯାଅ outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            ଯଦି (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(ନୂତନ UVector(uprv_deleteUObject, nullptr, 5, status), status);
                ଯଦି (U_FAILURE(status)) {
                    ଫେରନ୍ତୁ nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(ନୂତନ UnicodeString(currentDescriptor), status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ nullptr;
            }
            ଯଦି (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ nullptr;
            }
        } ଯେତେବେଳେ (ଚାବି.fallback());
outerEnd:

        ଯଦି (result != nullptr) {
            ଯଦି (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                ଯଦି (U_FAILURE(status)) {
                    ଫେରନ୍ତୁ nullptr;
                }

                ଯଦି (cacheDescriptorList.isValid()) {
                    ପାଇଁ (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        ଯଦି (U_FAILURE(status)) {
                            ଫେରନ୍ତୁ nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            ଯଦି (actualReturn != nullptr) {
                // strip null prefix
                ଯଦି (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } ଅନ୍ୟଥା {
                    *actualReturn = result->actualDescriptor;
                }

                ଯଦି (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    ଫେରନ୍ତୁ nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            ଯଦି (putInCache && !cacheResult) {
                delete result;
            }
            ଫେରନ୍ତୁ service;
        }
    }

    ଫେରନ୍ତୁ handleDefault(ଚାବି, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ଅଚଳ ICUService_ଚାବି& /*ଚାବି*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ଅଚଳ 
{
    ଫେରନ୍ତୁ nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ଅଚଳ {
    ଫେରନ୍ତୁ getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ଅଚଳ UnicodeString* matchID, UErrorCode& status) ଅଚଳ 
{
    result.removeAllElements();

    ଯଦି (U_FAILURE(status)) {
        ଫେରନ୍ତୁ result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ଅଚଳ Hashtable* map = getVisibleIDMap(status);
        ଯଦି (map != nullptr) {
            ICUService_ଚାବି* fallback_ଚାବି = create_ଚାବି(matchID, status);

            ପାଇଁ (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ଅଚଳ UHashElement* e = map->nextElement(pos);
                ଯଦି (e == nullptr) {
                    ଭାଙ୍ଗ;
                }

                ଅଚଳ UnicodeString* id = static_cast<ଅଚଳ UnicodeString*>(e->ଚାବି.pointer);
                ଯଦି (fallback_ଚାବି != nullptr) {
                    ଯଦି (!fallback_ଚାବି->isFallbackOf(*id)) {
                        ଜାରି;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_ଚାବି;
        }
    }
    ଯଦି (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    ଫେରନ୍ତୁ result;
}

ଅଚଳ Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ଅଚଳ {
    ଯଦି (U_FAILURE(status)) ଫେରନ୍ତୁ nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(ଏହି); // cast away semantic const
    ଯଦି (idCache == nullptr) {
        ncthis->idCache = ନୂତନ Hashtable(status);
        ଯଦି (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା ଯଦି (factories != nullptr) {
            ପାଇଁ (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            ଯଦି (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    ଫେରନ୍ତୁ idCache;
}


UnicodeString& 
ICUService::getDisplayName(ଅଚଳ UnicodeString& id, UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ଅଚଳ UnicodeString& id, UnicodeString& result, ଅଚଳ Locale& locale) ଅଚଳ 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ଅଚଳ Hashtable* map = getVisibleIDMap(status);
        ଯଦି (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            ଯଦି (f != nullptr) {
                f->getDisplayName(id, locale, result);
                ଫେରନ୍ତୁ result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_ଚାବି* fallback_ଚାବି = create_ଚାବି(&id, status);
            ଯେତେବେଳେ (fallback_ଚାବି != nullptr && fallback_ଚାବି->fallback()) {
                UnicodeString us;
                fallback_ଚାବି->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                ଯଦି (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_ଚାବି;
                    ଫେରନ୍ତୁ result;
                }
            }
            delete fallback_ଚାବି;
        }
    }
    result.setToBogus();
    ଫେରନ୍ତୁ result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ଅଚଳ Locale& locale, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ଅଚଳ Locale& locale, 
                            ଅଚଳ UnicodeString* matchID, 
                            UErrorCode& status) ଅଚଳ 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    ଯଦି (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(ଏହି); // cast away semantic const
        Mutex mutex(&lock);

        ଯଦି (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        ଯଦି (dnCache == nullptr) {
            ଅଚଳ Hashtable* m = getVisibleIDMap(status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ result;
            }
            ncthis->dnCache = ନୂତନ DNCache(locale); 
            ଯଦି (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ result;
            }

            int32_t pos = UHASH_FIRST;
            ଅଚଳ UHashElement* entry = nullptr;
            ଯେତେବେଳେ ((entry = m->nextElement(pos)) != nullptr) {
                ଅଚଳ UnicodeString* id = static_cast<ଅଚଳ UnicodeString*>(entry->ଚାବି.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                ଯଦି (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } ଅନ୍ୟଥା {
                    dnCache->cache.put(dname, (ଶୂନ୍ୟ*)id, status); // share pointer with visibleIDMap
                    ଯଦି (U_SUCCESS(status)) {
                        ଜାରି;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                ଫେରନ୍ତୁ result;
            }
        }
    }

    ICUService_ଚାବି* match_ଚାବି = create_ଚାବି(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ଅଚଳ UHashElement *entry = nullptr;
    ଯେତେବେଳେ ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ଅଚଳ UnicodeString* id = static_cast<ଅଚଳ UnicodeString*>(entry->value.pointer);
        ଯଦି (match_ଚାବି != nullptr && !match_ଚାବି->isFallbackOf(*id)) {
            ଜାରି;
        }
        ଅଚଳ UnicodeString* dn = static_cast<ଅଚଳ UnicodeString*>(entry->ଚାବି.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        ଯଦି (U_FAILURE(status)) {
            result.removeAllElements();
            ଭାଙ୍ଗ;
        }
    }
    delete match_ଚାବି;

    ଫେରନ୍ତୁ result;
}

URegistry_ଚାବି
ICUService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& id, UErrorCode& status) 
{
    ଫେରନ୍ତୁ registerInstance(objToAdopt, id, ସତ୍ୟ, status);
}

URegistry_ଚାବି
ICUService::registerInstance(UObject* objToAdopt, ଅଚଳ UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_ଚାବି* ଚାବି = create_ଚାବି(&id, status);
    ଯଦି (ଚାବି != nullptr) {
        UnicodeString canonicalID;
        ଚାବି->canonicalID(canonicalID);
        delete ଚାବି;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        ଯଦି (f != nullptr) {
            ଫେରନ୍ତୁ registerFactory(f, status);
        }
    }
    delete objToAdopt;
    ଫେରନ୍ତୁ nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ଅଚଳ UnicodeString& id, UBool visible, UErrorCode& status)
{
    ଯଦି (U_SUCCESS(status)) {
        ଯଦି ((objToAdopt != nullptr) && (!id.isBogus())) {
            ଫେରନ୍ତୁ ନୂତନ SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    ଫେରନ୍ତୁ nullptr;
}

URegistry_ଚାବି
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    ଯଦି (U_FAILURE(status) || factoryToAdopt == nullptr) {
        ଫେରନ୍ତୁ nullptr;
    }
    {
        Mutex mutex(&lock);

        ଯଦି (factories == nullptr) {
            LocalPointer<UVector> lpFactories(ନୂତନ UVector(uprv_deleteUObject, nullptr, status), status);
            ଯଦି (U_FAILURE(status)) {
                ଫେରନ୍ତୁ nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->ଭର୍ତ୍ତି_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        ଯଦି (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    ଯଦି (U_SUCCESS(status)) {
        notifyChanged();
        ଫେରନ୍ତୁ (URegistry_ଚାବି)factoryToAdopt;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_ଚାବି rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = ମିଥ୍ୟା;
    ଯଦି (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        ଯଦି (factories->removeElement(factory)) {
            clearCaches();
            result = ସତ୍ୟ;
        } ଅନ୍ୟଥା {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    ଯଦି (result) {
        notifyChanged();
    }
    ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

ଶୂନ୍ୟ 
ICUService::reInitializeFactories() 
{
    ଯଦି (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ଅଚଳ 
{
    ଫେରନ୍ତୁ ଗଣନା_Factories() == 0;
}

ICUService_ଚାବି* 
ICUService::create_ଚାବି(ଅଚଳ UnicodeString* id, UErrorCode& status) ଅଚଳ 
{
    ଫେରନ୍ତୁ (U_FAILURE(status) || id == nullptr) ? nullptr : ନୂତନ ICUService_ଚାବି(*id);
}

ଶୂନ୍ୟ 
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

ଶୂନ୍ୟ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ଅଚଳ EventListener& l) ଅଚଳ 
{
    ଫେରନ୍ତୁ dynamic_cast<ଅଚଳ ServiceListener*>(&l) != nullptr;
}

ଶୂନ୍ୟ 
ICUService::notifyListener(EventListener& l) ଅଚଳ 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*ଏହି);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ଅଚଳ 
{
    ଫେରନ୍ତୁ result.append(name);
}

int32_t 
ICUService::ଗଣନା_Factories() ଅଚଳ 
{
    ଫେରନ୍ତୁ factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ଅଚଳ
{
    ଫେରନ୍ତୁ timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
