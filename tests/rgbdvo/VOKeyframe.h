#ifndef CVT_VO_KEYFRAME_H
#define CVT_VO_KEYFRAME_H

namespace cvt
{
	class VOKeyframe
	{
		public:
			/**
			 * @param	rgb		RGB Image
			 * @param	depth	Depth Image
			 * @param	pose	pose for this keyframe
			 */
			VOKeyframe( const Image& rgb, const Image& depth, const Matrix4d& pose );
			
			~VOKeyframe();

		private:
			// we need:
			// jacobians per pixel 
	};
}

#endif
