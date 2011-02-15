#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <sstream>
#include <iostream>
#include <cstring>

namespace cvt {

#define CVTException( s ) ( ::cvt::Exception( s, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

	class Exception : std::exception
	{
		public:
			Exception() throw(): msg( "Unknown" )
            {
#ifdef DEBUG
                    std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
            }
        
			Exception( std::string s ) throw() : msg( s ) {}
			~Exception() throw() {}
			const char* what() const throw() { return msg.c_str(); }

			Exception( int err, const char* file, size_t line, const char* func ) throw()
			{
				std::stringstream str;
				str << strerror( err );
				str << " ( FILE: ";
				str << file;
				str << " LINE: ";
				str << line;
				str << " FUNC: ";
				str << func;
				str << " )";
				msg = str.str();
                
#ifdef DEBUG
                std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
			}


			Exception( std::string s, const char* file, size_t line, const char* func ) throw()
			{
				std::stringstream str;
				str << s;
				str << " ( FILE: ";
				str << file;
				str << " LINE: ";
				str << line;
				str << " FUNC: ";
				str << func;
				str << " )";
				msg = str.str();
#ifdef DEBUG
                std::cerr << "EXCEPTION: " << msg << std::endl;
#endif
			}

		private:
			std::string msg;
	};

}

#endif
