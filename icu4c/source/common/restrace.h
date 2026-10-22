// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#અન્તર્ભાવ "unicode/utypes.h"

#જો U_ENABLE_TRACING

સંરચના UResourceBundle;

U_NAMESPACE_BEGIN

class CharString;

/**
 * Instances of this class store information used to trace reads from resource
 * bundles when ICU is built with --enable-tracing.
 *
 * All arguments of type const UResourceBundle*, const char*, and
 * const ResourceTracer& are stored as pointers. The caller must retain
 * ownership for the lifetime of this ResourceTracer.
 *
 * Exported as U_COMMON_API for Windows because it is a value field
 * in other exported types.
 */
class U_COMMON_API ResourceTracer {
જાહેર:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_ચાવી(nullptr),
        fIndex(-1) {}

    ResourceTracer(અચલ UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_ચાવી(nullptr),
        fIndex(-1) {}

    ResourceTracer(અચલ UResourceBundle* resB, અચલ અક્ષર* ચાવી) :
        fResB(resB),
        fParent(nullptr),
        f_ચાવી(ચાવી),
        fIndex(-1) {}

    ResourceTracer(અચલ UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_ચાવી(nullptr),
        fIndex(index) {}

    ResourceTracer(અચલ ResourceTracer& parent, અચલ અક્ષર* ચાવી) :
        fResB(nullptr),
        fParent(&parent),
        f_ચાવી(ચાવી),
        fIndex(-1) {}

    ResourceTracer(અચલ ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_ચાવી(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    શૂન્ય trace(અચલ અક્ષર* type) અચલ;
    શૂન્ય traceOpen() અચલ;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    શૂન્ય maybeTrace(અચલ અક્ષર* type) અચલ {
        જો (fResB || fParent) {
            trace(type);
        }
    }

ખાનગી:
    અચલ UResourceBundle* fResB;
    અચલ ResourceTracer* fParent;
    અચલ અક્ષર* f_ચાવી;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) અચલ;

    CharString& getResPath(CharString& output, UErrorCode& status) અચલ;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
જાહેર:
    સ્થિર શૂન્ય traceOpen(અચલ અક્ષર* path, અચલ અક્ષર* type, અચલ અક્ષર* name);

ખાનગી:
    સ્થિર શૂન્ય traceOpenDataFile(અચલ અક્ષર* path, અચલ અક્ષર* type, અચલ અક્ષર* name);
    સ્થિર શૂન્ય traceOpenResFile(અચલ અક્ષર* path, અચલ અક્ષર* name);
};

U_NAMESPACE_END

#નહીંતર // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
જાહેર:
    ResourceTracer() {}

    ResourceTracer(અચલ શૂન્ય*) {}

    ResourceTracer(અચલ શૂન્ય*, અચલ અક્ષર*) {}

    ResourceTracer(અચલ શૂન્ય*, int32_t) {}

    ResourceTracer(અચલ ResourceTracer&, અચલ અક્ષર*) {}

    ResourceTracer(અચલ ResourceTracer&, int32_t) {}

    શૂન્ય trace(અચલ અક્ષર*) અચલ {}

    શૂન્ય traceOpen() અચલ {}

    શૂન્ય maybeTrace(અચલ અક્ષર*) અચલ {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
જાહેર:
    સ્થિર શૂન્ય traceOpen(અચલ અક્ષર*, અચલ અક્ષર*, અચલ અક્ષર*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
