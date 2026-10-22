// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ᮚᮦᮔ᮪ !UCONFIG_NO_SERVICE

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
    ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
        ᮚᮦᮔ᮪ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }

        ᮚᮦᮔ᮪ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ᮚᮦᮔ᮪ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ᮚᮦᮔ᮪ (U_FAILURE(status)) {
                    ᮝᮜᮤ;
                }
                listeners = lpListeners.orphan();
            } ᮜᮤᮚ {
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᮚᮦᮔ᮪ (l == el) {
                        ᮝᮜᮤ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ᮜᮤᮚ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ᮚᮦᮔ᮪ (U_SUCCESS(status)) {
        ᮚᮦᮔ᮪ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᮝᮜᮤ;
        }

        {
            Mutex lmx(&notifyLock);
            ᮚᮦᮔ᮪ (listeners != nullptr) {
                // identity equality check
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᮚᮦᮔ᮪ (l == el) {
                        listeners->removeElementAt(i);
                        ᮚᮦᮔ᮪ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ᮝᮜᮤ;
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
    ᮚᮦᮔ᮪ (listeners != nullptr) {
        for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

