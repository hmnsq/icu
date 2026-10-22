// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ถ้า !UCONFIG_NO_SERVICE

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
    ถ้า (U_SUCCESS(status)) {
        ถ้า (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            คืน;
        }

        ถ้า (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ถ้า (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ถ้า (U_FAILURE(status)) {
                    คืน;
                }
                listeners = lpListeners.orphan();
            } มิฉะนั้น {
                สำหรับ (จำนวนเต็ม i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ถ้า (l == el) {
                        คืน;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        มิฉะนั้น {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ถ้า (U_SUCCESS(status)) {
        ถ้า (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            คืน;
        }

        {
            Mutex lmx(&notifyLock);
            ถ้า (listeners != nullptr) {
                // identity equality check
                สำหรับ (จำนวนเต็ม i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ถ้า (l == el) {
                        listeners->removeElementAt(i);
                        ถ้า (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        คืน;
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
    ถ้า (listeners != nullptr) {
        สำหรับ (จำนวนเต็ม i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

