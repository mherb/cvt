#ifndef CVT_LABEL_H
#define CVT_LABEL_H

#include <cvt/gui/Widget.h>
#include <cvt/util/Signal.h>
#include <cvt/gfx/Alignment.h>

namespace cvt {
	class Label : public Widget
	{
		public:
			Label( const std::string& label, Alignment align = ALIGN_LEFT | ALIGN_VCENTER );
			~Label();
			const std::string& label() const;
			void setLabel( const std::string label );
			Alignment labelAlignment() const;
			void setLabelAlignment( Alignment a );

		private:
			void paintEvent( PaintEvent* e, GFX* g );
			Label( const Label& b );

			std::string _label;
			Alignment _aligment;

	};

	inline const std::string& Label::label() const
	{
		return _label;
	}

	inline void Label::setLabel( const std::string label )
	{
		_label = label;
		update();
	}

	inline Alignment Label::labelAlignment() const
	{
		return _aligment;
	}

	inline void Label::setLabelAlignment( Alignment a )
	{
		_aligment = a;
		update();
	}

}

#endif
