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

// Class header file.
#include "XalanElemTextLiteralAllocator.hpp"



XALAN_CPP_NAMESPACE_BEGIN



XalanElemTextLiteralAllocator::XalanElemTextLiteralAllocator(MemoryManagerType&  theManager, size_type	theBlockCount) :
	m_allocator(theManager, theBlockCount)
{
}



XalanElemTextLiteralAllocator::~XalanElemTextLiteralAllocator()
{
}



XalanElemTextLiteralAllocator::data_type*
XalanElemTextLiteralAllocator::create(
			StylesheetConstructionContext&	constructionContext,
			Stylesheet&						stylesheetTree,
			int								lineNumber,
			int								columnNumber,
            const XalanDOMChar*				ch,
			XalanDOMString::size_type		start,
			XalanDOMString::size_type		length,
			bool							preserveSpace,
            bool							disableOutputEscaping)
{
	data_type* const	theBlock = m_allocator.allocateBlock();
	assert(theBlock != 0);

	data_type* const	theResult =
		new(theBlock) data_type(
				constructionContext,
				stylesheetTree,
				lineNumber,
				columnNumber,
				ch,
				start,
				length,
				preserveSpace,
				disableOutputEscaping);

	m_allocator.commitAllocation(theBlock);

	return theResult;
}



XALAN_CPP_NAMESPACE_END
