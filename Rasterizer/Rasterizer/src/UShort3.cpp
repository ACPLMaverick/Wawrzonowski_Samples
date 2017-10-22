#include "UShort3.h"


namespace math
{
	UShort3::UShort3() :
		p(0),
		t(0),
		n(0)
	{
	}

	UShort3::UShort3(uint16_t np, uint16_t nt, uint16_t nn) :
		p(np),
		t(nt),
		n(nn)
	{
	}


	UShort3::~UShort3()
	{
	}

}