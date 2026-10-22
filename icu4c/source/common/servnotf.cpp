// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ᰀᰧ !UCONFIG_NO_SERVICE

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
    ᰀᰧ (U_SUCCESS(status)) {
        ᰀᰧ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᰀᰧᰌ;
        }

        ᰀᰧ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ᰀᰧ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ᰀᰧ (U_FAILURE(status)) {
                    ᰀᰧᰌ;
                }
                listeners = lpListeners.orphan();
            } ᰀᰧᰀ {
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᰀᰧ (l == el) {
                        ᰀᰧᰌ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ᰀᰧᰀ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ᰀᰧ (U_SUCCESS(status)) {
        ᰀᰧ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᰀᰧᰌ;
        }

        {
            Mutex lmx(&notifyLock);
            ᰀᰧ (listeners != nullptr) {
                // identity equality check
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᰀᰧ (l == el) {
                        listeners->removeElementAt(i);
                        ᰀᰧ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ᰀᰧᰌ;
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
    ᰀᰧ (listeners != nullptr) {
        for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

