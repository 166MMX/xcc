#include "stdafx.h"
#include <virtual_binary.h>

#include "file32.h"

Cvirtual_binary_source::Cvirtual_binary_source(const void* d, size_t cb_d, Csmart_ref_base* source)
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

Cvirtual_binary_source* Cvirtual_binary_source::attach()
{
	if (this)
		mc_references++;
	return this;
}

void Cvirtual_binary_source::detach()
{
	if (!this || --mc_references)
		return;
	if (m_source)
		m_source->detach();
	else
		delete[] m_data;
	delete this;
}

Cvirtual_binary_source* Cvirtual_binary_source::pre_edit()
{
	if (mc_references == 1 && !m_source)
		return this;
	Cvirtual_binary_source* t = new Cvirtual_binary_source(m_data, m_size, NULL);
	detach();
	return t;
}	

Cvirtual_binary::Cvirtual_binary()
{
	m_source = NULL;
}

Cvirtual_binary::Cvirtual_binary(const Cvirtual_binary& v)
{
	m_source = v.m_source->attach();
}

Cvirtual_binary::Cvirtual_binary(const void* d, size_t cb_d)
{
	m_source = new Cvirtual_binary_source(d, cb_d);
}

Cvirtual_binary::Cvirtual_binary(const void* d, size_t cb_d, Csmart_ref_base* source)
{
	m_source = new Cvirtual_binary_source(d, cb_d, source);
}

Cvirtual_binary::Cvirtual_binary(const_memory_range d)
{
	m_source = new Cvirtual_binary_source(d.begin, d.size());
}

Cvirtual_binary::Cvirtual_binary(const string& fname, bool use_mm)
{
	m_source = NULL;
	load(fname, use_mm);
}

Cvirtual_binary::~Cvirtual_binary()
{
	m_source->detach();
}

const Cvirtual_binary& Cvirtual_binary::operator=(const Cvirtual_binary& v)
{
	if (this != &v)
	{
		m_source->detach();
		m_source = v.m_source->attach();
	}
	return *this;
}

int Cvirtual_binary::save(const string& fname) const
{
	return data() ? file32_write(fname, data(), size()) : 1;
}

int Cvirtual_binary::load(const string& fname, bool use_mm)
{
	if (use_mm)
		*this = file32_read(fname);
	else if (Cvirtual_binary d = file32_read(fname))
		*this = Cvirtual_binary(d.data(), d.size());
	else
		clear();
	return !data();
}

void Cvirtual_binary::clear()
{
	m_source->detach();
	m_source = NULL;
}

void Cvirtual_binary::memset(int v)
{
	::memset(data_edit(), v, size());
}

size_t Cvirtual_binary::read(void* d) const
{
	memcpy(d, data(), size());
	return size();
}

byte* Cvirtual_binary::write_start(size_t cb_d)
{
	if (data() && size() == cb_d)
		return data_edit();
	m_source->detach();
	m_source = new Cvirtual_binary_source(NULL, cb_d);
	return data_edit();
}

void Cvirtual_binary::write(const void* d, size_t cb_d)
{
	memcpy(write_start(cb_d), d, cb_d);
}

Cvirtual_binary Cvirtual_binary::sub_bin(size_t offset, size_t size) const
{
	assert(offset >= 0 && offset + size <= Cvirtual_binary::size());
	return data() ? Cvirtual_binary(data() + offset, size, Csmart_ref<Cvirtual_binary>::create(*this)) : *this;
}