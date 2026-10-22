// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
*******************************************************************************
* Copyright (C) 2001-2014, International Business Machines Corporation.
* All Rights Reserved.
*******************************************************************************
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/localpointer.h"

#إذا !UCONFIG_NO_SERVICE

#ضمن "serv.h"
#ضمن "umutex.h"

#undef SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_مفتاح::PREFIX_DELIMITER = 0x002F;   /* '/' */

ICUService_مفتاح::ICUService_مفتاح(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_مفتاح::~ICUService_مفتاح() 
{
}

ثابت UnicodeString& 
ICUService_مفتاح::getID() ثابت 
{
    إرجاع _id;
}

UnicodeString& 
ICUService_مفتاح::canonicalID(UnicodeString& result) ثابت 
{
    إرجاع result.append(_id);
}

UnicodeString& 
ICUService_مفتاح::currentID(UnicodeString& result) ثابت 
{
    إرجاع canonicalID(result);
}

UnicodeString& 
ICUService_مفتاح::currentDescriptor(UnicodeString& result) ثابت 
{
    prefix(result);
    result.append(PREFIX_DELIMITER);
    إرجاع currentID(result);
}

UBool 
ICUService_مفتاح::fallback() 
{
    إرجاع خطأ;
}

UBool 
ICUService_مفتاح::isFallbackOf(ثابت UnicodeString& id) ثابت 
{
    إرجاع id == _id;
}

UnicodeString& 
ICUService_مفتاح::prefix(UnicodeString& result) ثابت 
{
    إرجاع result;
}

UnicodeString& 
ICUService_مفتاح::parsePrefix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    إذا (n < 0) {
        n = 0;
    }
    result.remove(n);
    إرجاع result;
}

UnicodeString& 
ICUService_مفتاح::parseSuffix(UnicodeString& result) 
{
    int32_t n = result.indexOf(PREFIX_DELIMITER);
    إذا (n >= 0) {
        result.remove(0, n+1);
    }
    إرجاع result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
ICUService_مفتاح::debug(UnicodeString& result) ثابت 
{
    debugClass(result);
    result.append((UnicodeString)" id: ");
    result.append(_id);
    إرجاع result;
}

UnicodeString& 
ICUService_مفتاح::debugClass(UnicodeString& result) ثابت 
{
    إرجاع result.append((UnicodeString)"ICUServiceKey");
}
#endif

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_مفتاح)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instanceToAdopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instanceToAdopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    delete _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_مفتاح& مفتاح, ثابت ICUService* service, UErrorCode& status) ثابت 
{
    إذا (U_SUCCESS(status)) {
        UnicodeString temp;
        إذا (_id == مفتاح.currentID(temp)) {
            إرجاع service->cloneInstance(_instance); 
        }
    }
    إرجاع nullptr;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& result, UErrorCode& status) ثابت 
{
    إذا (_visible) {
        result.put(_id, (فراغ*)هذا, status); // cast away const
    } وإلا {
        result.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& result) ثابت 
{
    إذا (_visible && _id == id) {
        result = _id;
    } وإلا {
        result.setToBogus();
    }
    إرجاع result;
}

#ifdef SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& toAppendTo) ثابت 
{
    debugClass(toAppendTo);
    toAppendTo.append((UnicodeString)" id: ");
    toAppendTo.append(_id);
    toAppendTo.append((UnicodeString)", visible: ");
    toAppendTo.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    إرجاع toAppendTo;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& toAppendTo) ثابت 
{
    إرجاع toAppendTo.append((UnicodeString)"SimpleFactory");
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
صنف CacheEntry : عام UMemory {
خاص:
    int32_t refcount;

عام:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~CacheEntry() {
        delete service;
    }

    CacheEntry(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
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
        إرجاع هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    CacheEntry* unref() {
        إذا ((--refcount) == 0) {
            delete هذا;
            إرجاع nullptr;
        }
        إرجاع هذا;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        إرجاع refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
cacheDeleter(فراغ* obj) {
    U_NAMESPACE_USE ((CacheEntry*)obj)->unref();
}

U_CDECL_END

/*
******************************************************************
*/

صنف DNCache : عام UMemory {
عام:
    Hashtable cache;
    ثابت Locale locale;

    DNCache(ثابت Locale& _locale) 
        : cache(), locale(_locale) 
    {
        // cache.setKeyDeleter(uprv_deleteUObject);
    }
};


/*
******************************************************************
*/

StringPair* 
StringPair::create(ثابت UnicodeString& displayName, 
                   ثابت UnicodeString& id,
                   UErrorCode& status)
{
    إذا (U_SUCCESS(status)) {
        StringPair* sp = جديد StringPair(displayName, id);
        إذا (sp == nullptr || sp->isBogus()) {
            status = U_MEMORY_ALLOCATION_ERROR;
            delete sp;
            إرجاع nullptr;
        }
        إرجاع sp;
    }
    إرجاع nullptr;
}

UBool 
StringPair::isBogus() ثابت {
    إرجاع displayName.isBogus() || id.isBogus();
}

StringPair::StringPair(ثابت UnicodeString& _displayName, 
                       ثابت UnicodeString& _id)
: displayName(_displayName)
, id(_id)
{
}

U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
userv_deleteStringPair(فراغ *obj) {
    U_NAMESPACE_USE delete (StringPair*) obj;
}
U_CDECL_END

/*
******************************************************************
*/

ساكن UMutex lock;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(nullptr)
, serviceCache(nullptr)
, idCache(nullptr)
, dnCache(nullptr)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
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
ICUService::get(ثابت UnicodeString& descriptor, UErrorCode& status) ثابت 
{
    إرجاع get(descriptor, nullptr, status);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    UObject* result = nullptr;
    ICUService_مفتاح* مفتاح = create_مفتاح(&descriptor, status);
    إذا (مفتاح) {
        result = get_مفتاح(*مفتاح, actualReturn, status);
        delete مفتاح;
    }
    إرجاع result;
}

UObject* 
ICUService::get_مفتاح(ICUService_مفتاح& مفتاح, UErrorCode& status) ثابت 
{
    إرجاع get_مفتاح(مفتاح, nullptr, status);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_مفتاح(ICUService_مفتاح& مفتاح, UnicodeString* actualReturn, UErrorCode& status) ثابت 
{
    إرجاع get_مفتاح(مفتاح, actualReturn, nullptr, status);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , fActive(!reentering) 
    {
        إذا (fActive) umtx_lock(fMutex);
    }
    مضمن ~XMutex() {
        إذا (fActive) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool fActive;
};

// called only by factories, treat as private
UObject* 
ICUService::get_مفتاح(ICUService_مفتاح& مفتاح, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UErrorCode& status) ثابت 
{
    إذا (U_FAILURE(status)) {
        إرجاع nullptr;
    }

    إذا (isDefault()) {
        إرجاع handleDefault(مفتاح, actualReturn, status);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

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

        إذا (serviceCache == nullptr) {
            ncthis->serviceCache = جديد Hashtable(status);
            إذا (ncthis->serviceCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                إرجاع nullptr;
            }
            إذا (U_FAILURE(status)) {
                delete serviceCache;
                إرجاع nullptr;
            }
            serviceCache->set_قيمة_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        LocalPointer<UVector> cacheDescriptorList;
        UBool putInCache = خطأ;

        int32_t startIndex = 0;
        int32_t limit = factories->size();
        UBool cacheResult = true;

        إذا (factory != nullptr) {
            لكل (int32_t i = 0; i < limit; ++i) {
                إذا (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    startIndex = i + 1;
                    اكسر;
                }
            }
            إذا (startIndex == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                status = U_ILLEGAL_ARGUMENT_ERROR;
                إرجاع nullptr;
            }
            cacheResult = خطأ;
        }

        افعل {
            currentDescriptor.remove();
            مفتاح.currentDescriptor(currentDescriptor);
            result = static_cast<CacheEntry*>(serviceCache->get(currentDescriptor));
            إذا (result != nullptr) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            putInCache = true;

            int32_t index = startIndex;
            بينما (index < limit) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(index++));
                LocalPointer<UObject> service(f->create(مفتاح, هذا, status));
                إذا (U_FAILURE(status)) {
                    إرجاع nullptr;
                }
                إذا (service.isValid()) {
                    result = جديد CacheEntry(currentDescriptor, service.getAlias());
                    إذا (result == nullptr) {
                        status = U_MEMORY_ALLOCATION_ERROR;
                        إرجاع nullptr;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outerEnd;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            إذا (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adoptInsteadAndCheckErrorCode(جديد UVector(uprv_deleteUObject, nullptr, 5, status), status);
                إذا (U_FAILURE(status)) {
                    إرجاع nullptr;
                }
            }

            LocalPointer<UnicodeString> idToCache(جديد UnicodeString(currentDescriptor), status);
            إذا (U_FAILURE(status)) {
                إرجاع nullptr;
            }
            إذا (idToCache->isBogus()) {
                status = U_MEMORY_ALLOCATION_ERROR;
                إرجاع nullptr;
            }
            cacheDescriptorList->adoptElement(idToCache.orphan(), status);
            إذا (U_FAILURE(status)) {
                إرجاع nullptr;
            }
        } بينما (مفتاح.fallback());
outerEnd:

        إذا (result != nullptr) {
            إذا (putInCache && cacheResult) {
                serviceCache->put(result->actualDescriptor, result, status);
                إذا (U_FAILURE(status)) {
                    إرجاع nullptr;
                }

                إذا (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, result, status);
                        إذا (U_FAILURE(status)) {
                            إرجاع nullptr;
                        }

                        result->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            إذا (actualReturn != nullptr) {
                // strip null prefix
                إذا (result->actualDescriptor.indexOf(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(result->actualDescriptor, 
                        1, 
                        result->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = result->actualDescriptor;
                }

                إذا (actualReturn->isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                    delete result;
                    إرجاع nullptr;
                }
            }

            UObject* service = cloneInstance(result->service);
            إذا (putInCache && !cacheResult) {
                delete result;
            }
            إرجاع service;
        }
    }

    إرجاع handleDefault(مفتاح, actualReturn, status);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_مفتاح& /*مفتاح*/, UnicodeString* /* actualIDReturn */, UErrorCode& /* status */) ثابت 
{
    إرجاع nullptr;
}

UVector& 
ICUService::getVisibleIDs(UVector& result, UErrorCode& status) ثابت {
    إرجاع getVisibleIDs(result, nullptr, status);
}

UVector& 
ICUService::getVisibleIDs(UVector& result, ثابت UnicodeString* matchID, UErrorCode& status) ثابت 
{
    result.removeAllElements();

    إذا (U_FAILURE(status)) {
        إرجاع result;
    }
    UObjectDeleter *savedDeleter = result.setDeleter(uprv_deleteUObject);

    {
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        إذا (map != nullptr) {
            ICUService_مفتاح* fallback_مفتاح = create_مفتاح(matchID, status);

            لكل (int32_t pos = UHASH_FIRST; U_SUCCESS(status); ) {
                ثابت UHashElement* e = map->nextElement(pos);
                إذا (e == nullptr) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->مفتاح.pointer);
                إذا (fallback_مفتاح != nullptr) {
                    إذا (!fallback_مفتاح->isFallbackOf(*id)) {
                        استمر;
                    }
                }

                LocalPointer<UnicodeString> idClone(id->clone(), status);
                result.adoptElement(idClone.orphan(), status);
            }
            delete fallback_مفتاح;
        }
    }
    إذا (U_FAILURE(status)) {
        result.removeAllElements();
    }
    result.setDeleter(savedDeleter);
    إرجاع result;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UErrorCode& status) ثابت {
    إذا (U_FAILURE(status)) إرجاع nullptr;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    إذا (idCache == nullptr) {
        ncthis->idCache = جديد Hashtable(status);
        إذا (idCache == nullptr) {
            status = U_MEMORY_ALLOCATION_ERROR;
        } وإلا إذا (factories != nullptr) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, status);
            }
            إذا (U_FAILURE(status)) {
                delete idCache;
                ncthis->idCache = nullptr;
            }
        }
    }

    إرجاع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result) ثابت 
{
    إرجاع getDisplayName(id, result, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& result, ثابت Locale& locale) ثابت 
{
    {
        UErrorCode status = U_ZERO_ERROR;
        Mutex mutex(&lock);
        ثابت Hashtable* map = getVisibleIDMap(status);
        إذا (map != nullptr) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            إذا (f != nullptr) {
                f->getDisplayName(id, locale, result);
                إرجاع result;
            }

            // fallback
            status = U_ZERO_ERROR;
            ICUService_مفتاح* fallback_مفتاح = create_مفتاح(&id, status);
            بينما (fallback_مفتاح != nullptr && fallback_مفتاح->fallback()) {
                UnicodeString us;
                fallback_مفتاح->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                إذا (f != nullptr) {
                    f->getDisplayName(id, locale, result);
                    delete fallback_مفتاح;
                    إرجاع result;
                }
            }
            delete fallback_مفتاح;
        }
    }
    result.setToBogus();
    إرجاع result;
}

UVector& 
ICUService::getDisplayNames(UVector& result, UErrorCode& status) ثابت 
{
    إرجاع getDisplayNames(result, Locale::getDefault(), nullptr, status);
}


UVector& 
ICUService::getDisplayNames(UVector& result, ثابت Locale& locale, UErrorCode& status) ثابت 
{
    إرجاع getDisplayNames(result, locale, nullptr, status);
}

UVector& 
ICUService::getDisplayNames(UVector& result, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* matchID, 
                            UErrorCode& status) ثابت 
{
    result.removeAllElements();
    result.setDeleter(userv_deleteStringPair);
    إذا (U_SUCCESS(status)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&lock);

        إذا (dnCache != nullptr && dnCache->locale != locale) {
            delete dnCache;
            ncthis->dnCache = nullptr;
        }

        إذا (dnCache == nullptr) {
            ثابت Hashtable* m = getVisibleIDMap(status);
            إذا (U_FAILURE(status)) {
                إرجاع result;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            إذا (dnCache == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                إرجاع result;
            }

            int32_t pos = UHASH_FIRST;
            ثابت UHashElement* entry = nullptr;
            بينما ((entry = m->nextElement(pos)) != nullptr) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->مفتاح.pointer);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(entry->value.pointer);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                إذا (dname.isBogus()) {
                    status = U_MEMORY_ALLOCATION_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, status); // share pointer with visibleIDMap
                    إذا (U_SUCCESS(status)) {
                        استمر;
                    }
                }
                delete dnCache;
                ncthis->dnCache = nullptr;
                إرجاع result;
            }
        }
    }

    ICUService_مفتاح* match_مفتاح = create_مفتاح(matchID, status);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_FIRST; 
    ثابت UHashElement *entry = nullptr;
    بينما ((entry = dnCache->cache.nextElement(pos)) != nullptr) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(entry->value.pointer);
        إذا (match_مفتاح != nullptr && !match_مفتاح->isFallbackOf(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(entry->مفتاح.pointer);
        StringPair* sp = StringPair::create(*id, *dn, status);
        result.adoptElement(sp, status);
        إذا (U_FAILURE(status)) {
            result.removeAllElements();
            اكسر;
        }
    }
    delete match_مفتاح;

    إرجاع result;
}

URegistry_مفتاح
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UErrorCode& status) 
{
    إرجاع registerInstance(objToAdopt, id, true, status);
}

URegistry_مفتاح
ICUService::registerInstance(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status) 
{
    ICUService_مفتاح* مفتاح = create_مفتاح(&id, status);
    إذا (مفتاح != nullptr) {
        UnicodeString canonicalID;
        مفتاح->canonicalID(canonicalID);
        delete مفتاح;

        ICUServiceFactory* f = createSimpleFactory(objToAdopt, canonicalID, visible, status);
        إذا (f != nullptr) {
            إرجاع registerFactory(f, status);
        }
    }
    delete objToAdopt;
    إرجاع nullptr;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* objToAdopt, ثابت UnicodeString& id, UBool visible, UErrorCode& status)
{
    إذا (U_SUCCESS(status)) {
        إذا ((objToAdopt != nullptr) && (!id.isBogus())) {
            إرجاع جديد SimpleFactory(objToAdopt, id, visible);
        }
        status = U_ILLEGAL_ARGUMENT_ERROR;
    }
    إرجاع nullptr;
}

URegistry_مفتاح
ICUService::registerFactory(ICUServiceFactory* factoryToAdopt, UErrorCode& status)
{
    LocalPointer<ICUServiceFactory>lpFactoryToAdopt(factoryToAdopt);
    إذا (U_FAILURE(status) || factoryToAdopt == nullptr) {
        إرجاع nullptr;
    }
    {
        Mutex mutex(&lock);

        إذا (factories == nullptr) {
            LocalPointer<UVector> lpFactories(جديد UVector(uprv_deleteUObject, nullptr, status), status);
            إذا (U_FAILURE(status)) {
                إرجاع nullptr;
            }
            factories = lpFactories.orphan();
        }
        factories->أدرج_Element_At(lpFactoryToAdopt.orphan(), 0, status);
        إذا (U_SUCCESS(status)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    إذا (U_SUCCESS(status)) {
        notifyChanged();
        إرجاع (URegistry_مفتاح)factoryToAdopt;
    } وإلا {
        إرجاع nullptr;
    }
}

UBool 
ICUService::unregister(URegistry_مفتاح rkey, UErrorCode& status) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool result = خطأ;
    إذا (factory != nullptr && factories != nullptr) {
        Mutex mutex(&lock);

        إذا (factories->removeElement(factory)) {
            clearCaches();
            result = true;
        } وإلا {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            delete factory;
        }
    }
    إذا (result) {
        notifyChanged();
    }
    إرجاع result;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&lock);
        reInitializeFactories();
        clearCaches();
    }
    notifyChanged();
}

فراغ 
ICUService::reInitializeFactories() 
{
    إذا (factories != nullptr) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    إرجاع تعداد_Factories() == 0;
}

ICUService_مفتاح* 
ICUService::create_مفتاح(ثابت UnicodeString* id, UErrorCode& status) ثابت 
{
    إرجاع (U_FAILURE(status) || id == nullptr) ? nullptr : جديد ICUService_مفتاح(*id);
}

فراغ 
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

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    delete serviceCache; serviceCache = nullptr;
}

UBool 
ICUService::acceptsListener(ثابت EventListener& l) ثابت 
{
    إرجاع dynamic_cast<ثابت ServiceListener*>(&l) != nullptr;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).serviceChanged(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& result) ثابت 
{
    إرجاع result.append(name);
}

int32_t 
ICUService::تعداد_Factories() ثابت 
{
    إرجاع factories == nullptr ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    إرجاع timestamp;
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif
