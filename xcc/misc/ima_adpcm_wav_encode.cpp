// ima_adpcm_wav_encode.cpp: implementation of the Cima_adpcm_wav_encode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "aud_decode.h"
#include "ima_adpcm_wav_encode.h"
#include "minmax.h"
#include "riff_structures.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cima_adpcm_wav_encode::Cima_adpcm_wav_encode()
{
	m_data = NULL;
}

Cima_adpcm_wav_encode::~Cima_adpcm_wav_encode()
{
	delete[] m_data;
}

void Cima_adpcm_wav_encode::load(const short* s, int cb_s, int c_channels)
{
	const int chunk_size = 512;
	mc_samples = (cb_s / c_channels >> 1);
	int c_chunks = (mc_samples + 1016) / 1017;
	delete[] m_data;
	mcb_data = sizeof(t_ima_adpcm_chunk_header) * c_channels * c_chunks + (c_channels * (mc_samples - c_chunks + 1) >> 1);
	int align_mask = (c_channels << 3) - 1;
	mcb_data = mcb_data + align_mask & ~align_mask;
	m_data = new byte[mcb_data];
	const short* r = s;
	byte* w = m_data;
	int cs_remaining = mc_samples;
	aud_decode left_d, right_d;
	left_d.init();
	right_d.init();
	while (cs_remaining)
	{
		if (c_channels == 1)
		{
			t_ima_adpcm_chunk_header& chunk_header = *reinterpret_cast<t_ima_adpcm_chunk_header*>(w);
			w += sizeof(t_ima_adpcm_chunk_header);
			chunk_header.sample = *r++;
			chunk_header.index = left_d.index();
			chunk_header.reserved = 0;
			cs_remaining--;
			int cs_chunk = min(cs_remaining, chunk_size - sizeof(t_ima_adpcm_chunk_header) << 1);
			left_d.init(chunk_header.index, chunk_header.sample);
			left_d.encode_chunk(r, w, cs_chunk);
			r += cs_chunk;
			w += cs_chunk >> 1;
			cs_remaining -= cs_chunk;
		}
		else
		{
			assert(c_channels == 2);
			t_ima_adpcm_chunk_header& left_chunk_header = *reinterpret_cast<t_ima_adpcm_chunk_header*>(w);
			w += sizeof(t_ima_adpcm_chunk_header);
			left_chunk_header.sample = *r++;
			left_chunk_header.index = left_d.index();
			left_chunk_header.reserved = 0;
			t_ima_adpcm_chunk_header& right_chunk_header = *reinterpret_cast<t_ima_adpcm_chunk_header*>(w);
			w += sizeof(t_ima_adpcm_chunk_header);
			right_chunk_header.sample = *r++;
			right_chunk_header.index = right_d.index();
			right_chunk_header.reserved = 0;
			cs_remaining--;
			int cs_chunk = min(cs_remaining, chunk_size - sizeof(t_ima_adpcm_chunk_header) << 1);
			aud_decode left_d, right_d;
			left_d.init(left_chunk_header.index, left_chunk_header.sample);
			right_d.init(right_chunk_header.index, right_chunk_header.sample);
			while (cs_chunk)
			{
				int cs_v = min(cs_chunk, 8);
				short left_t[8], right_t[8];
				int i = 0;
				for (; i < cs_v; i++)
				{
					left_t[i] = *r++;
					right_t[i] = *r++;
				}
				for (; i < 8; i++)
				{
					left_t[i] = right_t[i] = 0;
				}
				left_d.encode_chunk(left_t, w, 8);
				w += 4;
				right_d.encode_chunk(right_t, w, 8);
				w += 4;
				cs_chunk -= cs_v;
				cs_remaining -= cs_v;
			}
		}
	}
}
