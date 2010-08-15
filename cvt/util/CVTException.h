#ifndef CVTEXCEPTION_H
#define CVTEXCEPTION_H
#include <exception>

namespace cvt {

class CVTException : std::exception
{
    public:
	CVTException() throw(): msg( "Unknown" ) {};
	CVTException( std::string s ) throw() : msg( s ) {};
	~CVTException() throw() {};
	const char* what() const throw() { return msg.c_str(); };

    private:
	std::string msg;
};

}

#endif
