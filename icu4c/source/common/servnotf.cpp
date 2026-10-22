// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ᱡᱩᱫᱤ !UCONFIG_NO_SERVICE

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
    ᱡᱩᱫᱤ (U_SUCCESS(status)) {
        ᱡᱩᱫᱤ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᱨᱩᱣᱟᱹᱲ;
        }

        ᱡᱩᱫᱤ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ᱡᱩᱫᱤ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ᱡᱩᱫᱤ (U_FAILURE(status)) {
                    ᱨᱩᱣᱟᱹᱲ;
                }
                listeners = lpListeners.orphan();
            } ᱵᱟᱝ {
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᱡᱩᱫᱤ (l == el) {
                        ᱨᱩᱣᱟᱹᱲ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ᱵᱟᱝ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ᱡᱩᱫᱤ (U_SUCCESS(status)) {
        ᱡᱩᱫᱤ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ᱨᱩᱣᱟᱹᱲ;
        }

        {
            Mutex lmx(&notifyLock);
            ᱡᱩᱫᱤ (listeners != nullptr) {
                // identity equality check
                for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ᱡᱩᱫᱤ (l == el) {
                        listeners->removeElementAt(i);
                        ᱡᱩᱫᱤ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ᱨᱩᱣᱟᱹᱲ;
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
    ᱡᱩᱫᱤ (listeners != nullptr) {
        for (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

