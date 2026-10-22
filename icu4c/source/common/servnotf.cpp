// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_SERVICE

#include "servnotf.h"
#ifdef NOTIFIER_DEBUG
#include <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

static UMutex notifyLock;

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


void 
ICUNotifier::addListener(const EventListener* l, UErrorCode& status) 
{
    ប្រសិនបើ (U_SUCCESS(status)) {
        ប្រសិនបើ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ត្រឡប់;
        }

        ប្រសិនបើ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ប្រសិនបើ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ប្រសិនបើ (U_FAILURE(status)) {
                    ត្រឡប់;
                }
                listeners = lpListeners.orphan();
            } បើមិនដូច្នេះ {
                សម្រាប់ (ចំនួនគត់ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ប្រសិនបើ (l == el) {
                        ត្រឡប់;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        បើមិនដូច្នេះ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ប្រសិនបើ (U_SUCCESS(status)) {
        ប្រសិនបើ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ត្រឡប់;
        }

        {
            Mutex lmx(&notifyLock);
            ប្រសិនបើ (listeners != nullptr) {
                // identity equality check
                សម្រាប់ (ចំនួនគត់ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ប្រសិនបើ (l == el) {
                        listeners->removeElementAt(i);
                        ប្រសិនបើ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ត្រឡប់;
                    }
                }
            }
        }
    }
}

void 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    ប្រសិនបើ (listeners != nullptr) {
        សម្រាប់ (ចំនួនគត់ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

