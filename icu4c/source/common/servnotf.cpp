// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#만약 !UCONFIG_NO_SERVICE

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
    만약 (U_SUCCESS(status)) {
        만약 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            반환;
        }

        만약 (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            만약 (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                만약 (U_FAILURE(status)) {
                    반환;
                }
                listeners = lpListeners.orphan();
            } 아니면 {
                위해 (정수 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    만약 (l == el) {
                        반환;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        아니면 {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    만약 (U_SUCCESS(status)) {
        만약 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            반환;
        }

        {
            Mutex lmx(&notifyLock);
            만약 (listeners != nullptr) {
                // identity equality check
                위해 (정수 i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    만약 (l == el) {
                        listeners->removeElementAt(i);
                        만약 (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        반환;
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
    만약 (listeners != nullptr) {
        위해 (정수 i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

