// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ifndef __RESTRACE_H__
#define __RESTRACE_H__

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ U_ENABLE_TRACING

ഘടന UResourceBundle;

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
പൊതു:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_താക്കോൽ(nullptr),
        fIndex(-1) {}

    ResourceTracer(സ്ഥിരം UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_താക്കോൽ(nullptr),
        fIndex(-1) {}

    ResourceTracer(സ്ഥിരം UResourceBundle* resB, സ്ഥിരം അക്ഷരം* താക്കോൽ) :
        fResB(resB),
        fParent(nullptr),
        f_താക്കോൽ(താക്കോൽ),
        fIndex(-1) {}

    ResourceTracer(സ്ഥിരം UResourceBundle* resB, int32_t index) :
        fResB(resB),
        fParent(nullptr),
        f_താക്കോൽ(nullptr),
        fIndex(index) {}

    ResourceTracer(സ്ഥിരം ResourceTracer& parent, സ്ഥിരം അക്ഷരം* താക്കോൽ) :
        fResB(nullptr),
        fParent(&parent),
        f_താക്കോൽ(താക്കോൽ),
        fIndex(-1) {}

    ResourceTracer(സ്ഥിരം ResourceTracer& parent, int32_t index) :
        fResB(nullptr),
        fParent(&parent),
        f_താക്കോൽ(nullptr),
        fIndex(index) {}

    ~ResourceTracer();

    ശൂന്യം trace(സ്ഥിരം അക്ഷരം* type) സ്ഥിരം;
    ശൂന്യം traceOpen() സ്ഥിരം;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    ശൂന്യം maybeTrace(സ്ഥിരം അക്ഷരം* type) സ്ഥിരം {
        എങ്കിൽ (fResB || fParent) {
            trace(type);
        }
    }

സ്വകാര്യ:
    സ്ഥിരം UResourceBundle* fResB;
    സ്ഥിരം ResourceTracer* fParent;
    സ്ഥിരം അക്ഷരം* f_താക്കോൽ;
    int32_t fIndex;

    CharString& getFilePath(CharString& output, UErrorCode& status) സ്ഥിരം;

    CharString& getResPath(CharString& output, UErrorCode& status) സ്ഥിരം;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
പൊതു:
    സ്ഥിതം ശൂന്യം traceOpen(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* type, സ്ഥിരം അക്ഷരം* name);

സ്വകാര്യ:
    സ്ഥിതം ശൂന്യം traceOpenDataFile(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* type, സ്ഥിരം അക്ഷരം* name);
    സ്ഥിതം ശൂന്യം traceOpenResFile(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* name);
};

U_NAMESPACE_END

#അല്ലെങ്കിൽ // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
പൊതു:
    ResourceTracer() {}

    ResourceTracer(സ്ഥിരം ശൂന്യം*) {}

    ResourceTracer(സ്ഥിരം ശൂന്യം*, സ്ഥിരം അക്ഷരം*) {}

    ResourceTracer(സ്ഥിരം ശൂന്യം*, int32_t) {}

    ResourceTracer(സ്ഥിരം ResourceTracer&, സ്ഥിരം അക്ഷരം*) {}

    ResourceTracer(സ്ഥിരം ResourceTracer&, int32_t) {}

    ശൂന്യം trace(സ്ഥിരം അക്ഷരം*) സ്ഥിരം {}

    ശൂന്യം traceOpen() സ്ഥിരം {}

    ശൂന്യം maybeTrace(സ്ഥിരം അക്ഷരം*) സ്ഥിരം {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
പൊതു:
    സ്ഥിതം ശൂന്യം traceOpen(സ്ഥിരം അക്ഷരം*, സ്ഥിരം അക്ഷരം*, സ്ഥിരം അക്ഷരം*) {}
};

U_NAMESPACE_END

#endif // U_ENABLE_TRACING

#endif //__RESTRACE_H__
