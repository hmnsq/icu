// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#གལ་ཏེ !UCONFIG_NO_SERVICE

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
    གལ་ཏེ (U_SUCCESS(status)) {
        གལ་ཏེ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ཕྱིར;
        }

        གལ་ཏེ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            གལ་ཏེ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                གལ་ཏེ (U_FAILURE(status)) {
                    ཕྱིར;
                }
                listeners = lpListeners.orphan();
            } དེ་མིན {
                ཆེད་དུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    གལ་ཏེ (l == el) {
                        ཕྱིར;
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
    གལ་ཏེ (U_SUCCESS(status)) {
        གལ་ཏེ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ཕྱིར;
        }

        {
            Mutex lmx(&notifyLock);
            གལ་ཏེ (listeners != nullptr) {
                // identity equality check
                ཆེད་དུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    གལ་ཏེ (l == el) {
                        listeners->removeElementAt(i);
                        གལ་ཏེ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ཕྱིར;
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
    གལ་ཏེ (listeners != nullptr) {
        ཆེད་དུ (ཧྲིལ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

