#ifndef TOOLS_ABSTRACT_CAMERA
#define TOOLS_ABSTRACT_CAMERA

class AbstractCamera 
{		
	virtual ~AbstractCamera()=0;
	
	virtual IplImage* image() = 0;
};

#endif