// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#અન્તર્ભાવ "unicode/utypes.h"

#જો U_ENABLE_TRACING

#અન્તર્ભાવ "restrace.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "utracimp.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = મૂળભૂત;

શૂન્ય ResourceTracer::trace(અચલ અક્ષર* resType) અચલ {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    અચલ અક્ષર kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, કદ(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

શૂન્ય ResourceTracer::traceOpen() અચલ {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) અચલ {
    જો (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } નહીંતર {
        fParent->getFilePath(output, status);
    }
    પાછા output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) અચલ {
    જો (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } નહીંતર {
        fParent->getResPath(output, status);
    }
    જો (f_ચાવી) {
        output.append('/', status);
        output.append(f_ચાવી, status);
    }
    જો (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_સંખ્યા(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    પાછા output;
}

શૂન્ય FileTracer::traceOpen(અચલ અક્ષર* path, અચલ અક્ષર* type, અચલ અક્ષર* name) {
    જો (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } નહીંતર {
        traceOpenDataFile(path, type, name);
    }
}

શૂન્ય FileTracer::traceOpenDataFile(અચલ અક્ષર* path, અચલ અક્ષર* type, અચલ અક્ષર* name) {
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

શૂન્ય FileTracer::traceOpenResFile(અચલ અક્ષર* path, અચલ અક્ષર* name) {
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
