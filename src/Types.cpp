#include "Types.hpp"

using namespace AGGWrap;

AGGWrap::Exception::~Exception(void) {}

const char* AGGWrap::OutOfRangeException::GetMessage(void) const {
	return "Array index out of range";
}

AGGWrap::OutOfRangeException::~OutOfRangeException(void) {}
