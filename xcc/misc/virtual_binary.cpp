// virtual_binary.cpp: implementation of the Cvirtual_binary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "virtual_binary.h"

#include "file32.h"

Cvirtual_binary_source::Cvirtual_binary_source(const void* d, int cb_d, Csmart_ref_base* source)
{
	if (source)
	{
		m_data = const_cast<byte*>(reinterpret_cast<const byte*>(d));
		m_size = cb_d;
		m_source = source->attach();
	}
	else
	{
		m_data = new byte[cb_d];
		m_size = cb_d;
		if (d)
			memcpy(m_data, d, cb_d);
		m_source = NULL;
	}
	mc_references = 1;
}

void Cvirtual_binary_source::detach()
{
	if (!--mc_references)
	{
		if (m_source)
			m_source->detach();
		else
			delete m_data;
		delete this;
	}
}

Cvirtual_binary_source* Cvirtual_binary_source::pre_edit()
{
	if (mc_references == 1 && !m_source)
		return this;
	Cvirtual_binary_source t = *this;
	detach();
	return new Cvirtual_binary_source(t.data(), t.size(), NULL);
}	

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cvirtual_binary::Cvirtual_binary()
{
	m_source = NULL;
}

Cvirtual_binary::Cvirtual_binary(const Cvirtual_binary& v)
{
	m_source = v.m_source ? v.m_source->attach() : NULL;
}

Cvirtual_binary::Cvirtual_binary(const void* d, int cb_d)
{
	m_source = new Cvirtual_binary_source(d, cb_d);
}

Cvirtual_binary::Cvirtual_binary(const void* d, int cb_d, Csmart_ref_base* source)
{
	m_source = new Cvirtual_binary_source(d, cb_d, source);
}

Cvirtual_binary::Cvirtual_binary(const string& fname, bool use_mm)
{
	m_source = NULL;
	import(fname, use_mm);
}

Cvirtual_binary::~Cvirtual_binary()
{
	if (m_source)
		m_source->detach();
}

const Cvirtual_binary& Cvirtual_binary::operator=(const Cvirtual_binary& v)
{
	if (this != &v)
	{
		if (m_source)
			m_source->detach();
		m_source = v.m_source ? v.m_source->attach() : NULL;
	}
	return *this;
}

int Cvirtual_binary::export(string fname) const
{
	return file32_write(fname, data(), size());
}

int Cvirtual_binary::import(string fname, bool use_mm)
{
	if (use_mm)
		*this = file32_read(fname);
	else
	{
		Cvirtual_binary d = file32_read(fname);
		if (d)
			*this = Cvirtual_binary(d.data(), d.size());
		else
			clear();
	}
	return !data();
}

void Cvirtual_binary::clear()
{
	if (!m_source)
		return;
	m_source->detach();
	m_source = NULL;
}

void Cvirtual_binary::memset(int v)
{
	::memset(data_edit(), v, size());
}

int Cvirtual_binary::read(void* d) const
{
	memcpy(d, data(), size());
	return size();
}

byte* Cvirtual_binary::write_start(int cb_d)
{
	if (data() && size() == cb_d)
		return data_edit();
	if (m_source)
		m_source->detach();
	m_source = new Cvirtual_binary_source(NULL, cb_d);
	return data_edit();
}

void Cvirtual_binary::write(const void* d, int cb_d)
{
	memcpy(write_start(cb_d), d, cb_d);
}

Cvirtual_binary Cvirtual_binary::sub_bin(int offset, int size) const
{
	assert(offset >= 0 && offset + size <= Cvirtual_binary::size());
	return data() ? Cvirtual_binary(data() + offset, size, Csmart_ref<Cvirtual_binary>::create(*this)) : *this;
}