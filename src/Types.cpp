#include "Types.hpp"

using namespace AGGWrap;

AGGWrap::Exception::~Exception(void) {}

const char* AGGWrap::OutOfRangeException::GetMessage(void) const {
	return "Array index out of range";
}

AGGWrap::OutOfRangeException::~OutOfRangeException(void) {}

const char* AGGWrap::NullPointerException::GetMessage(void) const {
	return "A pointer that cannot be null was null";
}

AGGWrap::NullPointerException::~NullPointerException(void) {}

const char* AGGWrap::InvalidOperationException::GetMessage(void) const {
	return "The requested operation cannot be performed while the object is in this state";
}

AGGWrap::InvalidOperationException::~InvalidOperationException(void) {}
