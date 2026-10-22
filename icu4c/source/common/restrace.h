// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अगर_अघोषित __RESTRACE_H__
#घोषणा __RESTRACE_H__

#समावेश "unicode/utypes.h"

#अगर U_ENABLE_TRACING

काष्ठा UResourceBundle;

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
खुला:
    ResourceTracer() :
        fResB(nullptr),
        fParent(nullptr),
        f_कुंजी(nullptr),
        f_सूचकांक(-1) {}

    ResourceTracer(स्थिर UResourceBundle* resB) :
        fResB(resB),
        fParent(nullptr),
        f_कुंजी(nullptr),
        f_सूचकांक(-1) {}

    ResourceTracer(स्थिर UResourceBundle* resB, स्थिर अक्षर* कुंजी) :
        fResB(resB),
        fParent(nullptr),
        f_कुंजी(कुंजी),
        f_सूचकांक(-1) {}

    ResourceTracer(स्थिर UResourceBundle* resB, int32_t सूचकांक) :
        fResB(resB),
        fParent(nullptr),
        f_कुंजी(nullptr),
        f_सूचकांक(सूचकांक) {}

    ResourceTracer(स्थिर ResourceTracer& parent, स्थिर अक्षर* कुंजी) :
        fResB(nullptr),
        fParent(&parent),
        f_कुंजी(कुंजी),
        f_सूचकांक(-1) {}

    ResourceTracer(स्थिर ResourceTracer& parent, int32_t सूचकांक) :
        fResB(nullptr),
        fParent(&parent),
        f_कुंजी(nullptr),
        f_सूचकांक(सूचकांक) {}

    ~ResourceTracer();

    व्योम trace(स्थिर अक्षर* type) स्थिर;
    व्योम traceOpen() स्थिर;

    /**
     * Calls trace() if the resB or parent provided to the constructor was
     * non-null; otherwise, does nothing.
     */
    व्योम maybeTrace(स्थिर अक्षर* type) स्थिर {
        अगर (fResB || fParent) {
            trace(type);
        }
    }

निजी:
    स्थिर UResourceBundle* fResB;
    स्थिर ResourceTracer* fParent;
    स्थिर अक्षर* f_कुंजी;
    int32_t f_सूचकांक;

    CharString& getFilePath(CharString& output, UErrorCode& status) स्थिर;

    CharString& getResPath(CharString& output, UErrorCode& status) स्थिर;
};

/**
 * This class provides methods to trace data file reads when ICU is built
 * with --enable-tracing.
 */
class FileTracer {
खुला:
    अटल व्योम traceOpen(स्थिर अक्षर* path, स्थिर अक्षर* type, स्थिर अक्षर* name);

निजी:
    अटल व्योम trace_Open_तथ्य_File(स्थिर अक्षर* path, स्थिर अक्षर* type, स्थिर अक्षर* name);
    अटल व्योम traceOpenResFile(स्थिर अक्षर* path, स्थिर अक्षर* name);
};

U_NAMESPACE_END

#अन्यथा // U_ENABLE_TRACING

U_NAMESPACE_BEGIN

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class U_COMMON_API ResourceTracer {
खुला:
    ResourceTracer() {}

    ResourceTracer(स्थिर व्योम*) {}

    ResourceTracer(स्थिर व्योम*, स्थिर अक्षर*) {}

    ResourceTracer(स्थिर व्योम*, int32_t) {}

    ResourceTracer(स्थिर ResourceTracer&, स्थिर अक्षर*) {}

    ResourceTracer(स्थिर ResourceTracer&, int32_t) {}

    व्योम trace(स्थिर अक्षर*) स्थिर {}

    व्योम traceOpen() स्थिर {}

    व्योम maybeTrace(स्थिर अक्षर*) स्थिर {}
};

/**
 * Default trivial implementation when --enable-tracing is not used.
 */
class FileTracer {
खुला:
    अटल व्योम traceOpen(स्थिर अक्षर*, स्थिर अक्षर*, स्थिर अक्षर*) {}
};

U_NAMESPACE_END

#पूर्ण_अगर // U_ENABLE_TRACING

#पूर्ण_अगर //__RESTRACE_H__
