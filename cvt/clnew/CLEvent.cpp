#include <CLEvent.h>
#include <CLCommandQueue.h>

namespace cvt {

	CLCommandQueue CLEvent::queue() const
	{
		return CLCommandQueue( eventCommandQueue() );
	}

/*	CLEvent::CLContext context() const
	{
		return CLContext( eventContext() );
	}*/

}
