// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#include "unicode/utypes.h"

#ຖ້າ !UCONFIG_NO_SERVICE

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
    ຖ້າ (U_SUCCESS(status)) {
        ຖ້າ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ;
        }

        ຖ້າ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            ຖ້າ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(new UVector(5, status), status);
                ຖ້າ (U_FAILURE(status)) {
                    ສົ່ງຄືນ;
                }
                listeners = lpListeners.orphan();
            } ບໍ່ດັ່ງນັ້ນ {
                ສຳລັບ (ຈຳນວນເຕັມ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ຖ້າ (l == el) {
                        ສົ່ງຄືນ;
                    }
                }
            }

            listeners->addElement((void*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        ບໍ່ດັ່ງນັ້ນ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

void 
ICUNotifier::removeListener(const EventListener *l, UErrorCode& status) 
{
    ຖ້າ (U_SUCCESS(status)) {
        ຖ້າ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ;
        }

        {
            Mutex lmx(&notifyLock);
            ຖ້າ (listeners != nullptr) {
                // identity equality check
                ສຳລັບ (ຈຳນວນເຕັມ i = 0, e = listeners->size(); i < e; ++i) {
                    const EventListener* el = static_cast<const EventListener*>(listeners->elementAt(i));
                    ຖ້າ (l == el) {
                        listeners->removeElementAt(i);
                        ຖ້າ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        ສົ່ງຄືນ;
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
    ຖ້າ (listeners != nullptr) {
        ສຳລັບ (ຈຳນວນເຕັມ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

