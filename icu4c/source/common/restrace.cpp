// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ U_ENABLE_TRACING

#അന്തര്ഭാവ "restrace.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "utracimp.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = സ്ഥിരസ്ഥിതി;

ശൂന്യം ResourceTracer::trace(സ്ഥിരം അക്ഷരം* resType) സ്ഥിരം {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    സ്ഥിരം അക്ഷരം kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, വലുപ്പം(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

ശൂന്യം ResourceTracer::traceOpen() സ്ഥിരം {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } അല്ലെങ്കിൽ {
        fParent->getFilePath(output, status);
    }
    തിരികെ output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } അല്ലെങ്കിൽ {
        fParent->getResPath(output, status);
    }
    എങ്കിൽ (f_താക്കോൽ) {
        output.append('/', status);
        output.append(f_താക്കോൽ, status);
    }
    എങ്കിൽ (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_സംഖ്യ(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    തിരികെ output;
}

ശൂന്യം FileTracer::traceOpen(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* type, സ്ഥിരം അക്ഷരം* name) {
    എങ്കിൽ (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } അല്ലെങ്കിൽ {
        traceOpenDataFile(path, type, name);
    }
}

ശൂന്യം FileTracer::traceOpenDataFile(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* type, സ്ഥിരം അക്ഷരം* name) {
    UTRACE_ENTRY(UTRACE_UDATA_DATA_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

ശൂന്യം FileTracer::traceOpenResFile(സ്ഥിരം അക്ഷരം* path, സ്ഥിരം അക്ഷരം* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING
