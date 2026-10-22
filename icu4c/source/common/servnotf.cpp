// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2001-2012, International Business Machines Corporation and    *
 * others. All Rights Reserved.                                                *
 *******************************************************************************
 */

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_SERVICE

#അന്തര്ഭാവ "servnotf.h"
#ifdef NOTIFIER_DEBUG
#അന്തര്ഭാവ <stdio.h>
#endif

U_NAMESPACE_BEGIN

EventListener::~EventListener() {}
UOBJECT_DEFINE_RTTI_IMPLEMENTATION(EventListener)

സ്ഥിതം UMutex notifyLock;

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


ശൂന്യം 
ICUNotifier::addListener(സ്ഥിരം EventListener* l, UErrorCode& status) 
{
    എങ്കിൽ (U_SUCCESS(status)) {
        എങ്കിൽ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }

        എങ്കിൽ (acceptsListener(*l)) {
            Mutex lmx(&notifyLock);
            എങ്കിൽ (listeners == nullptr) {
                LocalPointer<UVector> lpListeners(പുതിയ UVector(5, status), status);
                എങ്കിൽ (U_FAILURE(status)) {
                    തിരികെ;
                }
                listeners = lpListeners.orphan();
            } അല്ലെങ്കിൽ {
                വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, e = listeners->size(); i < e; ++i) {
                    സ്ഥിരം EventListener* el = static_cast<സ്ഥിരം EventListener*>(listeners->elementAt(i));
                    എങ്കിൽ (l == el) {
                        തിരികെ;
                    }
                }
            }

            listeners->addElement((ശൂന്യം*)l, status); // cast away const
        }
#ifdef NOTIFIER_DEBUG
        അല്ലെങ്കിൽ {
            fprintf(stderr, "Listener invalid for this notifier.");
            exit(1);
        }
#endif
    }
}

ശൂന്യം 
ICUNotifier::removeListener(സ്ഥിരം EventListener *l, UErrorCode& status) 
{
    എങ്കിൽ (U_SUCCESS(status)) {
        എങ്കിൽ (l == nullptr) {
            status = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ;
        }

        {
            Mutex lmx(&notifyLock);
            എങ്കിൽ (listeners != nullptr) {
                // identity equality check
                വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, e = listeners->size(); i < e; ++i) {
                    സ്ഥിരം EventListener* el = static_cast<സ്ഥിരം EventListener*>(listeners->elementAt(i));
                    എങ്കിൽ (l == el) {
                        listeners->removeElementAt(i);
                        എങ്കിൽ (listeners->size() == 0) {
                            delete listeners;
                            listeners = nullptr;
                        }
                        തിരികെ;
                    }
                }
            }
        }
    }
}

ശൂന്യം 
ICUNotifier::notifyChanged() 
{
    Mutex lmx(&notifyLock);
    എങ്കിൽ (listeners != nullptr) {
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, e = listeners->size(); i < e; ++i) {
            EventListener* el = static_cast<EventListener*>(listeners->elementAt(i));
            notifyListener(*el);
        }
    }
}

U_NAMESPACE_END

/* UCONFIG_NO_SERVICE */
#endif

