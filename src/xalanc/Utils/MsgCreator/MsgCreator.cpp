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

#include "ICUResHandler.hpp"
#include "InMemHandler.hpp"
#include "MsgCreator.hpp"
#include "NLSHandler.hpp"

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>

#include <cassert>
#if !defined(NDEBUG) && defined(_MSC_VER)
#include <crtdbg.h>
#endif


XALAN_USING_STD(cerr)
XALAN_USING_STD(cout)
XALAN_USING_STD(endl)

static const char* s_xalanMsgFileName ="XalanMsg_";

static const char* s_txtExten = ".txt";

static const char* s_msgExten = ".msg";

static const char* s_en = "en_US";

static const char* DATA_FILE_NAME="LocalMsgData.hpp";

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    cout << "\nUsage:\n"
		"    MsgCreator [options] <XML file>\n\n"
		"This program reads XML message file, and then creates C++  " << endl <<
		" source / data files for message localization (for .dll/.so/.cat build) \n"
		
		"Options:\n"
		"   <XML file>   XLIFF file with messages. Required.  \n"
		"   -TYPE xxx    Type of localization [icu | nls | inmem*]\n"
		"   -LOCALE      Example: [ fr | fr_FR ]; The default value is 'en_US' \n"
		"  * = Default if not provided explicitly.\n\n"
		"    -?          Show this help.\n\n"
		"\n"
		<<  endl;
}


class CmdLineParams
{
	
public:
	
	typedef enum{

		ICU_LOCALMSG = 0,
		INMEM_LOCALMSG,
		NLS_LOCALMSG

	}TypeOfLocaleMsg;
	
	const char*		inXMLFileName;
	
	TypeOfLocaleMsg  enTypeOfLocaleMsg;
	
	const char*      encodingName;
	
	char		localeName[6];

	CmdLineParams():
		inXMLFileName(0),
		enTypeOfLocaleMsg(INMEM_LOCALMSG),
		encodingName("LATIN1")
		{  
			localeName[0] = 0;
			strcpy( localeName, s_en);
		}

	~CmdLineParams() {}
};


bool
getArgs(
		int				argc,
		char*			argv[],
		CmdLineParams&	p,
		const char**	pErrorMsg)
{

	bool fSuccess = true;

	const char*		errorMessage = 0;

	if ( argc > 7)
	{
		fSuccess = false;
		errorMessage = "Too many parameters\n";
	}

	for (int i = 1; i < argc && fSuccess == true; ++i)
	{
		if (!strcmp("-h", argv[i]) || !strcmp("-?", argv[i]))
		{
			fSuccess = false;
		}
		else if (!strcmp("-LOCALE", argv[i]) )
		{
			++i;
			if( i < argc && argv[i][0] != '-' )
			{
#if defined(XALAN_STRICT_ANSI_HEADERS)
				using std::size_t;
#endif
				const size_t	localeLen = strlen(argv[i]);

				if ( localeLen != 2 && localeLen !=5)
				{
					fSuccess = false;
					errorMessage = "Unrecognized locale\n";
				}

				if ( fSuccess  && localeLen == 5 && argv[i][2] != '_')
				{
					fSuccess = false;
					errorMessage = "Unrecognized form of locale\n";
				}
				if ( fSuccess )
				{
					strcpy(p.localeName,argv[i]);
					
					p.localeName[5]='\0'; // just in case
				}
			}
		}
		else if (!strcmp("-TYPE", argv[i]) )
		{
			++i;
			
			if( i < argc && argv[i][0] != '-' )
			{
				if( !strcmp("icu",argv[i] ))
				{
					p.enTypeOfLocaleMsg = CmdLineParams::ICU_LOCALMSG;
					
				}else if( !strcmp("nls",argv[i] ))
				{
					p.enTypeOfLocaleMsg = CmdLineParams::NLS_LOCALMSG;
					
				}
				else if( !strcmp("inmem",argv[i] ))
				{
					p.enTypeOfLocaleMsg = CmdLineParams::INMEM_LOCALMSG;
					
				}else 
				{ 
					fSuccess = false;
					errorMessage = "Unknown TYPE of localization\n";
				}
			}
			else
			{
				fSuccess = false;
			}
		}else if ( argv[i][0] != '-' )
		{
			p.inXMLFileName = argv[i];
		}
	}

	if ( fSuccess && p.inXMLFileName == 0)
	{
		fSuccess = false;
		*pErrorMsg = "Please provide XLIFF file.\n";
	}

	if ( fSuccess )
	{
		*pErrorMsg = 0;
	}
	else
	{
		*pErrorMsg = errorMessage;
	}

	return fSuccess;
}


static char* buildOutputFileName(const CmdLineParams&	p)
{
	char* pOutputFileName = new char[80];
	pOutputFileName[0] = '\0';

	switch(p.enTypeOfLocaleMsg)
	{
		
	case CmdLineParams::ICU_LOCALMSG:
		{
			strcpy(pOutputFileName, p.localeName);
			
			strcat(pOutputFileName, s_txtExten);
			
			break;
		}
	case CmdLineParams::INMEM_LOCALMSG:
		{
			strcpy(pOutputFileName, DATA_FILE_NAME);
			
			break;
		}
		
	case CmdLineParams::NLS_LOCALMSG:
		{
			strcpy(pOutputFileName,s_xalanMsgFileName);
			
			strncat(pOutputFileName, p.localeName,6);
			
			strcat(pOutputFileName, s_msgExten);
			
			break;
		}
	default:
		{
			// in that stage the local should be defined
			assert ( 0 );
		}
	}
	return pOutputFileName;
}




// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
	
#if !defined(NDEBUG) && defined(_MSC_VER)
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif
    
	CmdLineParams	theParams;

	int iReturnValue = 0;

	const char* errorMessage = 0;

	if (getArgs(argC, argV, theParams, &errorMessage) == false)
	{
		if ( errorMessage != 0)
		{
			cerr << errorMessage;
		}
		usage();
		iReturnValue = 1;
		
	}
	else
	{
		
		try
		{
			XMLPlatformUtils::Initialize();
		}
		
		catch (const XMLException& toCatch)
		{
			cerr << "Error during initialization! : "
				<< StrX(toCatch.getMessage()) << endl;
			iReturnValue = 2;
		}
		if (iReturnValue == 0)
		{
			
			{	//  Create a SAX parser object. 
				SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
				
				parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
				
				parser->setFeature(XMLUni::fgXercesDynamic, true);
				
				parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);
				
				parser->setFeature(XMLUni::fgXercesSchema, true);
				
				parser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
				
				parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
				

				//  Create the handler object and install it as the document and error
				//  handler for the parser. Then parse the file and catch any exceptions
				//  that propogate out
				//

				int errorCount = 0;

				const char* 		fileName = theParams.inXMLFileName;

				char * const		pOutputFileName = buildOutputFileName(theParams);

				SAX2Handler* handler = 0;
				try
				{
					switch(theParams.enTypeOfLocaleMsg){
					case CmdLineParams::ICU_LOCALMSG:
						{
							handler = new ICUResHandler(pOutputFileName);
							break;
						}
					case CmdLineParams::INMEM_LOCALMSG:
						{
							handler = new InMemHandler(pOutputFileName);
							break;
						}
					case CmdLineParams::NLS_LOCALMSG:
						{
							handler = new NLSHandler(pOutputFileName);
							break;
						}
					default:
						{
							assert (0);
						}
					}
				}
				
				catch(const XalanFileOutputStream::XalanFileOutputStreamOpenException& e)
				{
					cerr << "\nAn error occurred\n  Error: ";
					if (e.m_pMessage != 0)
					{
						cerr << e.m_pMessage <<  endl;
					}
					else
					{
						cerr << "unknown (XalanFileOutputStreamOpenException) "  << endl;
						
					}
					
					iReturnValue = 4;	
					
					
				}
				catch(const XalanFileOutputStream::XalanFileOutputStreamWriteException& e)
				{
					cerr << "\nAn error occurred\n  Error: ";
					if (e.m_pMessage != 0)
					{
						cerr << e.m_pMessage << endl;
					}
					else
					{
						cerr << "unknown (XalanFileOutputStreamWriteException) " <<  endl;
						
					}
					
					iReturnValue = 5;	
					
				}
				catch (...)
				{
					cerr << "Unknown error occured." << endl;
					iReturnValue = 6;
				}
				
				// run the parser
				if ( iReturnValue == 0)
				{
					try
					{				
						handler->setLocale(theParams.localeName);
	
						parser->setContentHandler(handler);

						parser->setErrorHandler(handler);
						
						parser->parse(fileName);
						
						errorCount = parser->getErrorCount();
					}
					
					catch (const XMLException& toCatch)
					{
						cerr << "\nAn error occurred\n  Error: "
							<< StrX(toCatch.getMessage()) << endl;

						iReturnValue = 7;
					}
					catch (...)
					{	
						cerr << endl << "General error occured." << endl;
						iReturnValue = 8;
					}
				}
				

				delete handler;

				delete[] pOutputFileName;

				//
				//  Delete the parser itself.  Must be done prior to calling Terminate, below.
				//
				delete parser;

				if (errorCount != 0)
					iReturnValue = 9;

			}
			// And call the termination method
			XMLPlatformUtils::Terminate();
			
			
			
		}
		
	}
	return iReturnValue;
}

