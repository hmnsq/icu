// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#གལ་སྲིད !UCONFIG_NO_SERVICE

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
    གལ་སྲིད (U_SUCCESS(status)) {
        གལ་སྲིད (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ལོག;
        }

        གལ་སྲིད (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            གལ་སྲིད (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                གལ་སྲིད (U_FAILURE(status)) {
                    ལོག;
                }
                listeners = lpListeners.orphan();
            } དེ་མིན {
                དོན་ལུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    གལ་སྲིད (l == el) {
                        ལོག;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        དེ་མིན {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    གལ་སྲིད (U_SUCCESS(status)) {
        གལ་སྲིད (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ལོག;
        }

        {
            Mutex lmx(&notifyLock);
            གལ་སྲིད (listeners != nullptr) {
                // identity equality check
                དོན་ལུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    གལ་སྲིད (l == el) {
                        listeners->removeElementAt(i);
                        གལ་སྲིད (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ལོག;
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
    གལ་སྲིད (listeners != nullptr) {
        དོན་ལུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

