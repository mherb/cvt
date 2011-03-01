#include <cvt/vision/FAST.h>

namespace cvt {

    bool FAST::isDarkerCorner9( const uint8_t * p, const int barrier )
	{
		if( !( p[ pixel[ 15 ] ] < barrier ) ) {     // ???????????????-
            if( !(p[ pixel[ 8 ] ] < barrier ) ) {   // ????????-??????-
				return false;
			}                                       // ????????@??????-
            if( !(p[ pixel[ 7 ] ] < barrier ) ) {   // ???????-@??????-
				return false;
			}										// ???????@@??????-
            if( !(p[ pixel[ 6 ] ] < barrier ) ) {   // ??????-@@??????-
				return false;
			}                                           // ??????@@@??????-
            if( !(p[ pixel[ 3 ] ] < barrier ) ) {		// ??????@@@??????-
                if( !(p[ pixel[ 10 ] ] < barrier ) ){  // ?????-@@@?????--
					return false;
				}                               // ?????-@@@?????@- :p13
                if( !(p[ pixel[ 11 ] ] < barrier ) ) {   // ?????-@@@????-@-
					return false;
				}                               // ?????-@@@????@@-
                if( !(p[ pixel[ 15 ] ] < barrier ) ) {  // ?????-@@@-???@@-
					return false;
				}                               // ?????-@@@@???@@-
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ?????-@@@@-??@@-
					return false;
				} // ?????-@@@@@??@@-
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ?????-@@@@@-?@@-
					return false;
				} // ?????-@@@@@@?@@-
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // ?????-@@@@@@-@@-
					return false;
				} // ?????-@@@@@@@@@-
				return true;
			} // ?????@@@@??????-
            if( !(p[ pixel[ 4 ] ] < barrier ) ) { // ????-@@@@??????-
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ????-@@@@????-?-
					return false;
				} // ????-@@@@????@?-
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ????-@@@@-???@?-
					return false;
				} // ????-@@@@@???@?-
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ????-@@@@@-??@?-
					return false;
				} // ????-@@@@@@??@?-
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ????-@@@@@@-?@?-
					return false;
				} // ????-@@@@@@@?@?-
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // ????-@@@@@@@-@?-
					return false;
				} // ????-@@@@@@@@@?-
				return true;
			} // ????@@@@@??????-
            if( !(p[ pixel[ 5 ] ] < barrier ) ) { // ???-@@@@@??????-
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ???-@@@@@-?????-
					return false;
				} // ???-@@@@@@?????-
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ???-@@@@@@-????-
					return false;
				} // ???-@@@@@@@????-
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ???-@@@@@@@-???-
					return false;
				} // ???-@@@@@@@@???-
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // ???-@@@@@@@@-??-
					return false;
				} // ???-@@@@@@@@@??-
				return true;
			} // ???@@@@@@??????-
            if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ???@@@@@@-?????-
                if( !(p[ pixel[ 8 ] ] < barrier ) ) { // -??@@@@@@-?????-
					return false;
				} // @??@@@@@@-?????-
                if( !(p[ pixel[ 7 ] ] < barrier ) ) { // @-?@@@@@@-?????-
					return false;
				} // @@?@@@@@@-?????-
                if( !(p[ pixel[ 6 ] ] < barrier ) ) { // @@-@@@@@@-?????-
					return false;
				} // @@@@@@@@@-?????-
				return true;
			} // ???@@@@@@@?????-
            if( !(p[ pixel[ 6 ] ] < barrier ) ) { // ??-@@@@@@@?????-
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ??-@@@@@@@-????-
					return false;
				} // ??-@@@@@@@@????-
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ??-@@@@@@@@-???-
					return false;
				} // ??-@@@@@@@@@???-
				return true;
			} // ??@@@@@@@@?????-
            if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ??@@@@@@@@-????-
                if( !(p[ pixel[ 7 ] ] < barrier ) ) { // ?-@@@@@@@@-????-
					return false;
				} // ?@@@@@@@@@-????-
				return true;
			} // ??@@@@@@@@@????-
			return true;
		} // ???????????????@
        if( !(p[ pixel[ 10 ] ] < barrier ) ) { // ??????????????-@
            if( !(p[ pixel[ 3 ] ] < barrier ) ) { // ?????-????????-@
				return false;
			} // ?????@????????-@
            if( !(p[ pixel[ 1 ]] < barrier ) ) { // ?????@?-??????-@
				return false;
			} // ?????@?@??????-@
            if( !(p[ pixel[ 2 ] ] < barrier ) ) { // ?????@-@??????-@
				return false;
			} // ?????@@@??????-@
            if( !(p[ pixel[ 4 ] ] < barrier ) ) { // ????-@@@??????-@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ????-@@@?????--@
					return false;
				} // ????-@@@?????@-@
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // ????-@@@????-@-@
					return false;
				} // ????-@@@????@@-@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // ????-@@@-???@@-@
					return false;
				} // ????-@@@@???@@-@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ????-@@@@-??@@-@
					return false;
				} // ????-@@@@@??@@-@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ????-@@@@@-?@@-@
					return false;
				} // ????-@@@@@@?@@-@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ????-@@@@@@-@@-@
					return false;
				} // ????-@@@@@@@@@-@
				return true;
			} // ????@@@@??????-@
            if( !(p[ pixel[ 5 ] ] < barrier ) ) { // ???-@@@@??????-@
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // ???-@@@@????-?-@
					return false;
				} // ???-@@@@????@?-@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // ???-@@@@-???@?-@
					return false;
				} // ???-@@@@@???@?-@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ???-@@@@@-??@?-@
					return false;
				} // ???-@@@@@@??@?-@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ???-@@@@@@-?@?-@
					return false;
				} // ???-@@@@@@@?@?-@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ???-@@@@@@@-@?-@
					return false;
				} // ???-@@@@@@@@@?-@
				return true;
			} // ???@@@@@??????-@
            if( !(p[ pixel[ 6 ] ] < barrier ) ) { // ??-@@@@@??????-@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // ??-@@@@@-?????-@
					return false;
				} // ??-@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ??-@@@@@@-????-@
					return false;
				} // ??-@@@@@@@????-@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ??-@@@@@@@-???-@
					return false;
				} // ??-@@@@@@@@???-@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // ??-@@@@@@@@-??-@
					return false;
				} // ??-@@@@@@@@@??-@
				return true;
			} // ??@@@@@@??????-@
            if( !(p[ pixel[ 7 ] ] < barrier ) ) { // ?-@@@@@@??????-@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // ?-@@@@@@-?????-@
					return false;
				} // ?-@@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // ?-@@@@@@@-????-@
					return false;
				} // ?-@@@@@@@@????-@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // ?-@@@@@@@@-???-@
					return false;
				} // ?-@@@@@@@@@???-@
				return true;
			} // ?@@@@@@@??????-@
            if( !(p[ pixel[ 8 ] ] < barrier ) ) { // -@@@@@@@??????-@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // -@@@@@@@-?????-@
					return false;
				} // -@@@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // -@@@@@@@@-????-@
					return false;
				} // -@@@@@@@@@????-@
				return true;
			} // @@@@@@@@??????-@
			return true;
		} // ??????????????@@
        if( !(p[ pixel[ 8 ] ] < barrier ) ) { // -?????????????@@
            if( !(p[ pixel[ 1 ]] < barrier ) ) { // -??????-??????@@
				return false;
			} // -??????@??????@@
            if( !(p[ pixel[ 15 ] ] < barrier ) ) { // -??????@?-????@@
				return false;
			} // -??????@?@????@@
            if( !(p[ pixel[ 0 ]] < barrier ) ) { // -??????@-@????@@
				return false;
			} // -??????@@@????@@
            if( !(p[ pixel[ 14 ] ] < barrier ) ) { // -??????@@@-???@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // -?????-@@@-???@@
					return false;
				} // -?????@@@@-???@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // -????-@@@@-???@@
					return false;
				} // -????@@@@@-???@@
                if( !(p[ pixel[ 7 ] ] < barrier ) ) { // --???@@@@@-???@@
					return false;
				} // -@???@@@@@-???@@
                if( !(p[ pixel[ 6 ] ] < barrier ) ) { // -@-??@@@@@-???@@
					return false;
				} // -@@??@@@@@-???@@
                if( !(p[ pixel[ 5 ] ] < barrier ) ) { // -@@-?@@@@@-???@@
					return false;
				} // -@@@?@@@@@-???@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // -@@@-@@@@@-???@@
					return false;
				} // -@@@@@@@@@-???@@
				return true;
			} // -??????@@@@???@@
            if( !(p[ pixel[ 11 ] ] < barrier ) ) { // -??????@@@@-??@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // -?????-@@@@-??@@
					return false;
				} // -?????@@@@@-??@@
                if( !(p[ pixel[ 6 ] ] < barrier ) ) { // -?-???@@@@@-??@@
					return false;
				} // -?@???@@@@@-??@@
                if( !(p[ pixel[ 5 ] ] < barrier ) ) { // -?@-??@@@@@-??@@
					return false;
				} // -?@@??@@@@@-??@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // -?@@-?@@@@@-??@@
					return false;
				} // -?@@@?@@@@@-??@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // -?@@@-@@@@@-??@@
					return false;
				} // -?@@@@@@@@@-??@@
				return true;
			} // -??????@@@@@??@@
            if( !(p[ pixel[ 12 ] ] < barrier ) ) { // -??????@@@@@-?@@
                if( !(p[ pixel[ 5 ] ] < barrier ) ) { // -??-???@@@@@-?@@
					return false;
				} // -??@???@@@@@-?@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // -??@-??@@@@@-?@@
					return false;
				} // -??@@??@@@@@-?@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // -??@@-?@@@@@-?@@
					return false;
				} // -??@@@?@@@@@-?@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // -??@@@-@@@@@-?@@
					return false;
				} // -??@@@@@@@@@-?@@
				return true;
			} // -??????@@@@@@?@@
            if( !(p[ pixel[ 11 ] ] < barrier ) ) { // -??????@@@@@@-@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // -???-??@@@@@@-@@
					return false;
				} // -???@??@@@@@@-@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // -???@-?@@@@@@-@@
					return false;
				} // -???@@?@@@@@@-@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // -???@@-@@@@@@-@@
					return false;
				} // -???@@@@@@@@@-@@
				return true;
			} // -??????@@@@@@@@@
			return true;
		} // @?????????????@@
        if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @????????????-@@
            if( !(p[ pixel[ 2 ] ] < barrier ) ) { // @?????-??????-@@
				return false;
			} // @?????@??????-@@
            if( !(p[ pixel[ 3 ] ] < barrier ) ) { // @????-@??????-@@
				return false;
			} // @????@@??????-@@
            if( !(p[ pixel[ 4 ] ] < barrier ) ) { // @???-@@??????-@@
				return false;
			} // @???@@@??????-@@
            if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @??-@@@??????-@@
                if( !(p[ pixel[ 12 ] ] < barrier ) ) { // @??-@@@?????--@@
					return false;
				} // @??-@@@?????@-@@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @??-@@@????-@-@@
					return false;
				} // @??-@@@????@@-@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @??-@@@-???@@-@@
					return false;
				} // @??-@@@@???@@-@@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // @??-@@@@-??@@-@@
					return false;
				} // @??-@@@@@??@@-@@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @??-@@@@@-?@@-@@
					return false;
				} // @??-@@@@@@?@@-@@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @??-@@@@@@-@@-@@
					return false;
				} // @??-@@@@@@@@@-@@
				return true;
			} // @??@@@@??????-@@
            if( !(p[ pixel[ 6 ] ] < barrier ) ) { // @?-@@@@??????-@@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @?-@@@@????-?-@@
					return false;
				} // @?-@@@@????@?-@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @?-@@@@-???@?-@@
					return false;
				} // @?-@@@@@???@?-@@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // @?-@@@@@-??@?-@@
					return false;
				} // @?-@@@@@@??@?-@@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @?-@@@@@@-?@?-@@
					return false;
				} // @?-@@@@@@@?@?-@@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @?-@@@@@@@-@?-@@
					return false;
				} // @?-@@@@@@@@@?-@@
				return true;
			} // @?@@@@@??????-@@
            if( !(p[ pixel[ 7 ] ] < barrier ) ) { // @-@@@@@??????-@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @-@@@@@-?????-@@
					return false;
				} // @-@@@@@@?????-@@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // @-@@@@@@-????-@@
					return false;
				} // @-@@@@@@@????-@@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @-@@@@@@@-???-@@
					return false;
				} // @-@@@@@@@@???-@@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @-@@@@@@@@-??-@@
					return false;
				} // @-@@@@@@@@@??-@@
				return true;
			} // @@@@@@@??????-@@
			return true;
		} // @????????????@@@
        if( !(p[ pixel[ 7 ] ] < barrier ) ) { // @-???????????@@@
            if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @-????????-??@@@
				return false;
			} // @-????????@??@@@
            if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @-???????-@??@@@
				return false;
			} // @-???????@@??@@@
            if( !(p[ pixel[ 0 ]] < barrier ) ) { // @-??????-@@??@@@
				return false;
			} // @-??????@@@??@@@
            if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @-??????@@@-?@@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @-?????-@@@-?@@@
					return false;
				} // @-?????@@@@-?@@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // @-????-@@@@-?@@@
					return false;
				} // @-????@@@@@-?@@@
                if( !(p[ pixel[ 6 ] ] < barrier ) ) { // @--???@@@@@-?@@@
					return false;
				} // @-@???@@@@@-?@@@
                if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @-@-??@@@@@-?@@@
					return false;
				} // @-@@??@@@@@-?@@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // @-@@-?@@@@@-?@@@
					return false;
				} // @-@@@?@@@@@-?@@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // @-@@@-@@@@@-?@@@
					return false;
				} // @-@@@@@@@@@-?@@@
				return true;
			} // @-??????@@@@?@@@
            if( !(p[ pixel[ 12 ] ] < barrier ) ) { // @-??????@@@@-@@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @-?????-@@@@-@@@
					return false;
				} // @-?????@@@@@-@@@
                if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @-?-???@@@@@-@@@
					return false;
				} // @-?@???@@@@@-@@@
                if( !(p[ pixel[ 4 ] ] < barrier ) ) { // @-?@-??@@@@@-@@@
					return false;
				} // @-?@@??@@@@@-@@@
                if( !(p[ pixel[ 3 ] ] < barrier ) ) { // @-?@@-?@@@@@-@@@
					return false;
				} // @-?@@@?@@@@@-@@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // @-?@@@-@@@@@-@@@
					return false;
				} // @-?@@@@@@@@@-@@@
				return true;
			} // @-??????@@@@@@@@
			return true;
		} // @@???????????@@@
        if( !(p[ pixel[ 12 ] ] < barrier ) ) { // @@??????????-@@@
            if( !(p[ pixel[ 3 ] ] < barrier ) ) { // @@???-??????-@@@
				return false;
			} // @@???@??????-@@@
            if( !(p[ pixel[ 4 ] ] < barrier ) ) { // @@??-@??????-@@@
				return false;
			} // @@??@@??????-@@@
            if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @@?-@@??????-@@@
				return false;
			} // @@?@@@??????-@@@
            if( !(p[ pixel[ 6 ] ] < barrier ) ) { // @@-@@@??????-@@@
                if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @@-@@@?????--@@@
					return false;
				} // @@-@@@?????@-@@@
                if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @@-@@@????-@-@@@
					return false;
				} // @@-@@@????@@-@@@
                if( !(p[ pixel[ 2 ] ] < barrier ) ) { // @@-@@@-???@@-@@@
					return false;
				} // @@-@@@@???@@-@@@
                if( !(p[ pixel[ 1 ]] < barrier ) ) { // @@-@@@@-??@@-@@@
					return false;
				} // @@-@@@@@??@@-@@@
                if( !(p[ pixel[ 0 ]] < barrier ) ) { // @@-@@@@@-?@@-@@@
					return false;
				} // @@-@@@@@@?@@-@@@
                if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @@-@@@@@@-@@-@@@
					return false;
				} // @@-@@@@@@@@@-@@@
				return true;
			} // @@@@@@??????-@@@
			return true;
		} // @@??????????@@@@
        if( !(p[ pixel[ 6 ] ] < barrier ) ) { // @@-?????????@@@@
            if( !(p[ pixel[ 15 ] ] < barrier ) ) { // @@-??????-??@@@@
				return false;
			} // @@-??????@??@@@@
            if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @@-??????@-?@@@@
				return false;
			} // @@-??????@@?@@@@
            if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @@-??????@@-@@@@
				return false;
			} // @@-??????@@@@@@@
			return true;
		} // @@@?????????@@@@
        if( !(p[ pixel[ 11 ] ] < barrier ) ) { // @@@????????-@@@@
            if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @@@-???????-@@@@
				return false;
			} // @@@@???????-@@@@
            if( !(p[ pixel[ 4 ] ] < barrier ) ) { // @@@@-??????-@@@@
				return false;
			} // @@@@@??????-@@@@
			return true;
		} // @@@????????@@@@@
        if( !(p[ pixel[ 5 ] ] < barrier ) ) { // @@@-???????@@@@@
            if( !(p[ pixel[ 14 ] ] < barrier ) ) { // @@@-??????-@@@@@
				return false;
			} // @@@-??????@@@@@@
			return true;
		} // @@@@???????@@@@@
		return true;
	}
    /* pixel, stride, 3x stride barrier (center+thres)
	 */
    bool FAST::isBrighterCorner9( const uint8_t * p, const int barrier )
	{
		if( !( p[ pixel[ 9 ] ] > barrier ) ) {	// ???????????????-
            if( !(p[ pixel[ 0 ]] > barrier ) ) {		// ???????-???????-
				return false;
			}								// ????????@??????-
            if( !(p[ pixel[ 1 ]] > barrier ) ) {		// ???????-@??????-
				return false;
			}								// ???????@@??????-
            if( !(p[ pixel[ 2 ] ] > barrier ) ) {		// ??????-@@??????-
				return false;
			}								// ??????@@@??????-
            if( !(p[ pixel[ 3 ] ] > barrier ) ) {		// ?????-@@@??????-
                if( !(p[ pixel[ 10 ] ] > barrier ) ){ // ?????-@@@?????--
					return false;
				} // ?????-@@@?????@-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ?????-@@@????-@-
					return false;
				} // ?????-@@@????@@-
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ?????-@@@-???@@-
					return false;
				} // ?????-@@@@???@@-
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ?????-@@@@-??@@-
					return false;
				} // ?????-@@@@@??@@-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ?????-@@@@@-?@@-
					return false;
				} // ?????-@@@@@@?@@-
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // ?????-@@@@@@-@@-
					return false;
				} // ?????-@@@@@@@@@-
				return true;
			} // ?????@@@@??????-
            if( !(p[ pixel[ 4 ] ] > barrier ) ) { // ????-@@@@??????-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ????-@@@@????-?-
					return false;
				} // ????-@@@@????@?-
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ????-@@@@-???@?-
					return false;
				} // ????-@@@@@???@?-
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ????-@@@@@-??@?-
					return false;
				} // ????-@@@@@@??@?-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ????-@@@@@@-?@?-
					return false;
				} // ????-@@@@@@@?@?-
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // ????-@@@@@@@-@?-
					return false;
				} // ????-@@@@@@@@@?-
				return true;
			} // ????@@@@@??????-
            if( !(p[ pixel[ 5 ] ] > barrier ) ) { // ???-@@@@@??????-
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ???-@@@@@-?????-
					return false;
				} // ???-@@@@@@?????-
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ???-@@@@@@-????-
					return false;
				} // ???-@@@@@@@????-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ???-@@@@@@@-???-
					return false;
				} // ???-@@@@@@@@???-
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // ???-@@@@@@@@-??-
					return false;
				} // ???-@@@@@@@@@??-
				return true;
			} // ???@@@@@@??????-
            if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ???@@@@@@-?????-
                if( !(p[ pixel[ 8 ] ] > barrier ) ) { // -??@@@@@@-?????-
					return false;
				} // @??@@@@@@-?????-
                if( !(p[ pixel[ 7 ] ] > barrier ) ) { // @-?@@@@@@-?????-
					return false;
				} // @@?@@@@@@-?????-
                if( !(p[ pixel[ 6 ] ] > barrier ) ) { // @@-@@@@@@-?????-
					return false;
				} // @@@@@@@@@-?????-
				return true;
			} // ???@@@@@@@?????-
            if( !(p[ pixel[ 6 ] ] > barrier ) ) { // ??-@@@@@@@?????-
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ??-@@@@@@@-????-
					return false;
				} // ??-@@@@@@@@????-
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ??-@@@@@@@@-???-
					return false;
				} // ??-@@@@@@@@@???-
				return true;
			} // ??@@@@@@@@?????-
            if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ??@@@@@@@@-????-
                if( !(p[ pixel[ 7 ] ] > barrier ) ) { // ?-@@@@@@@@-????-
					return false;
				} // ?@@@@@@@@@-????-
				return true;
			} // ??@@@@@@@@@????-
			return true;
		} // ???????????????@
        if( !(p[ pixel[ 10 ] ] > barrier ) ) { // ??????????????-@
            if( !(p[ pixel[ 3 ] ] > barrier ) ) { // ?????-????????-@
				return false;
			} // ?????@????????-@
            if( !(p[ pixel[ 1 ]] > barrier ) ) { // ?????@?-??????-@
				return false;
			} // ?????@?@??????-@
            if( !(p[ pixel[ 2 ] ] > barrier ) ) { // ?????@-@??????-@
				return false;
			} // ?????@@@??????-@
            if( !(p[ pixel[ 4 ] ] > barrier ) ) { // ????-@@@??????-@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ????-@@@?????--@
					return false;
				} // ????-@@@?????@-@
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // ????-@@@????-@-@
					return false;
				} // ????-@@@????@@-@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // ????-@@@-???@@-@
					return false;
				} // ????-@@@@???@@-@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ????-@@@@-??@@-@
					return false;
				} // ????-@@@@@??@@-@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ????-@@@@@-?@@-@
					return false;
				} // ????-@@@@@@?@@-@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ????-@@@@@@-@@-@
					return false;
				} // ????-@@@@@@@@@-@
				return true;
			} // ????@@@@??????-@
            if( !(p[ pixel[ 5 ] ] > barrier ) ) { // ???-@@@@??????-@
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // ???-@@@@????-?-@
					return false;
				} // ???-@@@@????@?-@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // ???-@@@@-???@?-@
					return false;
				} // ???-@@@@@???@?-@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ???-@@@@@-??@?-@
					return false;
				} // ???-@@@@@@??@?-@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ???-@@@@@@-?@?-@
					return false;
				} // ???-@@@@@@@?@?-@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ???-@@@@@@@-@?-@
					return false;
				} // ???-@@@@@@@@@?-@
				return true;
			} // ???@@@@@??????-@
            if( !(p[ pixel[ 6 ] ] > barrier ) ) { // ??-@@@@@??????-@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // ??-@@@@@-?????-@
					return false;
				} // ??-@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ??-@@@@@@-????-@
					return false;
				} // ??-@@@@@@@????-@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ??-@@@@@@@-???-@
					return false;
				} // ??-@@@@@@@@???-@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // ??-@@@@@@@@-??-@
					return false;
				} // ??-@@@@@@@@@??-@
				return true;
			} // ??@@@@@@??????-@
            if( !(p[ pixel[ 7 ] ] > barrier ) ) { // ?-@@@@@@??????-@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // ?-@@@@@@-?????-@
					return false;
				} // ?-@@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // ?-@@@@@@@-????-@
					return false;
				} // ?-@@@@@@@@????-@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // ?-@@@@@@@@-???-@
					return false;
				} // ?-@@@@@@@@@???-@
				return true;
			} // ?@@@@@@@??????-@
            if( !(p[ pixel[ 8 ] ] > barrier ) ) { // -@@@@@@@??????-@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // -@@@@@@@-?????-@
					return false;
				} // -@@@@@@@@?????-@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // -@@@@@@@@-????-@
					return false;
				} // -@@@@@@@@@????-@
				return true;
			} // @@@@@@@@??????-@
			return true;
		} // ??????????????@@
        if( !(p[ pixel[ 8 ] ] > barrier ) ) { // -?????????????@@
            if( !(p[ pixel[ 1 ]] > barrier ) ) { // -??????-??????@@
				return false;
			} // -??????@??????@@
            if( !(p[ pixel[ 15 ] ] > barrier ) ) { // -??????@?-????@@
				return false;
			} // -??????@?@????@@
            if( !(p[ pixel[ 0 ]] > barrier ) ) { // -??????@-@????@@
				return false;
			} // -??????@@@????@@
            if( !(p[ pixel[ 14 ] ] > barrier ) ) { // -??????@@@-???@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // -?????-@@@-???@@
					return false;
				} // -?????@@@@-???@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // -????-@@@@-???@@
					return false;
				} // -????@@@@@-???@@
                if( !(p[ pixel[ 7 ] ] > barrier ) ) { // --???@@@@@-???@@
					return false;
				} // -@???@@@@@-???@@
                if( !(p[ pixel[ 6 ] ] > barrier ) ) { // -@-??@@@@@-???@@
					return false;
				} // -@@??@@@@@-???@@
                if( !(p[ pixel[ 5 ] ] > barrier ) ) { // -@@-?@@@@@-???@@
					return false;
				} // -@@@?@@@@@-???@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // -@@@-@@@@@-???@@
					return false;
				} // -@@@@@@@@@-???@@
				return true;
			} // -??????@@@@???@@
            if( !(p[ pixel[ 11 ] ] > barrier ) ) { // -??????@@@@-??@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // -?????-@@@@-??@@
					return false;
				} // -?????@@@@@-??@@
                if( !(p[ pixel[ 6 ] ] > barrier ) ) { // -?-???@@@@@-??@@
					return false;
				} // -?@???@@@@@-??@@
                if( !(p[ pixel[ 5 ] ] > barrier ) ) { // -?@-??@@@@@-??@@
					return false;
				} // -?@@??@@@@@-??@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // -?@@-?@@@@@-??@@
					return false;
				} // -?@@@?@@@@@-??@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // -?@@@-@@@@@-??@@
					return false;
				} // -?@@@@@@@@@-??@@
				return true;
			} // -??????@@@@@??@@
            if( !(p[ pixel[ 12 ] ] > barrier ) ) { // -??????@@@@@-?@@
                if( !(p[ pixel[ 5 ] ] > barrier ) ) { // -??-???@@@@@-?@@
					return false;
				} // -??@???@@@@@-?@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // -??@-??@@@@@-?@@
					return false;
				} // -??@@??@@@@@-?@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // -??@@-?@@@@@-?@@
					return false;
				} // -??@@@?@@@@@-?@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // -??@@@-@@@@@-?@@
					return false;
				} // -??@@@@@@@@@-?@@
				return true;
			} // -??????@@@@@@?@@
            if( !(p[ pixel[ 11 ] ] > barrier ) ) { // -??????@@@@@@-@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // -???-??@@@@@@-@@
					return false;
				} // -???@??@@@@@@-@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // -???@-?@@@@@@-@@
					return false;
				} // -???@@?@@@@@@-@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // -???@@-@@@@@@-@@
					return false;
				} // -???@@@@@@@@@-@@
				return true;
			} // -??????@@@@@@@@@
			return true;
		} // @?????????????@@
        if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @????????????-@@
            if( !(p[ pixel[ 2 ] ] > barrier ) ) { // @?????-??????-@@
				return false;
			} // @?????@??????-@@
            if( !(p[ pixel[ 3 ] ] > barrier ) ) { // @????-@??????-@@
				return false;
			} // @????@@??????-@@
            if( !(p[ pixel[ 4 ] ] > barrier ) ) { // @???-@@??????-@@
				return false;
			} // @???@@@??????-@@
            if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @??-@@@??????-@@
                if( !(p[ pixel[ 12 ] ] > barrier ) ) { // @??-@@@?????--@@
					return false;
				} // @??-@@@?????@-@@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @??-@@@????-@-@@
					return false;
				} // @??-@@@????@@-@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @??-@@@-???@@-@@
					return false;
				} // @??-@@@@???@@-@@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // @??-@@@@-??@@-@@
					return false;
				} // @??-@@@@@??@@-@@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @??-@@@@@-?@@-@@
					return false;
				} // @??-@@@@@@?@@-@@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @??-@@@@@@-@@-@@
					return false;
				} // @??-@@@@@@@@@-@@
				return true;
			} // @??@@@@??????-@@
            if( !(p[ pixel[ 6 ] ] > barrier ) ) { // @?-@@@@??????-@@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @?-@@@@????-?-@@
					return false;
				} // @?-@@@@????@?-@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @?-@@@@-???@?-@@
					return false;
				} // @?-@@@@@???@?-@@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // @?-@@@@@-??@?-@@
					return false;
				} // @?-@@@@@@??@?-@@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @?-@@@@@@-?@?-@@
					return false;
				} // @?-@@@@@@@?@?-@@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @?-@@@@@@@-@?-@@
					return false;
				} // @?-@@@@@@@@@?-@@
				return true;
			} // @?@@@@@??????-@@
            if( !(p[ pixel[ 7 ] ] > barrier ) ) { // @-@@@@@??????-@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @-@@@@@-?????-@@
					return false;
				} // @-@@@@@@?????-@@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // @-@@@@@@-????-@@
					return false;
				} // @-@@@@@@@????-@@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @-@@@@@@@-???-@@
					return false;
				} // @-@@@@@@@@???-@@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @-@@@@@@@@-??-@@
					return false;
				} // @-@@@@@@@@@??-@@
				return true;
			} // @@@@@@@??????-@@
			return true;
		} // @????????????@@@
        if( !(p[ pixel[ 7 ] ] > barrier ) ) { // @-???????????@@@
            if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @-????????-??@@@
				return false;
			} // @-????????@??@@@
            if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @-???????-@??@@@
				return false;
			} // @-???????@@??@@@
            if( !(p[ pixel[ 0 ]] > barrier ) ) { // @-??????-@@??@@@
				return false;
			} // @-??????@@@??@@@
            if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @-??????@@@-?@@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @-?????-@@@-?@@@
					return false;
				} // @-?????@@@@-?@@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // @-????-@@@@-?@@@
					return false;
				} // @-????@@@@@-?@@@
                if( !(p[ pixel[ 6 ] ] > barrier ) ) { // @--???@@@@@-?@@@
					return false;
				} // @-@???@@@@@-?@@@
                if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @-@-??@@@@@-?@@@
					return false;
				} // @-@@??@@@@@-?@@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // @-@@-?@@@@@-?@@@
					return false;
				} // @-@@@?@@@@@-?@@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // @-@@@-@@@@@-?@@@
					return false;
				} // @-@@@@@@@@@-?@@@
				return true;
			} // @-??????@@@@?@@@
            if( !(p[ pixel[ 12 ] ] > barrier ) ) { // @-??????@@@@-@@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @-?????-@@@@-@@@
					return false;
				} // @-?????@@@@@-@@@
                if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @-?-???@@@@@-@@@
					return false;
				} // @-?@???@@@@@-@@@
                if( !(p[ pixel[ 4 ] ] > barrier ) ) { // @-?@-??@@@@@-@@@
					return false;
				} // @-?@@??@@@@@-@@@
                if( !(p[ pixel[ 3 ] ] > barrier ) ) { // @-?@@-?@@@@@-@@@
					return false;
				} // @-?@@@?@@@@@-@@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // @-?@@@-@@@@@-@@@
					return false;
				} // @-?@@@@@@@@@-@@@
				return true;
			} // @-??????@@@@@@@@
			return true;
		} // @@???????????@@@
        if( !(p[ pixel[ 12 ] ] > barrier ) ) { // @@??????????-@@@
            if( !(p[ pixel[ 3 ] ] > barrier ) ) { // @@???-??????-@@@
				return false;
			} // @@???@??????-@@@
            if( !(p[ pixel[ 4 ] ] > barrier ) ) { // @@??-@??????-@@@
				return false;
			} // @@??@@??????-@@@
            if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @@?-@@??????-@@@
				return false;
			} // @@?@@@??????-@@@
            if( !(p[ pixel[ 6 ] ] > barrier ) ) { // @@-@@@??????-@@@
                if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @@-@@@?????--@@@
					return false;
				} // @@-@@@?????@-@@@
                if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @@-@@@????-@-@@@
					return false;
				} // @@-@@@????@@-@@@
                if( !(p[ pixel[ 2 ] ] > barrier ) ) { // @@-@@@-???@@-@@@
					return false;
				} // @@-@@@@???@@-@@@
                if( !(p[ pixel[ 1 ]] > barrier ) ) { // @@-@@@@-??@@-@@@
					return false;
				} // @@-@@@@@??@@-@@@
                if( !(p[ pixel[ 0 ]] > barrier ) ) { // @@-@@@@@-?@@-@@@
					return false;
				} // @@-@@@@@@?@@-@@@
                if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @@-@@@@@@-@@-@@@
					return false;
				} // @@-@@@@@@@@@-@@@
				return true;
			} // @@@@@@??????-@@@
			return true;
		} // @@??????????@@@@
        if( !(p[ pixel[ 6 ] ] > barrier ) ) { // @@-?????????@@@@
            if( !(p[ pixel[ 15 ] ] > barrier ) ) { // @@-??????-??@@@@
				return false;
			} // @@-??????@??@@@@
            if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @@-??????@-?@@@@
				return false;
			} // @@-??????@@?@@@@
            if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @@-??????@@-@@@@
				return false;
			} // @@-??????@@@@@@@
			return true;
		} // @@@?????????@@@@
        if( !(p[ pixel[ 11 ] ] > barrier ) ) { // @@@????????-@@@@
            if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @@@-???????-@@@@
				return false;
			} // @@@@???????-@@@@
            if( !(p[ pixel[ 4 ] ] > barrier ) ) { // @@@@-??????-@@@@
				return false;
			} // @@@@@??????-@@@@
			return true;
		} // @@@????????@@@@@
        if( !(p[ pixel[ 5 ] ] > barrier ) ) { // @@@-???????@@@@@
            if( !(p[ pixel[ 14 ] ] > barrier ) ) { // @@@-??????-@@@@@
				return false;
			} // @@@-??????@@@@@@
			return true;
		} // @@@@???????@@@@@
		return true;
	}
	
    void FAST::detect9( const uint8_t* im, size_t stride, size_t width, size_t height, std::vector<Feature2D> & features )    
    {
		make_offsets( stride );
        size_t h = height - 3;
        size_t w = width - 3;

        im += ( 3 * stride + 3 );

        features.reserve( 1024 );
        int upperBound;
        int lowerBound;        
        for( size_t y = 3; y < h; y++ ){
            const uint8_t * curr = im;
            
            for( size_t x = 3; x < w; x++ ){
                lowerBound = *curr - _threshold;
                upperBound = *curr + _threshold;
                
                if( lowerBound && isDarkerCorner9( curr, lowerBound ) ){
                    features.push_back( Feature2D( x, y ) );
                } else {
                    if( upperBound < 255 && isBrighterCorner9( curr, upperBound ) ){
                        features.push_back( Feature2D( x, y ) );
                    }
                }
                curr++;
            }
            im += stride;
        }
        
    }
    
	/* score a single pixel */
	int FAST::score9( const uint8_t* p ){
		int bmin = _threshold;
		int bmax = 255;
		int b = (bmax + bmin)/2;

		/*Compute the score using binary search*/
		for(;;)
		{
			int cb = *p + b;
			int c_b= *p - b;


			if( p[pixel[0]] > cb)
				if( p[pixel[1]] > cb)
					if( p[pixel[2]] > cb)
						if( p[pixel[3]] > cb)
							if( p[pixel[4]] > cb)
								if( p[pixel[5]] > cb)
									if( p[pixel[6]] > cb)
										if( p[pixel[7]] > cb)
											if( p[pixel[8]] > cb)
												goto is_a_corner;
											else
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
										else if( p[pixel[7]] < c_b)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else if( p[pixel[14]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[13]] < c_b)
																		if( p[pixel[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else if( p[pixel[6]] < c_b)
										if( p[pixel[15]] > cb)
											if( p[pixel[13]] > cb)
												if( p[pixel[14]] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else if( p[pixel[13]] < c_b)
												if( p[pixel[7]] < c_b)
													if( p[pixel[8]] < c_b)
														if( p[pixel[9]] < c_b)
															if( p[pixel[10]] < c_b)
																if( p[pixel[11]] < c_b)
																	if( p[pixel[12]] < c_b)
																		if( p[pixel[14]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											if( p[pixel[7]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[13]] < c_b)
																		if( p[pixel[14]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else if( p[pixel[13]] < c_b)
											if( p[pixel[7]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[14]] < c_b)
																		if( p[pixel[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else if( p[pixel[5]] < c_b)
									if( p[pixel[14]] > cb)
										if( p[pixel[12]] > cb)
											if( p[pixel[13]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		if( p[pixel[11]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else if( p[pixel[12]] < c_b)
											if( p[pixel[6]] < c_b)
												if( p[pixel[7]] < c_b)
													if( p[pixel[8]] < c_b)
														if( p[pixel[9]] < c_b)
															if( p[pixel[10]] < c_b)
																if( p[pixel[11]] < c_b)
																	if( p[pixel[13]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[14]] < c_b)
										if( p[pixel[7]] < c_b)
											if( p[pixel[8]] < c_b)
												if( p[pixel[9]] < c_b)
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															if( p[pixel[12]] < c_b)
																if( p[pixel[13]] < c_b)
																	if( p[pixel[6]] < c_b)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										if( p[pixel[6]] < c_b)
											if( p[pixel[7]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	if( p[pixel[13]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		if( p[pixel[11]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[12]] < c_b)
										if( p[pixel[7]] < c_b)
											if( p[pixel[8]] < c_b)
												if( p[pixel[9]] < c_b)
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	if( p[pixel[6]] < c_b)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else if( p[pixel[4]] < c_b)
								if( p[pixel[13]] > cb)
									if( p[pixel[11]] > cb)
										if( p[pixel[12]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[11]] < c_b)
										if( p[pixel[5]] < c_b)
											if( p[pixel[6]] < c_b)
												if( p[pixel[7]] < c_b)
													if( p[pixel[8]] < c_b)
														if( p[pixel[9]] < c_b)
															if( p[pixel[10]] < c_b)
																if( p[pixel[12]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[pixel[13]] < c_b)
									if( p[pixel[7]] < c_b)
										if( p[pixel[8]] < c_b)
											if( p[pixel[9]] < c_b)
												if( p[pixel[10]] < c_b)
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															if( p[pixel[6]] < c_b)
																if( p[pixel[5]] < c_b)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] < c_b)
																	if( p[pixel[15]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									if( p[pixel[5]] < c_b)
										if( p[pixel[6]] < c_b)
											if( p[pixel[7]] < c_b)
												if( p[pixel[8]] < c_b)
													if( p[pixel[9]] < c_b)
														if( p[pixel[10]] < c_b)
															if( p[pixel[11]] < c_b)
																if( p[pixel[12]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	if( p[pixel[10]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[pixel[11]] < c_b)
									if( p[pixel[7]] < c_b)
										if( p[pixel[8]] < c_b)
											if( p[pixel[9]] < c_b)
												if( p[pixel[10]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[6]] < c_b)
																if( p[pixel[5]] < c_b)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] < c_b)
																	if( p[pixel[15]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else if( p[pixel[3]] < c_b)
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[pixel[10]] < c_b)
								if( p[pixel[7]] < c_b)
									if( p[pixel[8]] < c_b)
										if( p[pixel[9]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[6]] < c_b)
													if( p[pixel[5]] < c_b)
														if( p[pixel[4]] < c_b)
															goto is_a_corner;
														else
															if( p[pixel[12]] < c_b)
																if( p[pixel[13]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[12]] < c_b)
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
																if( p[pixel[15]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																if( p[pixel[9]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[pixel[10]] < c_b)
								if( p[pixel[7]] < c_b)
									if( p[pixel[8]] < c_b)
										if( p[pixel[9]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[5]] < c_b)
															if( p[pixel[4]] < c_b)
																goto is_a_corner;
															else
																if( p[pixel[13]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] < c_b)
																if( p[pixel[14]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
																if( p[pixel[15]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else if( p[pixel[2]] < c_b)
						if( p[pixel[9]] > cb)
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[9]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[10]] < c_b)
										if( p[pixel[6]] < c_b)
											if( p[pixel[5]] < c_b)
												if( p[pixel[4]] < c_b)
													if( p[pixel[3]] < c_b)
														goto is_a_corner;
													else
														if( p[pixel[11]] < c_b)
															if( p[pixel[12]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															if( p[pixel[13]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						if( p[pixel[9]] > cb)
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															if( p[pixel[8]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[9]] < c_b)
							if( p[pixel[7]] < c_b)
								if( p[pixel[8]] < c_b)
									if( p[pixel[10]] < c_b)
										if( p[pixel[11]] < c_b)
											if( p[pixel[6]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[4]] < c_b)
														if( p[pixel[3]] < c_b)
															goto is_a_corner;
														else
															if( p[pixel[12]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[12]] < c_b)
															if( p[pixel[13]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															if( p[pixel[14]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															if( p[pixel[15]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else if( p[pixel[1]] < c_b)
					if( p[pixel[8]] > cb)
						if( p[pixel[9]] > cb)
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[2]] > cb)
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[8]] < c_b)
						if( p[pixel[7]] < c_b)
							if( p[pixel[9]] < c_b)
								if( p[pixel[6]] < c_b)
									if( p[pixel[5]] < c_b)
										if( p[pixel[4]] < c_b)
											if( p[pixel[3]] < c_b)
												if( p[pixel[2]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[10]] < c_b)
														if( p[pixel[11]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[10]] < c_b)
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[10]] < c_b)
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[10]] < c_b)
										if( p[pixel[11]] < c_b)
											if( p[pixel[12]] < c_b)
												if( p[pixel[13]] < c_b)
													if( p[pixel[14]] < c_b)
														if( p[pixel[15]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					if( p[pixel[8]] > cb)
						if( p[pixel[9]] > cb)
							if( p[pixel[10]] > cb)
								if( p[pixel[11]] > cb)
									if( p[pixel[12]] > cb)
										if( p[pixel[13]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[15]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[2]] > cb)
										if( p[pixel[3]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[7]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[8]] < c_b)
						if( p[pixel[7]] < c_b)
							if( p[pixel[9]] < c_b)
								if( p[pixel[10]] < c_b)
									if( p[pixel[6]] < c_b)
										if( p[pixel[5]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[3]] < c_b)
													if( p[pixel[2]] < c_b)
														goto is_a_corner;
													else
														if( p[pixel[11]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[11]] < c_b)
														if( p[pixel[12]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														if( p[pixel[13]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														if( p[pixel[14]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[11]] < c_b)
											if( p[pixel[12]] < c_b)
												if( p[pixel[13]] < c_b)
													if( p[pixel[14]] < c_b)
														if( p[pixel[15]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else if( p[pixel[0]] < c_b)
				if( p[pixel[1]] > cb)
					if( p[pixel[8]] > cb)
						if( p[pixel[7]] > cb)
							if( p[pixel[9]] > cb)
								if( p[pixel[6]] > cb)
									if( p[pixel[5]] > cb)
										if( p[pixel[4]] > cb)
											if( p[pixel[3]] > cb)
												if( p[pixel[2]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[10]] > cb)
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[10]] > cb)
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[10]] > cb)
										if( p[pixel[11]] > cb)
											if( p[pixel[12]] > cb)
												if( p[pixel[13]] > cb)
													if( p[pixel[14]] > cb)
														if( p[pixel[15]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[8]] < c_b)
						if( p[pixel[9]] < c_b)
							if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[2]] < c_b)
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[pixel[1]] < c_b)
					if( p[pixel[2]] > cb)
						if( p[pixel[9]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[10]] > cb)
										if( p[pixel[6]] > cb)
											if( p[pixel[5]] > cb)
												if( p[pixel[4]] > cb)
													if( p[pixel[3]] > cb)
														goto is_a_corner;
													else
														if( p[pixel[11]] > cb)
															if( p[pixel[12]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															if( p[pixel[13]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[9]] < c_b)
							if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[2]] < c_b)
						if( p[pixel[3]] > cb)
							if( p[pixel[10]] > cb)
								if( p[pixel[7]] > cb)
									if( p[pixel[8]] > cb)
										if( p[pixel[9]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[6]] > cb)
													if( p[pixel[5]] > cb)
														if( p[pixel[4]] > cb)
															goto is_a_corner;
														else
															if( p[pixel[12]] > cb)
																if( p[pixel[13]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[12]] > cb)
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
																if( p[pixel[15]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[3]] < c_b)
							if( p[pixel[4]] > cb)
								if( p[pixel[13]] > cb)
									if( p[pixel[7]] > cb)
										if( p[pixel[8]] > cb)
											if( p[pixel[9]] > cb)
												if( p[pixel[10]] > cb)
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															if( p[pixel[6]] > cb)
																if( p[pixel[5]] > cb)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] > cb)
																	if( p[pixel[15]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[pixel[13]] < c_b)
									if( p[pixel[11]] > cb)
										if( p[pixel[5]] > cb)
											if( p[pixel[6]] > cb)
												if( p[pixel[7]] > cb)
													if( p[pixel[8]] > cb)
														if( p[pixel[9]] > cb)
															if( p[pixel[10]] > cb)
																if( p[pixel[12]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[11]] < c_b)
										if( p[pixel[12]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									if( p[pixel[5]] > cb)
										if( p[pixel[6]] > cb)
											if( p[pixel[7]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else if( p[pixel[4]] < c_b)
								if( p[pixel[5]] > cb)
									if( p[pixel[14]] > cb)
										if( p[pixel[7]] > cb)
											if( p[pixel[8]] > cb)
												if( p[pixel[9]] > cb)
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															if( p[pixel[12]] > cb)
																if( p[pixel[13]] > cb)
																	if( p[pixel[6]] > cb)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[14]] < c_b)
										if( p[pixel[12]] > cb)
											if( p[pixel[6]] > cb)
												if( p[pixel[7]] > cb)
													if( p[pixel[8]] > cb)
														if( p[pixel[9]] > cb)
															if( p[pixel[10]] > cb)
																if( p[pixel[11]] > cb)
																	if( p[pixel[13]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else if( p[pixel[12]] < c_b)
											if( p[pixel[13]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		if( p[pixel[11]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										if( p[pixel[6]] > cb)
											if( p[pixel[7]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[13]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else if( p[pixel[5]] < c_b)
									if( p[pixel[6]] > cb)
										if( p[pixel[15]] < c_b)
											if( p[pixel[13]] > cb)
												if( p[pixel[7]] > cb)
													if( p[pixel[8]] > cb)
														if( p[pixel[9]] > cb)
															if( p[pixel[10]] > cb)
																if( p[pixel[11]] > cb)
																	if( p[pixel[12]] > cb)
																		if( p[pixel[14]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else if( p[pixel[13]] < c_b)
												if( p[pixel[14]] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											if( p[pixel[7]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[13]] > cb)
																		if( p[pixel[14]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else if( p[pixel[6]] < c_b)
										if( p[pixel[7]] > cb)
											if( p[pixel[14]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[13]] > cb)
																		if( p[pixel[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else if( p[pixel[7]] < c_b)
											if( p[pixel[8]] < c_b)
												goto is_a_corner;
											else
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[13]] > cb)
											if( p[pixel[7]] > cb)
												if( p[pixel[8]] > cb)
													if( p[pixel[9]] > cb)
														if( p[pixel[10]] > cb)
															if( p[pixel[11]] > cb)
																if( p[pixel[12]] > cb)
																	if( p[pixel[14]] > cb)
																		if( p[pixel[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[12]] > cb)
										if( p[pixel[7]] > cb)
											if( p[pixel[8]] > cb)
												if( p[pixel[9]] > cb)
													if( p[pixel[10]] > cb)
														if( p[pixel[11]] > cb)
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	if( p[pixel[6]] > cb)
																		goto is_a_corner;
																	else
																		if( p[pixel[15]] > cb)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		if( p[pixel[11]] < c_b)
																			goto is_a_corner;
																		else
																			goto is_not_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								if( p[pixel[11]] > cb)
									if( p[pixel[7]] > cb)
										if( p[pixel[8]] > cb)
											if( p[pixel[9]] > cb)
												if( p[pixel[10]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[6]] > cb)
																if( p[pixel[5]] > cb)
																	goto is_a_corner;
																else
																	if( p[pixel[14]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
															else
																if( p[pixel[14]] > cb)
																	if( p[pixel[15]] > cb)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	if( p[pixel[10]] < c_b)
																		goto is_a_corner;
																	else
																		goto is_not_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							if( p[pixel[10]] > cb)
								if( p[pixel[7]] > cb)
									if( p[pixel[8]] > cb)
										if( p[pixel[9]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[6]] > cb)
														if( p[pixel[5]] > cb)
															if( p[pixel[4]] > cb)
																goto is_a_corner;
															else
																if( p[pixel[13]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
														else
															if( p[pixel[13]] > cb)
																if( p[pixel[14]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
																if( p[pixel[15]] > cb)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																if( p[pixel[9]] < c_b)
																	goto is_a_corner;
																else
																	goto is_not_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
					else
						if( p[pixel[9]] > cb)
							if( p[pixel[7]] > cb)
								if( p[pixel[8]] > cb)
									if( p[pixel[10]] > cb)
										if( p[pixel[11]] > cb)
											if( p[pixel[6]] > cb)
												if( p[pixel[5]] > cb)
													if( p[pixel[4]] > cb)
														if( p[pixel[3]] > cb)
															goto is_a_corner;
														else
															if( p[pixel[12]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
													else
														if( p[pixel[12]] > cb)
															if( p[pixel[13]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															if( p[pixel[14]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															if( p[pixel[15]] > cb)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else if( p[pixel[9]] < c_b)
							if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															if( p[pixel[8]] < c_b)
																goto is_a_corner;
															else
																goto is_not_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
				else
					if( p[pixel[8]] > cb)
						if( p[pixel[7]] > cb)
							if( p[pixel[9]] > cb)
								if( p[pixel[10]] > cb)
									if( p[pixel[6]] > cb)
										if( p[pixel[5]] > cb)
											if( p[pixel[4]] > cb)
												if( p[pixel[3]] > cb)
													if( p[pixel[2]] > cb)
														goto is_a_corner;
													else
														if( p[pixel[11]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
												else
													if( p[pixel[11]] > cb)
														if( p[pixel[12]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														if( p[pixel[13]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														if( p[pixel[14]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[11]] > cb)
											if( p[pixel[12]] > cb)
												if( p[pixel[13]] > cb)
													if( p[pixel[14]] > cb)
														if( p[pixel[15]] > cb)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else if( p[pixel[8]] < c_b)
						if( p[pixel[9]] < c_b)
							if( p[pixel[10]] < c_b)
								if( p[pixel[11]] < c_b)
									if( p[pixel[12]] < c_b)
										if( p[pixel[13]] < c_b)
											if( p[pixel[14]] < c_b)
												if( p[pixel[15]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[2]] < c_b)
										if( p[pixel[3]] < c_b)
											if( p[pixel[4]] < c_b)
												if( p[pixel[5]] < c_b)
													if( p[pixel[6]] < c_b)
														if( p[pixel[7]] < c_b)
															goto is_a_corner;
														else
															goto is_not_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
			else
				if( p[pixel[7]] > cb)
					if( p[pixel[8]] > cb)
						if( p[pixel[9]] > cb)
							if( p[pixel[6]] > cb)
								if( p[pixel[5]] > cb)
									if( p[pixel[4]] > cb)
										if( p[pixel[3]] > cb)
											if( p[pixel[2]] > cb)
												if( p[pixel[1]] > cb)
													goto is_a_corner;
												else
													if( p[pixel[10]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[10]] > cb)
													if( p[pixel[11]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[10]] > cb)
												if( p[pixel[11]] > cb)
													if( p[pixel[12]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[10]] > cb)
											if( p[pixel[11]] > cb)
												if( p[pixel[12]] > cb)
													if( p[pixel[13]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[10]] > cb)
										if( p[pixel[11]] > cb)
											if( p[pixel[12]] > cb)
												if( p[pixel[13]] > cb)
													if( p[pixel[14]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								if( p[pixel[10]] > cb)
									if( p[pixel[11]] > cb)
										if( p[pixel[12]] > cb)
											if( p[pixel[13]] > cb)
												if( p[pixel[14]] > cb)
													if( p[pixel[15]] > cb)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else if( p[pixel[7]] < c_b)
					if( p[pixel[8]] < c_b)
						if( p[pixel[9]] < c_b)
							if( p[pixel[6]] < c_b)
								if( p[pixel[5]] < c_b)
									if( p[pixel[4]] < c_b)
										if( p[pixel[3]] < c_b)
											if( p[pixel[2]] < c_b)
												if( p[pixel[1]] < c_b)
													goto is_a_corner;
												else
													if( p[pixel[10]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
											else
												if( p[pixel[10]] < c_b)
													if( p[pixel[11]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
										else
											if( p[pixel[10]] < c_b)
												if( p[pixel[11]] < c_b)
													if( p[pixel[12]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
									else
										if( p[pixel[10]] < c_b)
											if( p[pixel[11]] < c_b)
												if( p[pixel[12]] < c_b)
													if( p[pixel[13]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
								else
									if( p[pixel[10]] < c_b)
										if( p[pixel[11]] < c_b)
											if( p[pixel[12]] < c_b)
												if( p[pixel[13]] < c_b)
													if( p[pixel[14]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
							else
								if( p[pixel[10]] < c_b)
									if( p[pixel[11]] < c_b)
										if( p[pixel[12]] < c_b)
											if( p[pixel[13]] < c_b)
												if( p[pixel[14]] < c_b)
													if( p[pixel[15]] < c_b)
														goto is_a_corner;
													else
														goto is_not_a_corner;
												else
													goto is_not_a_corner;
											else
												goto is_not_a_corner;
										else
											goto is_not_a_corner;
									else
										goto is_not_a_corner;
								else
									goto is_not_a_corner;
						else
							goto is_not_a_corner;
					else
						goto is_not_a_corner;
				else
					goto is_not_a_corner;

is_a_corner:
			bmin=b;
			goto end_if;

is_not_a_corner:
			bmax=b;
			goto end_if;

end_if:

			if(bmin == bmax - 1 || bmin == bmax)
				return bmin;
			b = (bmin + bmax) / 2;
		}
	}
}
