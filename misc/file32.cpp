#include "stdafx.h"
#include "file32.h"

#ifdef _MSC_VER
int Cfile32::open(const Cwin_handle& h)
{
	m_h = h;
	m_p = 0;
	return !is_open();
}

int Cfile32::open(const string& name, int access)
{
	return access & GENERIC_WRITE ? open(name, access, CREATE_ALWAYS, 0) : open(name, access, OPEN_EXISTING, FILE_SHARE_READ);
}

int Cfile32::open(const string& name, int access, int creation, int share)
{
    close();
	m_h = CreateFile(name.c_str(), access, share, NULL, creation, FILE_ATTRIBUTE_NORMAL, NULL);
	m_p = 0;
    return !is_open();
}

FILETIME Cfile32::get_creation_time() const
{
	assert(is_open());
	FILETIME time;
	int r = GetFileTime(h(), &time, NULL, NULL);
	assert(r);
	return time;
}

FILETIME Cfile32::get_last_access_time() const
{
    assert(is_open());
	FILETIME time;
	int r = GetFileTime(h(), NULL, &time,  NULL);
	assert(r);
	return time;
}

FILETIME Cfile32::get_last_write_time() const
{
    assert(is_open());
	FILETIME time;
	int r = GetFileTime(h(), NULL, NULL, &time);
	assert(r);
	return time;
}
#endif

int Cfile32::open_read(const string& name)
{
#ifdef _MSC_VER
	return open(name, GENERIC_READ, OPEN_EXISTING, FILE_SHARE_READ);
#else
	assert(!m_f.is_open());
	m_f.open(name.c_str(), ios::binary | ios::in);
	return m_f.fail();
#endif
}

int Cfile32::open_edit(const string& name)
{
#ifdef _MSC_VER
	return open(name, GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS, 0);
#else
	m_f.open(name.c_str(), ios::binary | ios::in | ios::out);
	return m_f.fail();
#endif
}

int Cfile32::open_write(const string& name)
{
#ifdef _MSC_VER
	return open(name, GENERIC_WRITE, CREATE_ALWAYS, 0);
#else
	m_f.open(name.c_str(), ios::binary | ios::out | ios::trunc);
	return m_f.fail();
#endif
}

int Cfile32::get_size() const
{
    assert(is_open());
#ifdef _MSC_VER
    int res = GetFileSize(h(), NULL);
    assert(res != -1);
    return res;
#else
	fstream f = m_f;
	int pos = f.tellp();
	f.seekp(0, ios::end);
	int size = f.tellp();
	f.seekp(pos);
	return size;
#endif
}

int Cfile32::read(void* data, int size)
{
    assert(is_open());
#ifdef _MSC_VER
    if (SetFilePointer(h(), m_p, 0, FILE_BEGIN) == -1)
        return 1;
    DWORD cb_read;
	if (!ReadFile(h(), data, size, &cb_read, 0) || cb_read != size)
		return 1;
    m_p += size;
    return 0;
#else
	m_f.read(reinterpret_cast<char*>(data), size);
	return m_f.fail();
#endif
}

int Cfile32::write(const void* data, int size)
{
    assert(is_open());
#ifdef _MSC_VER
    if (SetFilePointer(h(), m_p, 0, FILE_BEGIN) == -1)
        return 1;
    DWORD cb_write;
	if (!WriteFile(h(), data, size, &cb_write, 0) || cb_write != size)
		return 1;
    m_p += size;
    return 0;
#else
	m_f.write(reinterpret_cast<const char*>(data), size);
	return m_f.fail();
#endif
}

int Cfile32::write(int v)
{
	return write(&v, sizeof(int));
};

int Cfile32::set_eof()
{
    assert(is_open());
#ifdef _MSC_VER
    if (SetFilePointer(h(), m_p, 0, FILE_BEGIN) == -1)
        return 1;
	return !SetEndOfFile(h());
#else
	return write(NULL, 0);
#endif
}

void Cfile32::close()
{
#ifdef _MSC_VER
	m_h.clear();
#else
	m_f.close();
#endif
}

Cvirtual_binary Cfile32::get_mm()
{
	int size = get_size();
	if (!size)
		return Cvirtual_binary();
	Cmemory_map mm(*this);
	return mm.d() ? Cvirtual_binary(mm.d(), size, Csmart_ref<Cmemory_map>::create(mm)) : Cvirtual_binary();
}

Cvirtual_binary file32_read(const string& name)
{
	Cfile32 f;
	return f.open_read(name) ? Cvirtual_binary() : f.get_mm();
}

int file32_write(const string& name, const void* s, int cb_s)
{
	Cfile32 f;
	if (int error = f.open_write(name))
		return error;
	return f.write(s, cb_s);
}

Cmemory_map_source::Cmemory_map_source(const Cfile32& f)
{
	m_fh = f.h();
	m_mh = CreateFileMapping(f.h(), NULL, PAGE_READONLY, 0, 0, NULL);
	m_d = m_mh ? static_cast<byte*>(MapViewOfFile(m_mh, FILE_MAP_READ, 0, 0, 0)) : NULL;
	mc_references = 1;
}

Cmemory_map_source* Cmemory_map_source::attach()
{
	if (this)
		mc_references++;
	return this;
}

void Cmemory_map_source::detach()
{
	if (!this || --mc_references)
		return;
	UnmapViewOfFile(m_d);
	delete this;
}

Cmemory_map::Cmemory_map(const Cmemory_map& v)
{
	m_source = v.m_source->attach();
}

Cmemory_map::Cmemory_map(const Cfile32& f)
{
	m_source = new Cmemory_map_source(f);
}

Cmemory_map::~Cmemory_map()
{
	m_source->detach();
}

const Cmemory_map& Cmemory_map::operator=(const Cmemory_map& v)
{
	if (this != &v)
	{
		m_source->detach();
		m_source = v.m_source->attach();
	}
	return *this;
}

void Cmemory_map::clear()
{
	m_source->detach();
	m_source = NULL;
}