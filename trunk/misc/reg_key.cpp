#include "stdafx.h"
#include "reg_key.h"

#include <vector>

using namespace std;

Creg_key::Creg_key()
{
	m_h = NULL;
}

Creg_key::Creg_key(HKEY key, const string& name, REGSAM sam_desired)
{
	if (open(key, name, sam_desired) != ERROR_SUCCESS)
		throw exception();
}

Creg_key::Creg_key(const Creg_key& key, const string& name, REGSAM sam_desired)
{
	if (open(key.m_h, name, sam_desired) != ERROR_SUCCESS)
		throw exception();
}

Creg_key::~Creg_key()
{
	close();
}

LONG Creg_key::create(HKEY key, const string& name)
{
	close();
	return RegCreateKeyEx(key, name.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_h, NULL);
}


LONG Creg_key::open(HKEY key, const string& name, REGSAM sam_desired)
{
	close();
	return RegOpenKeyEx(key, name.c_str(), 0, sam_desired, &m_h);
};

LONG Creg_key::open(const Creg_key& key, const string& name, REGSAM sam_desired)
{
	return open(key.m_h, name, sam_desired);
};

LONG Creg_key::close()
{
	if (!m_h)
		return ERROR_SUCCESS;
	LONG result = RegCloseKey(m_h);
	m_h = NULL;
	return result;
}

LONG Creg_key::query_value(const string& name, string& value)
{
	DWORD cb_d = 0;
	LONG result = RegQueryValueEx(m_h, name.c_str(), NULL, NULL, NULL, &cb_d);
	if (result != ERROR_SUCCESS)
		return result;
	if (!cb_d)
	{
		value.erase();
		return result;
	}
	vector<BYTE> d(cb_d);
	result = RegQueryValueEx(m_h, name.c_str(), NULL, NULL, &d.front(), &cb_d);
	if (result == ERROR_SUCCESS)
	{
		if (cb_d)
			value.assign(reinterpret_cast<char*>(&d.front()), cb_d - 1);
		else
			value.erase();
	}
	return result;
}

string Creg_key::query_value(const string& name)
{
	string d;
	if (query_value(name, d) != ERROR_SUCCESS)
		throw exception();
	return d;
}

LONG Creg_key::set_value(const string& name, const string& value)
{
	return RegSetValueEx(m_h, name.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.c_str()), value.size());
}

