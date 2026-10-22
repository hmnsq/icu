// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ᨀᨘ !UCONFIG_NO_SERVICE

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
    ᨀᨘ (U_SUCCESS(status)) {
        ᨀᨘ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᨀᨘᨒ;
        }

        ᨀᨘ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ᨀᨘ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ᨀᨘ (U_FAILURE(status)) {
                    ᨀᨘᨒ;
                }
                listeners = lpListeners.orphan();
            } ᨀᨘᨁ {
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᨀᨘ (l == el) {
                        ᨀᨘᨒ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ᨀᨘᨁ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ᨀᨘ (U_SUCCESS(status)) {
        ᨀᨘ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᨀᨘᨒ;
        }

        {
            Mutex lmx(&notifyLock);
            ᨀᨘ (listeners != nullptr) {
                // identity equality check
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᨀᨘ (l == el) {
                        listeners->removeElementAt(i);
                        ᨀᨘ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ᨀᨘᨒ;
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
    ᨀᨘ (listeners != nullptr) {
        for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

