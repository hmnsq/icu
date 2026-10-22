// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି U_ENABLE_TRACING

#ଅନ୍ତର୍ଭାଵ "restrace.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "utracimp.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

ଶୂନ୍ୟ ResourceTracer::trace(ଅଚଳ ଅକ୍ଷର* resType) ଅଚଳ {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ଅଚଳ ଅକ୍ଷର kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, ଆକାର(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

ଶୂନ୍ୟ ResourceTracer::traceOpen() ଅଚଳ {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ଅଚଳ {
    ଯଦି (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } ଅନ୍ୟଥା {
        fParent->getFilePath(output, status);
    }
    ଫେରନ୍ତୁ output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ଅଚଳ {
    ଯଦି (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } ଅନ୍ୟଥା {
        fParent->getResPath(output, status);
    }
    ଯଦି (f_ଚାବି) {
        output.append('/', status);
        output.append(f_ଚାବି, status);
    }
    ଯଦି (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_ସଂଖ୍ୟା(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    ଫେରନ୍ତୁ output;
}

ଶୂନ୍ୟ FileTracer::traceOpen(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* type, ଅଚଳ ଅକ୍ଷର* name) {
    ଯଦି (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } ଅନ୍ୟଥା {
        traceOpenDataFile(path, type, name);
    }
}

ଶୂନ୍ୟ FileTracer::traceOpenDataFile(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* type, ଅଚଳ ଅକ୍ଷର* name) {
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

ଶୂନ୍ୟ FileTracer::traceOpenResFile(ଅଚଳ ଅକ୍ଷର* path, ଅଚଳ ଅକ୍ଷର* name) {
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
