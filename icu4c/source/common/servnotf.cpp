// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ꦪꦼꦤ꧀ !UCONFIG_NO_SERVICE

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
    ꦪꦼꦤ꧀ (U_SUCCESS(status)) {
        ꦪꦼꦤ꧀ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }

        ꦪꦼꦤ꧀ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ꦪꦼꦤ꧀ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ꦪꦼꦤ꧀ (U_FAILURE(status)) {
                    ꦮꦭꦶ;
                }
                listeners = lpListeners.orphan();
            } ꦭꦶꦪ {
                ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ꦪꦼꦤ꧀ (l == el) {
                        ꦮꦭꦶ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ꦭꦶꦪ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ꦪꦼꦤ꧀ (U_SUCCESS(status)) {
        ꦪꦼꦤ꧀ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ꦮꦭꦶ;
        }

        {
            Mutex lmx(&notifyLock);
            ꦪꦼꦤ꧀ (listeners != nullptr) {
                // identity equality check
                ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ꦪꦼꦤ꧀ (l == el) {
                        listeners->removeElementAt(i);
                        ꦪꦼꦤ꧀ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ꦮꦭꦶ;
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
    ꦪꦼꦤ꧀ (listeners != nullptr) {
        ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

