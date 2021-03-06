/*
 * Copyright 1999-2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "TestHarness.hpp"


#include <xalanc/Include/XalanMemoryManagement.hpp>


#include <xalanc/PlatformSupport/DOMStringHelper.hpp>



XALAN_USING_XALAN(XalanMemMgrs)
XALAN_USING_XALAN(CharVectorType)
XALAN_USING_XALAN(c_str)


XALAN_USING_STD(ostringstream)



TestCase::TestCase() :
        stylesheet(""),
        inputDocument(""),
        resultDocument(""),
        resultDirectory(""),
        goldResult(""),
        numIterations(1),
        minTimeToExecute(0),
        verifyResult(false),
        inputMode("file"),
        processorOptions(XalanMemMgrs::getDefaultXercesMemMgr())
{
}

TestCase::TestCase(const TestCase& theRhs) :
        stylesheet(theRhs.stylesheet),             
        inputDocument(theRhs.inputDocument),
        resultDocument(theRhs.resultDocument),
        resultDirectory(theRhs.resultDirectory),
        goldResult(theRhs.goldResult),
        numIterations(theRhs.numIterations),
        minTimeToExecute(theRhs.minTimeToExecute),
        verifyResult(theRhs.verifyResult),
        inputMode(theRhs.inputMode),
        processorOptions(theRhs.processorOptions, XalanMemMgrs::getDefaultXercesMemMgr())
{
}


