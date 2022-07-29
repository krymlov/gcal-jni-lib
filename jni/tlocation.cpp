#include "tlocation.h"

// cities above 450000 people
// PORTABLE

TLocation TLocation::gloc[] = { };

int TLocation::GetLocationCount()
{
	return sizeof(gloc)/sizeof(TLocation);
}
