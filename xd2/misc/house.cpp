#include "house.h"

Chouse::Chouse(const map<string, string>& d)
{
	name_ = find_ref(d, "name", "");
}
