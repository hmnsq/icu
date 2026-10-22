// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#ضمن "unicode/utypes.h"

#𐡄𐡍 !UCONFIG_NO_SERVICE

#ضمن "servnotf.h"
#ifdef NOTIFIER_DEBUG
#ضمن <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

ساكن UMutex notifyLock;

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


فراغ 
ICUNotifier::addListener(ثابت EventListener* l, UErrorCode& status) 
{
    𐡄𐡍 (U_SUCCESS(status)) {
        𐡄𐡍 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }

        𐡄𐡍 (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            𐡄𐡍 (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(جديد UVector(5, status), status);
                𐡄𐡍 (U_FAILURE(status)) {
                    أرجع;
                }
                listeners = lpListeners.orphan();
            } وإلا {
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    𐡄𐡍 (l == el) {
                        أرجع;
                    }
                }
            }

            listeners->addElement((فراغ*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        وإلا {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

فراغ 
ICUNotifier::removeListener(ثابت EventListener *l, UErrorCode& status) 
{
    𐡄𐡍 (U_SUCCESS(status)) {
        𐡄𐡍 (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع;
        }

        {
            Mutex lmx(&notifyLock);
            𐡄𐡍 (listeners != nullptr) {
                // identity equality check
                لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
                    ثابت EventListener* el = static_cast<ثابت EventListener*>(listeners->elementAt(i));
                    𐡄𐡍 (l == el) {
                        listeners->removeElementAt(i);
                        𐡄𐡍 (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        أرجع;
                    }
                }
            }
        }
    }
}

فراغ 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    𐡄𐡍 (listeners != nullptr) {
        لكل (صحيح i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

