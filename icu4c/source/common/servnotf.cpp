// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_SERVICE

#شامل "servnotf.h"
#ifdef NOTIFIER_DEBUG
#شامل <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ایستا UMutex notifyLock;

ICUNotifier::ICUNotifier() 
: listeners(nullptr) 
{
}

ICUNotifier::~ICUNotifier() {
    {
        Mutex lmx(&notifyLock);
        delete listeners;
        listeners = nullptr;
    }
}


تهی 
ICUNotifier::addListener(ثابت EventListener* l, UErrorCode& status) 
{
    اگر (U_SUCCESS(status)) {
        اگر (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }

        اگر (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            اگر (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(جدید UVector(5, status), status);
                اگر (U_FAILURE(status)) {
                    بازگشت;
                }
                listeners = lpListeners.orphan();
            } وگرنه {
                برای (صحیح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    اگر (l == el) {
                        بازگشت;
                    }
                }
            }

            listeners->addElement((تهی*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        وگرنه {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

تهی 
ICUNotifier::removeListener(ثابت EventListener *l, UErrorCode& status) 
{
    اگر (U_SUCCESS(status)) {
        اگر (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت;
        }

        {
            Mutex lmx(&notifyLock);
            اگر (listeners != nullptr) {
                // identity equality check
                برای (صحیح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    اگر (l == el) {
                        listeners->removeElementAt(i);
                        اگر (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        بازگشت;
                    }
                }
            }
        }
    }
}

تهی 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    اگر (listeners != nullptr) {
        برای (صحیح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

