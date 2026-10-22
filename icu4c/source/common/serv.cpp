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

#تعریف_لغوه SERVICE_REFCOUNT

// in case we use the refcount stuff

U_NAMESPACE_BEGIN

/*
******************************************************************
*/

ثابت char16_t ICUService_کلی::مختاړی_DELIMITER = 0x002F;   /* '/' */

ICUService_کلی::ICUService_کلی(ثابت UnicodeString& id) 
: _id(id) {
}

ICUService_کلی::~ICUService_کلی() 
{
}

ثابت UnicodeString& 
ICUService_کلی::getID() ثابت 
{
    أرجع _id;
}

UnicodeString& 
ICUService_کلی::canonicalID(UnicodeString& پایله) ثابت 
{
    أرجع پایله.append(_id);
}

UnicodeString& 
ICUService_کلی::currentID(UnicodeString& پایله) ثابت 
{
    أرجع canonicalID(پایله);
}

UnicodeString& 
ICUService_کلی::currentDescriptor(UnicodeString& پایله) ثابت 
{
    مختاړی(پایله);
    پایله.append(مختاړی_DELIMITER);
    أرجع currentID(پایله);
}

UBool 
ICUService_کلی::fallback() 
{
    أرجع خطأ;
}

UBool 
ICUService_کلی::is_Fallback_د(ثابت UnicodeString& id) ثابت 
{
    أرجع id == _id;
}

UnicodeString& 
ICUService_کلی::مختاړی(UnicodeString& پایله) ثابت 
{
    أرجع پایله;
}

UnicodeString& 
ICUService_کلی::parse_مختاړی(UnicodeString& پایله) 
{
    int32_t n = پایله.شاخص_د(مختاړی_DELIMITER);
    إذا (n < 0) {
        n = 0;
    }
    پایله.remove(n);
    أرجع پایله;
}

UnicodeString& 
ICUService_کلی::parseSuffix(UnicodeString& پایله) 
{
    int32_t n = پایله.شاخص_د(مختاړی_DELIMITER);
    إذا (n >= 0) {
        پایله.remove(0, n+1);
    }
    أرجع پایله;
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString& 
ICUService_کلی::debug(UnicodeString& پایله) ثابت 
{
    debugClass(پایله);
    پایله.append((UnicodeString)" id: ");
    پایله.append(_id);
    أرجع پایله;
}

UnicodeString& 
ICUService_کلی::debugClass(UnicodeString& پایله) ثابت 
{
    أرجع پایله.append((UnicodeString)"ICUServiceKey");
}
#د_که_پای

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(ICUService_کلی)

/*
******************************************************************
*/

ICUServiceFactory::~ICUServiceFactory() {}

SimpleFactory::SimpleFactory(UObject* instance_منزل_Adopt, ثابت UnicodeString& id, UBool visible) 
: _instance(instance_منزل_Adopt), _id(id), _visible(visible)
{
}

SimpleFactory::~SimpleFactory() 
{
    ړنګول _instance;
}

UObject* 
SimpleFactory::create(ثابت ICUService_کلی& کلی, ثابت ICUService* service, UError_کوډ& حالت) ثابت 
{
    إذا (U_SUCCESS(حالت)) {
        UnicodeString لنډمهاله;
        إذا (_id == کلی.currentID(لنډمهاله)) {
            أرجع service->cloneInstance(_instance); 
        }
    }
    أرجع تش_اشاره;
}

فراغ 
SimpleFactory::updateVisibleIDs(Hashtable& پایله, UError_کوډ& حالت) ثابت 
{
    إذا (_visible) {
        پایله.put(_id, (فراغ*)هذا, حالت); // cast away const
    } وإلا {
        پایله.remove(_id);
    }
}

UnicodeString& 
SimpleFactory::getDisplayName(ثابت UnicodeString& id, ثابت Locale& /* locale */, UnicodeString& پایله) ثابت 
{
    إذا (_visible && _id == id) {
        پایله = _id;
    } وإلا {
        پایله.سېټ_منزل_Bogus();
    }
    أرجع پایله;
}

#که_تعریف_شوی SERVICE_DEBUG
UnicodeString& 
SimpleFactory::debug(UnicodeString& منزل_Append_منزل) ثابت 
{
    debugClass(منزل_Append_منزل);
    منزل_Append_منزل.append((UnicodeString)" id: ");
    منزل_Append_منزل.append(_id);
    منزل_Append_منزل.append((UnicodeString)", visible: ");
    منزل_Append_منزل.append(_visible ? (UnicodeString)"T" : (UnicodeString)"F");
    أرجع منزل_Append_منزل;
}

UnicodeString& 
SimpleFactory::debugClass(UnicodeString& منزل_Append_منزل) ثابت 
{
    أرجع منزل_Append_منزل.append((UnicodeString)"SimpleFactory");
}
#د_که_پای

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
صنف Cache_ثبت : عام UMemory {
خاص:
    int32_t refcount;

عام:
    UnicodeString actualDescriptor;
    UObject* service;

    /**
    * Releases a reference to the shared resource.
    */
    ~Cache_ثبت() {
        ړنګول service;
    }

    Cache_ثبت(ثابت UnicodeString& _actualDescriptor, UObject* _service) 
        : refcount(1), actualDescriptor(_actualDescriptor), service(_service) {
    }

    /**
    * Instantiation creates an initial reference, so don't call this
    * unless you're creating a new pointer to this.  Management of
    * that pointer will have to know how to deal with refcounts.  
    * Return true if the resource has not already been released.
    */
    Cache_ثبت* ref() {
        ++refcount;
        أرجع هذا;
    }

    /**
    * Destructions removes a reference, so don't call this unless
    * you're removing pointer to this somewhere.  Management of that
    * pointer will have to know how to deal with refcounts.  Once
    * the refcount drops to zero, the resource is released.  Return
    * false if the resource has been released.
    */
    Cache_ثبت* unref() {
        إذا ((--refcount) == 0) {
            ړنګول هذا;
            أرجع تش_اشاره;
        }
        أرجع هذا;
    }

    /**
    * Return true if there is at least one reference to this and the
    * resource has not been released.
    */
    UBool isShared() ثابت {
        أرجع refcount > 1;
    }
};

// Deleter for serviceCache
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
cacheDeleter(فراغ* obj) {
    U_NAMESPACE_USE ((Cache_ثبت*)obj)->unref();
}

U_CDECL_پای_ټکی

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
                   UError_کوډ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        StringPair* sp = جديد StringPair(displayName, id);
        إذا (sp == تش_اشاره || sp->isBogus()) {
            حالت = U_حافظه_ویش_ERROR;
            ړنګول sp;
            أرجع تش_اشاره;
        }
        أرجع sp;
    }
    أرجع تش_اشاره;
}

UBool 
StringPair::isBogus() ثابت {
    أرجع displayName.isBogus() || id.isBogus();
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
    U_NAMESPACE_USE ړنګول (StringPair*) obj;
}
U_CDECL_پای_ټکی

/*
******************************************************************
*/

ساكن UMutex قلف;

ICUService::ICUService()
: name()
, timestamp(0)
, factories(تش_اشاره)
, serviceCache(تش_اشاره)
, idCache(تش_اشاره)
, dnCache(تش_اشاره)
{
}

ICUService::ICUService(ثابت UnicodeString& newName) 
: name(newName)
, timestamp(0)
, factories(تش_اشاره)
, serviceCache(تش_اشاره)
, idCache(تش_اشاره)
, dnCache(تش_اشاره)
{
}

ICUService::~ICUService()
{
    {
        Mutex mutex(&قلف);
        clearCaches();
        ړنګول factories;
        factories = تش_اشاره;
    }
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UError_کوډ& حالت) ثابت 
{
    أرجع get(descriptor, تش_اشاره, حالت);
}

UObject* 
ICUService::get(ثابت UnicodeString& descriptor, UnicodeString* actualReturn, UError_کوډ& حالت) ثابت 
{
    UObject* پایله = تش_اشاره;
    ICUService_کلی* کلی = create_کلی(&descriptor, حالت);
    إذا (کلی) {
        پایله = get_کلی(*کلی, actualReturn, حالت);
        ړنګول کلی;
    }
    أرجع پایله;
}

UObject* 
ICUService::get_کلی(ICUService_کلی& کلی, UError_کوډ& حالت) ثابت 
{
    أرجع get_کلی(کلی, تش_اشاره, حالت);
}

// this is a vector that subclasses of ICUService can override to further customize the result object
// before returning it.  All other public get functions should call this one.

UObject* 
ICUService::get_کلی(ICUService_کلی& کلی, UnicodeString* actualReturn, UError_کوډ& حالت) ثابت 
{
    أرجع get_کلی(کلی, actualReturn, تش_اشاره, حالت);
}

// make it possible to call reentrantly on systems that don't have reentrant mutexes.
// we can use this simple approach since we know the situation where we're calling
// reentrantly even without knowing the thread.
صنف XMutex : عام UMemory {
عام:
    مضمن XMutex(UMutex *mutex, UBool reentering) 
        : fMutex(mutex)
        , f_فعال(!reentering) 
    {
        إذا (f_فعال) umtx_قلف(fMutex);
    }
    مضمن ~XMutex() {
        إذا (f_فعال) umtx_unlock(fMutex);
    }

خاص:
    UMutex  *fMutex;
    UBool f_فعال;
};

// called only by factories, treat as private
UObject* 
ICUService::get_کلی(ICUService_کلی& کلی, UnicodeString* actualReturn, ثابت ICUServiceFactory* factory, UError_کوډ& حالت) ثابت 
{
    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    إذا (isDefault()) {
        أرجع handleDefault(کلی, actualReturn, حالت);
    }

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const

    Cache_ثبت* پایله = تش_اشاره;
    {
        // The factory list can't be modified until we're done, 
        // otherwise we might update the cache with an invalid result.
        // The cache has to stay in synch with the factory list.
        // ICU doesn't have monitors so we can't use rw locks, so 
        // we single-thread everything using this service, for now.

        // if factory is not null, we're calling from within the mutex,
        // and since some unix machines don't have reentrant mutexes we
        // need to make sure not to try to lock it again.
        XMutex mutex(&قلف, factory != تش_اشاره);

        إذا (serviceCache == تش_اشاره) {
            ncthis->serviceCache = جديد Hashtable(حالت);
            إذا (ncthis->serviceCache == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع تش_اشاره;
            }
            إذا (U_FAILURE(حالت)) {
                ړنګول serviceCache;
                أرجع تش_اشاره;
            }
            serviceCache->سېټ_ارزښت_Deleter(cacheDeleter);
        }

        UnicodeString currentDescriptor;
        Local_اشاره_ګر<UVector> cacheDescriptorList;
        UBool put_دننه_Cache = خطأ;

        int32_t پیل_شاخص = 0;
        int32_t حد = factories->size();
        UBool cache_پایله = ریښتیا;

        إذا (factory != تش_اشاره) {
            لكل (int32_t i = 0; i < حد; ++i) {
                إذا (factory == static_cast<ثابت ICUServiceFactory*>(factories->elementAt(i))) {
                    پیل_شاخص = i + 1;
                    اكسر;
                }
            }
            إذا (پیل_شاخص == 0) {
                // throw new InternalError("Factory " + factory + "not registered with service: " + this);
                حالت = U_ILLEGAL_دلیل_ERROR;
                أرجع تش_اشاره;
            }
            cache_پایله = خطأ;
        }

        افعل {
            currentDescriptor.remove();
            کلی.currentDescriptor(currentDescriptor);
            پایله = static_cast<Cache_ثبت*>(serviceCache->get(currentDescriptor));
            إذا (پایله != تش_اشاره) {
                اكسر;
            }

            // first test of cache failed, so we'll have to update
            // the cache if we eventually succeed-- that is, if we're 
            // going to update the cache at all.
            put_دننه_Cache = ریښتیا;

            int32_t شاخص = پیل_شاخص;
            طالما (شاخص < حد) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(شاخص++));
                Local_اشاره_ګر<UObject> service(f->create(کلی, هذا, حالت));
                إذا (U_FAILURE(حالت)) {
                    أرجع تش_اشاره;
                }
                إذا (service.isValid()) {
                    پایله = جديد Cache_ثبت(currentDescriptor, service.getAlias());
                    إذا (پایله == تش_اشاره) {
                        حالت = U_حافظه_ویش_ERROR;
                        أرجع تش_اشاره;
                    }
                    service.orphan(); // result now owns service.

                    اذهب outer_پای_ټکی;
                }
            }

            // prepare to load the cache with all additional ids that 
            // will resolve to result, assuming we'll succeed.  We
            // don't want to keep querying on an id that's going to
            // fallback to the one that succeeded, we want to hit the
            // cache the first time next goaround.
            إذا (cacheDescriptorList.isNull()) {
                cacheDescriptorList.adopt_Instead_And_Check_Error_کوډ(جديد UVector(uprv_deleteUObject, تش_اشاره, 5, حالت), حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع تش_اشاره;
                }
            }

            Local_اشاره_ګر<UnicodeString> id_منزل_Cache(جديد UnicodeString(currentDescriptor), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع تش_اشاره;
            }
            إذا (id_منزل_Cache->isBogus()) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع تش_اشاره;
            }
            cacheDescriptorList->adoptElement(id_منزل_Cache.orphan(), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع تش_اشاره;
            }
        } طالما (کلی.fallback());
outer_پای_ټکی:

        إذا (پایله != تش_اشاره) {
            إذا (put_دننه_Cache && cache_پایله) {
                serviceCache->put(پایله->actualDescriptor, پایله, حالت);
                إذا (U_FAILURE(حالت)) {
                    أرجع تش_اشاره;
                }

                إذا (cacheDescriptorList.isValid()) {
                    لكل (int32_t i = cacheDescriptorList->size(); --i >= 0;) {
                        UnicodeString* desc = static_cast<UnicodeString*>(cacheDescriptorList->elementAt(i));

                        serviceCache->put(*desc, پایله, حالت);
                        إذا (U_FAILURE(حالت)) {
                            أرجع تش_اشاره;
                        }

                        پایله->ref();
                        cacheDescriptorList->removeElementAt(i);
                    }
                }
            }

            إذا (actualReturn != تش_اشاره) {
                // strip null prefix
                إذا (پایله->actualDescriptor.شاخص_د(static_cast<char16_t>(0x2f)) == 0) { // U+002f=slash (/)
                    actualReturn->remove();
                    actualReturn->append(پایله->actualDescriptor, 
                        1, 
                        پایله->actualDescriptor.length() - 1);
                } وإلا {
                    *actualReturn = پایله->actualDescriptor;
                }

                إذا (actualReturn->isBogus()) {
                    حالت = U_حافظه_ویش_ERROR;
                    ړنګول پایله;
                    أرجع تش_اشاره;
                }
            }

            UObject* service = cloneInstance(پایله->service);
            إذا (put_دننه_Cache && !cache_پایله) {
                ړنګول پایله;
            }
            أرجع service;
        }
    }

    أرجع handleDefault(کلی, actualReturn, حالت);
}

UObject* 
ICUService::handleDefault(ثابت ICUService_کلی& /*کلی*/, UnicodeString* /* actualIDReturn */, UError_کوډ& /* status */) ثابت 
{
    أرجع تش_اشاره;
}

UVector& 
ICUService::getVisibleIDs(UVector& پایله, UError_کوډ& حالت) ثابت {
    أرجع getVisibleIDs(پایله, تش_اشاره, حالت);
}

UVector& 
ICUService::getVisibleIDs(UVector& پایله, ثابت UnicodeString* سمون_ID, UError_کوډ& حالت) ثابت 
{
    پایله.removeAllElements();

    إذا (U_FAILURE(حالت)) {
        أرجع پایله;
    }
    UObjectDeleter *savedDeleter = پایله.سېټ_Deleter(uprv_deleteUObject);

    {
        Mutex mutex(&قلف);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != تش_اشاره) {
            ICUService_کلی* fallback_کلی = create_کلی(سمون_ID, حالت);

            لكل (int32_t pos = UHASH_لومړی; U_SUCCESS(حالت); ) {
                ثابت UHashElement* e = map->بل_Element(pos);
                إذا (e == تش_اشاره) {
                    اكسر;
                }

                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(e->کلی.اشاره_ګر);
                إذا (fallback_کلی != تش_اشاره) {
                    إذا (!fallback_کلی->is_Fallback_د(*id)) {
                        استمر;
                    }
                }

                Local_اشاره_ګر<UnicodeString> idClone(id->clone(), حالت);
                پایله.adoptElement(idClone.orphan(), حالت);
            }
            ړنګول fallback_کلی;
        }
    }
    إذا (U_FAILURE(حالت)) {
        پایله.removeAllElements();
    }
    پایله.سېټ_Deleter(savedDeleter);
    أرجع پایله;
}

ثابت Hashtable* 
ICUService::getVisibleIDMap(UError_کوډ& حالت) ثابت {
    إذا (U_FAILURE(حالت)) أرجع تش_اشاره;

    // must only be called when lock is already held

    ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
    إذا (idCache == تش_اشاره) {
        ncthis->idCache = جديد Hashtable(حالت);
        إذا (idCache == تش_اشاره) {
            حالت = U_حافظه_ویش_ERROR;
        } وإلا إذا (factories != تش_اشاره) {
            لكل (int32_t pos = factories->size(); --pos >= 0;) {
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(factories->elementAt(pos));
                f->updateVisibleIDs(*idCache, حالت);
            }
            إذا (U_FAILURE(حالت)) {
                ړنګول idCache;
                ncthis->idCache = تش_اشاره;
            }
        }
    }

    أرجع idCache;
}


UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& پایله) ثابت 
{
    أرجع getDisplayName(id, پایله, Locale::getDefault());
}

UnicodeString& 
ICUService::getDisplayName(ثابت UnicodeString& id, UnicodeString& پایله, ثابت Locale& locale) ثابت 
{
    {
        UError_کوډ حالت = U_ZERO_ERROR;
        Mutex mutex(&قلف);
        ثابت Hashtable* map = getVisibleIDMap(حالت);
        إذا (map != تش_اشاره) {
            ICUServiceFactory* f = static_cast<ICUServiceFactory*>(map->get(id));
            إذا (f != تش_اشاره) {
                f->getDisplayName(id, locale, پایله);
                أرجع پایله;
            }

            // fallback
            حالت = U_ZERO_ERROR;
            ICUService_کلی* fallback_کلی = create_کلی(&id, حالت);
            طالما (fallback_کلی != تش_اشاره && fallback_کلی->fallback()) {
                UnicodeString us;
                fallback_کلی->currentID(us);
                f = static_cast<ICUServiceFactory*>(map->get(us));
                إذا (f != تش_اشاره) {
                    f->getDisplayName(id, locale, پایله);
                    ړنګول fallback_کلی;
                    أرجع پایله;
                }
            }
            ړنګول fallback_کلی;
        }
    }
    پایله.سېټ_منزل_Bogus();
    أرجع پایله;
}

UVector& 
ICUService::get_Display_نومونه(UVector& پایله, UError_کوډ& حالت) ثابت 
{
    أرجع get_Display_نومونه(پایله, Locale::getDefault(), تش_اشاره, حالت);
}


UVector& 
ICUService::get_Display_نومونه(UVector& پایله, ثابت Locale& locale, UError_کوډ& حالت) ثابت 
{
    أرجع get_Display_نومونه(پایله, locale, تش_اشاره, حالت);
}

UVector& 
ICUService::get_Display_نومونه(UVector& پایله, 
                            ثابت Locale& locale, 
                            ثابت UnicodeString* سمون_ID, 
                            UError_کوډ& حالت) ثابت 
{
    پایله.removeAllElements();
    پایله.سېټ_Deleter(userv_deleteStringPair);
    إذا (U_SUCCESS(حالت)) {
        ICUService* ncthis = const_cast<ICUService*>(هذا); // cast away semantic const
        Mutex mutex(&قلف);

        إذا (dnCache != تش_اشاره && dnCache->locale != locale) {
            ړنګول dnCache;
            ncthis->dnCache = تش_اشاره;
        }

        إذا (dnCache == تش_اشاره) {
            ثابت Hashtable* m = getVisibleIDMap(حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع پایله;
            }
            ncthis->dnCache = جديد DNCache(locale); 
            إذا (dnCache == تش_اشاره) {
                حالت = U_حافظه_ویش_ERROR;
                أرجع پایله;
            }

            int32_t pos = UHASH_لومړی;
            ثابت UHashElement* ثبت = تش_اشاره;
            طالما ((ثبت = m->بل_Element(pos)) != تش_اشاره) {
                ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(ثبت->کلی.اشاره_ګر);
                ICUServiceFactory* f = static_cast<ICUServiceFactory*>(ثبت->value.اشاره_ګر);
                UnicodeString dname;
                f->getDisplayName(*id, locale, dname);
                إذا (dname.isBogus()) {
                    حالت = U_حافظه_ویش_ERROR;
                } وإلا {
                    dnCache->cache.put(dname, (فراغ*)id, حالت); // share pointer with visibleIDMap
                    إذا (U_SUCCESS(حالت)) {
                        استمر;
                    }
                }
                ړنګول dnCache;
                ncthis->dnCache = تش_اشاره;
                أرجع پایله;
            }
        }
    }

    ICUService_کلی* سمون_کلی = create_کلی(سمون_ID, حالت);
    /* To ensure that all elements in the hashtable are iterated, set pos to -1.
     * nextElement(pos) will skip the position at pos and begin the iteration
     * at the next position, which in this case will be 0.
     */
    int32_t pos = UHASH_لومړی; 
    ثابت UHashElement *ثبت = تش_اشاره;
    طالما ((ثبت = dnCache->cache.بل_Element(pos)) != تش_اشاره) {
        ثابت UnicodeString* id = static_cast<ثابت UnicodeString*>(ثبت->value.اشاره_ګر);
        إذا (سمون_کلی != تش_اشاره && !سمون_کلی->is_Fallback_د(*id)) {
            استمر;
        }
        ثابت UnicodeString* dn = static_cast<ثابت UnicodeString*>(ثبت->کلی.اشاره_ګر);
        StringPair* sp = StringPair::create(*id, *dn, حالت);
        پایله.adoptElement(sp, حالت);
        إذا (U_FAILURE(حالت)) {
            پایله.removeAllElements();
            اكسر;
        }
    }
    ړنګول سمون_کلی;

    أرجع پایله;
}

URegistry_کلی
ICUService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UError_کوډ& حالت) 
{
    أرجع ثبتوونکی_Instance(obj_منزل_Adopt, id, ریښتیا, حالت);
}

URegistry_کلی
ICUService::ثبتوونکی_Instance(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_کوډ& حالت) 
{
    ICUService_کلی* کلی = create_کلی(&id, حالت);
    إذا (کلی != تش_اشاره) {
        UnicodeString canonicalID;
        کلی->canonicalID(canonicalID);
        ړنګول کلی;

        ICUServiceFactory* f = createSimpleFactory(obj_منزل_Adopt, canonicalID, visible, حالت);
        إذا (f != تش_اشاره) {
            أرجع ثبتوونکی_Factory(f, حالت);
        }
    }
    ړنګول obj_منزل_Adopt;
    أرجع تش_اشاره;
}

ICUServiceFactory* 
ICUService::createSimpleFactory(UObject* obj_منزل_Adopt, ثابت UnicodeString& id, UBool visible, UError_کوډ& حالت)
{
    إذا (U_SUCCESS(حالت)) {
        إذا ((obj_منزل_Adopt != تش_اشاره) && (!id.isBogus())) {
            أرجع جديد SimpleFactory(obj_منزل_Adopt, id, visible);
        }
        حالت = U_ILLEGAL_دلیل_ERROR;
    }
    أرجع تش_اشاره;
}

URegistry_کلی
ICUService::ثبتوونکی_Factory(ICUServiceFactory* factory_منزل_Adopt, UError_کوډ& حالت)
{
    Local_اشاره_ګر<ICUServiceFactory>lp_Factory_منزل_Adopt(factory_منزل_Adopt);
    إذا (U_FAILURE(حالت) || factory_منزل_Adopt == تش_اشاره) {
        أرجع تش_اشاره;
    }
    {
        Mutex mutex(&قلف);

        إذا (factories == تش_اشاره) {
            Local_اشاره_ګر<UVector> lpFactories(جديد UVector(uprv_deleteUObject, تش_اشاره, حالت), حالت);
            إذا (U_FAILURE(حالت)) {
                أرجع تش_اشاره;
            }
            factories = lpFactories.orphan();
        }
        factories->ورننباسه_Element_At(lp_Factory_منزل_Adopt.orphan(), 0, حالت);
        إذا (U_SUCCESS(حالت)) {
            clearCaches();
        }
    }   // Close of mutex lock block.

    إذا (U_SUCCESS(حالت)) {
        notify_بدل_شوی();
        أرجع (URegistry_کلی)factory_منزل_Adopt;
    } وإلا {
        أرجع تش_اشاره;
    }
}

UBool 
ICUService::unregister(URegistry_کلی rkey, UError_کوډ& حالت) 
{
    ICUServiceFactory *factory = (ICUServiceFactory*)rkey;
    UBool پایله = خطأ;
    إذا (factory != تش_اشاره && factories != تش_اشاره) {
        Mutex mutex(&قلف);

        إذا (factories->removeElement(factory)) {
            clearCaches();
            پایله = ریښتیا;
        } وإلا {
            حالت = U_ILLEGAL_دلیل_ERROR;
            ړنګول factory;
        }
    }
    إذا (پایله) {
        notify_بدل_شوی();
    }
    أرجع پایله;
}

فراغ 
ICUService::reset() 
{
    {
        Mutex mutex(&قلف);
        reInitializeFactories();
        clearCaches();
    }
    notify_بدل_شوی();
}

فراغ 
ICUService::reInitializeFactories() 
{
    إذا (factories != تش_اشاره) {
        factories->removeAllElements();
    }
}

UBool 
ICUService::isDefault() ثابت 
{
    أرجع شمېر_Factories() == 0;
}

ICUService_کلی* 
ICUService::create_کلی(ثابت UnicodeString* id, UError_کوډ& حالت) ثابت 
{
    أرجع (U_FAILURE(حالت) || id == تش_اشاره) ? تش_اشاره : جديد ICUService_کلی(*id);
}

فراغ 
ICUService::clearCaches() 
{
    // callers synchronize before use
    ++timestamp;
    ړنګول dnCache;
    dnCache = تش_اشاره;
    ړنګول idCache;
    idCache = تش_اشاره;
    ړنګول serviceCache; serviceCache = تش_اشاره;
}

فراغ 
ICUService::clearServiceCache() 
{
    // callers synchronize before use
    ړنګول serviceCache; serviceCache = تش_اشاره;
}

UBool 
ICUService::مني_Listener(ثابت EventListener& l) ثابت 
{
    أرجع dynamic_cast<ثابت ServiceListener*>(&l) != تش_اشاره;
}

فراغ 
ICUService::notifyListener(EventListener& l) ثابت 
{
    (static_cast<ServiceListener&>(l)).service_بدل_شوی(*هذا);
}

UnicodeString&
ICUService::getName(UnicodeString& پایله) ثابت 
{
    أرجع پایله.append(name);
}

int32_t 
ICUService::شمېر_Factories() ثابت 
{
    أرجع factories == تش_اشاره ? 0 : factories->size();
}

int32_t
ICUService::getTimestamp() ثابت
{
    أرجع timestamp;
}

U_NAMESPACE_پای_ټکی

/* UCONFIG_NO_SERVICE */
#د_که_پای
