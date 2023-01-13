// ZipX.cpp
// Based on ZIP Utils, by Lucian Wischik, www.wischik.com/lu

#include "stdafx.h"
#include "ZipX.h"

typedef unsigned char	uch;
typedef unsigned short	ush;
typedef unsigned long	ulg;
typedef size_t			extent;
typedef unsigned		Pos;
typedef unsigned		IPos;

#ifndef EOF
#define EOF	(-1)
#endif

#define ZE_MISS			-1
#define ZE_OK			0
#define ZE_EOF			2
#define ZE_FORM			3
#define ZE_MEM			4
#define ZE_LOGIC		5
#define ZE_BIG			6
#define ZE_NOTE			7
#define ZE_TEST			8
#define ZE_ABORT		9
#define ZE_TEMP			10
#define ZE_READ			11
#define ZE_NONE			12
#define ZE_NAME			13
#define ZE_WRITE		14
#define ZE_CREAT		15
#define ZE_PARMS		16
#define ZE_OPEN			18
#define ZE_MAXERR		18

#define UNKNOWN			(-1)
#define BINARY			0
#define ASCII			1

#define BEST			-1
#define STORE			0
#define DEFLATE			8

#define CRCVAL_INITIAL	0L

#define MSDOS_HIDDEN_ATTR	0x02
#define MSDOS_DIR_ATTR		0x10

#define LOCHEAD			26
#define CENHEAD			42
#define ENDHEAD			18

#define EB_HEADSIZE		4
#define EB_LEN			2
#define EB_UT_MINLEN	1
#define EB_UT_FLAGS		0
#define EB_UT_TIME1		1
#define EB_UT_FL_MTIME	(1 << 0)
#define EB_UT_FL_ATIME	(1 << 1)
#define EB_UT_FL_CTIME	(1 << 2)
#define EB_UT_LEN(n)	(EB_UT_MINLEN + 4 * (n))
#define EB_L_UT_SIZE	(EB_HEADSIZE + EB_UT_LEN(3))
#define EB_C_UT_SIZE	(EB_HEADSIZE + EB_UT_LEN(1))

#define PUTSH(a, f)		{ char _putsh_c	= (char)((a) & 0xff); wfunc(param, &_putsh_c, 1); _putsh_c	= (char)((a) >> 8); wfunc(param, &_putsh_c, 1); }
#define PUTLG(a, f)		{ PUTSH((a) & 0xffff, (f)) PUTSH((a) >> 16, (f))}

#define LOCSIG			0x04034b50L
#define CENSIG			0x02014b50L
#define ENDSIG			0x06054b50L
#define EXTLOCSIG		0x08074b50L

#define MIN_MATCH		3
#define MAX_MATCH		258

#define WSIZE			(0x8000)

#define MIN_LOOKAHEAD	(MAX_MATCH + MIN_MATCH + 1)
#define MAX_DIST		(WSIZE - MIN_LOOKAHEAD)

#define MAX_BITS		15
#define MAX_BL_BITS		7
#define LENGTH_CODES	29
#define LITERALS		256
#define END_BLOCK		256
#define L_CODES			(LITERALS + 1 + LENGTH_CODES)
#define D_CODES			30
#define BL_CODES		19

#define STORED_BLOCK	0
#define STATIC_TREES	1
#define DYN_TREES		2

#define LIT_BUFSIZE		0x8000
#define DIST_BUFSIZE	LIT_BUFSIZE

#define REP_3_6			16
#define REPZ_3_10		17
#define REPZ_11_138		18
#define HEAP_SIZE		(2 * L_CODES + 1)

#define Buf_size		(8 * 2 * sizeof(char))

#define PUTSHORT(state, w)															\
{																					\
	if (state.bs.out_offset >= state.bs.out_size - 1)								\
	{																				\
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);	\
	}																				\
																					\
	state.bs.out_buf[state.bs.out_offset++]	= (char)((w) & 0xff);					\
	state.bs.out_buf[state.bs.out_offset++]	= (char)((ush)(w) >> 8);				\
}

#define PUTBYTE(state, b)															\
{																					\
	if (state.bs.out_offset >= state.bs.out_size)									\
	{																				\
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);	\
	}																				\
																					\
	state.bs.out_buf[state.bs.out_offset++]	= (char)(b);							\
}

#define HASH_BITS	15
#define HASH_SIZE	(unsigned)(1 << HASH_BITS)
#define HASH_MASK	(HASH_SIZE - 1)
#define WMASK		(WSIZE - 1)

#define NIL			0

#define FAST		4
#define SLOW		2

#define TOO_FAR		4096
#define EQUAL		0

#define H_SHIFT				((HASH_BITS + MIN_MATCH - 1) / MIN_MATCH)
#define max_insert_length	max_lazy_match

const int	extra_lbits[LENGTH_CODES]	=
{
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4,
	5, 5, 5, 5, 0
};

const int	extra_dbits[D_CODES]	=
{
	 0,  0,  0,  0,  1,  1,  2,  2,
	 3,  3,  4,  4,  5,  5,  6,  6,
	 7,  7,  8,  8,  9,  9, 10, 10,
	11, 11, 12, 12, 13, 13
};

const int	extra_blbits[BL_CODES]	=
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	2, 3, 7
};

const uch	bl_order[BL_CODES]	=
{
	16, 17, 18,  0,  8,  7,  9,  6,
	10,  5, 11,  4, 12,  3, 13,  2,
	14,  1, 15
};

typedef struct config
{
	ush	good_length;
	ush	max_lazy;
	ush	nice_length;
	ush	max_chain;
} config;

const config	configuration_table[10]	=
{
	{    0,    0,    0,    0 },
	{    4,    4,    8,    4 },
	{    4,    5,   16,    8 },
	{    4,    6,   32,   32 },
	{    4,    4,   16,   16 },
	{    8,   16,   32,   32 },
	{    8,   16,  128,  128 },
	{    8,   32,  128,  256 },
	{   32,  128,  258, 1024 },
	{   32,  258,  258, 4096 }
};

typedef struct ct_data
{
	union
	{
		ush	freq;
		ush	code;
	} fc;

	union
	{
		ush	dad;
		ush	len;
	} dl;
} ct_data;

typedef struct tree_desc
{
	ct_data*	dyn_tree;
	ct_data*	static_tree;
	const int*	extra_bits;
	int			extra_base;
	int			elems;
	int			max_length;
	int			max_code;
} tree_desc;

UINT	g_uiCodePageZ	= CP_ACP;
int		g_nLevel		= 8;

void ZipCodePage(UINT uiCodePage)
{
	g_uiCodePageZ	= uiCodePage;
}

void ZipLevel(int nLevel)
{
	if ((nLevel < 1) || (nLevel > 9))
	{
		nLevel	= 8;
	}

	g_nLevel	= nLevel;
}

class TTreeState
{
public:
	TTreeState();

	ct_data	dyn_ltree[HEAP_SIZE];
	ct_data	dyn_dtree[2 * D_CODES + 1];
	ct_data	static_ltree[L_CODES + 2];
	ct_data	static_dtree[D_CODES];
	ct_data	bl_tree[2 * BL_CODES + 1];

	tree_desc	l_desc;
	tree_desc	d_desc;
	tree_desc	bl_desc;

	ush	bl_count[MAX_BITS + 1];

	int	heap[2 * L_CODES + 1];
	int	heap_len;
	int	heap_max;

	uch	depth[2 * L_CODES + 1];
	uch	length_code[MAX_MATCH - MIN_MATCH + 1];
	uch	dist_code[512];

	int	base_length[LENGTH_CODES];
	int	base_dist[D_CODES];

	uch	l_buf[LIT_BUFSIZE];
	ush	d_buf[DIST_BUFSIZE];

	uch	flag_buf[(LIT_BUFSIZE / 8)];

	unsigned	last_lit;
	unsigned	last_dist;
	unsigned	last_flags;
	uch			flags;
	uch			flag_bit;

	ulg	opt_len;
	ulg	static_len;

	ulg	cmpr_bytelen;
	ulg	cmpr_len_bits;

	ulg	input_len;

	ush*	file_type;
};

TTreeState::TTreeState()
{
	tree_desc	a	= {dyn_ltree, static_ltree, extra_lbits, LITERALS + 1, L_CODES, MAX_BITS, 0};
	tree_desc	b	= {dyn_dtree, static_dtree, extra_dbits, 0, D_CODES, MAX_BITS, 0};
	tree_desc	c	= {bl_tree, NULL, extra_blbits, 0, BL_CODES, MAX_BL_BITS, 0};

	l_desc		= a;
	d_desc		= b;
	bl_desc		= c;
	last_lit	= 0;
	last_dist	= 0;
	last_flags	= 0;

	memset(dyn_ltree, 0, sizeof(dyn_ltree));
	memset(dyn_dtree, 0, sizeof(dyn_dtree));
	memset(static_ltree, 0, sizeof(static_ltree));
	memset(static_dtree, 0, sizeof(static_dtree));
	memset(bl_tree, 0, sizeof(bl_tree));
	memset(bl_count, 0, sizeof(bl_count));
	memset(heap, 0, sizeof(heap));

	heap_len	= 0;
	heap_max	= 0;

	memset(depth, 0, sizeof(depth));
	memset(length_code, 0, sizeof(length_code));
	memset(dist_code, 0, sizeof(dist_code));
	memset(base_length, 0, sizeof(base_length));
	memset(base_dist, 0, sizeof(base_dist));
	memset(l_buf, 0, sizeof(l_buf));
	memset(d_buf, 0, sizeof(d_buf));
	memset(flag_buf, 0, sizeof(flag_buf));

	last_lit		= 0;
	last_dist		= 0;
	last_flags		= 0;
	flags			= 0;
	flag_bit		= 0;
	opt_len			= 0;
	static_len		= 0;
	cmpr_bytelen	= 0;
	cmpr_len_bits	= 0;
	input_len		= 0;
	file_type		= 0;
}

class TBitState
{
public:
	TBitState()
	{
		flush_flg	= 0;
		bi_buf		= 0;
		bi_valid	= 0;
		out_buf		= 0;
		out_offset	= 0;
		out_size	= 0;
		bits_sent	= 0;
	}

	int			flush_flg;
	unsigned	bi_buf;
	int			bi_valid;
	char*		out_buf;
	unsigned	out_offset;
	unsigned	out_size;
	ulg			bits_sent;
};

class TDeflateState
{
public:
	TDeflateState()
	{
		memset(window, 0, sizeof(window));
		memset(prev, 0, sizeof(prev));
		memset(head, 0, sizeof(head));

		window_size			= 0;
		block_start			= 0;
		sliding				= 0;
		ins_h				= 0;
		prev_length			= 0;
		strstart			= 0;
		match_start			= 0;
		eofile				= 0;
		lookahead			= 0;
		max_chain_length	= 0;
		max_lazy_match		= 0;
		good_match			= 0;
		nice_match			= 0;
	}

	uch				window[2L * WSIZE];
	Pos				prev[WSIZE];
	Pos				head[HASH_SIZE];
	ulg				window_size;
	long			block_start;
	int				sliding;
	unsigned		ins_h;
	unsigned int	prev_length;
	unsigned		strstart;
	unsigned		match_start;
	int				eofile;
	unsigned		lookahead;
	unsigned		max_chain_length;
	unsigned int	max_lazy_match;
	unsigned		good_match;
	int				nice_match;
};

typedef long lutime_t;

typedef struct iztimes
{
	lutime_t	atime;
	lutime_t	mtime;
	lutime_t	ctime;
} iztimes;

typedef struct zlist
{
	ush		vem;
	ush		ver;
	ush		flg;
	ush		how;
	ulg		tim;
	ulg		crc;
	ulg		siz;
	ulg		len;
	extent	nam;
	extent	ext;
	extent	cext;
	extent	com;
	ush		dsk;
	ush		att;
	ush		lflg;
	ulg		atx;
	ulg		off;
	char	name[MAX_PATH];
	char*	extra;
	char*	cextra;
	char*	comment;
	char	iname[MAX_PATH];
	char	zname[MAX_PATH];
	int		mark;
	int		trash;
	int		dosflag;
	struct	zlist*	nxt;
} TZipFileInfo;

class TState;

typedef unsigned (*READFUNC)(TState& state, char* buf, unsigned size);
typedef unsigned (*FLUSHFUNC)(void* param, const char* buf, unsigned* size);
typedef unsigned (*WRITEFUNC)(void* param, const char* buf, unsigned size);

class TState
{
public:
	TState()
	{
		param			= 0;
		level			= 0;
		seekable		= FALSE;
		readfunc		= 0;
		flush_outbuf	= 0;
		err				= 0;
	}

	void*			param;
	int				level;
	bool			seekable;
	READFUNC		readfunc;
	FLUSHFUNC		flush_outbuf;
	TTreeState		ts;
	TBitState		bs;
	TDeflateState	ds;
	const char*		err;
};

void filetime2dosdatetime(const FILETIME ft, WORD* dosdate, WORD* dostime)
{
	SYSTEMTIME	st;

	FileTimeToSystemTime(&ft, &st);

	*dosdate	= (WORD)(((st.wYear - 1980) & 0x7f) << 9);
	*dosdate	|= (WORD)((st.wMonth & 0x0f) << 5);
	*dosdate	|= (WORD)((st.wDay & 0x1f));
	*dostime	= (WORD)((st.wHour & 0x1f) << 11);
	*dostime	|= (WORD)((st.wMinute & 0x3f) << 5);
	*dostime	|= (WORD)((st.wSecond * 2) & 0x1f);
}

lutime_t	filetime2timet(const FILETIME ft)
{
	LONGLONG	i	= *(LONGLONG*)&ft;

	return (lutime_t)((i - 116444736000000000LL) / 10000000LL);
}

void GetNow(lutime_t* pft, WORD* dosdate, WORD* dostime)
{
	SYSTEMTIME	st;
	FILETIME	ft;

	GetLocalTime(&st);
	SystemTimeToFileTime(&st, &ft);
	filetime2dosdatetime(ft, dosdate, dostime);

	*pft	= filetime2timet(ft);
}

DWORD GetFilePosZ(HANDLE hfout)
{
	return SetFilePointer(hfout, 0, 0, FILE_CURRENT);
}

ZRESULT GetFileInfo(HANDLE hf, ulg* attr, long* size, iztimes* times, ulg* timestamp)
{
	BY_HANDLE_FILE_INFORMATION	bhi;
	BOOL						res	= GetFileInformationByHandle(hf, &bhi);

	if (!res)
	{
		return ZR_NOFILE;
	}

	DWORD	fa	= bhi.dwFileAttributes;
	ulg		a	= 0;

	if (fa & FILE_ATTRIBUTE_READONLY)
	{
		a	|= 0x01;
	}

	if (fa & FILE_ATTRIBUTE_HIDDEN)
	{
		a	|= 0x02;
	}

	if (fa & FILE_ATTRIBUTE_SYSTEM)
	{
		a	|= 0x04;
	}

	if (fa & FILE_ATTRIBUTE_DIRECTORY)
	{
		a	|= 0x10;
	}

	if (fa & FILE_ATTRIBUTE_ARCHIVE)
	{
		a	|= 0x20;
	}

	if (fa & FILE_ATTRIBUTE_DIRECTORY)
	{
		a	|= 0x40000000;
	}
	else
	{
		a	|= 0x80000000;
	}

	a	|= 0x01000000;

	if (fa & FILE_ATTRIBUTE_READONLY)
	{
	}
	else
	{
		a	|= 0x00800000;
	}

	DWORD 	red;
	DWORD	hsize	= GetFileSize(hf, NULL);

	if (hsize > 40)
	{
		unsigned short	magic;
		unsigned long	hpos;

		SetFilePointer(hf, 0, NULL, FILE_BEGIN);
		ReadFile(hf, &magic, sizeof(magic), &red, NULL);
		SetFilePointer(hf, 36, NULL, FILE_BEGIN);
		ReadFile(hf, &hpos, sizeof(hpos), &red, NULL);

		if (magic == 0x54AD && hsize > hpos + 4 + 20 + 28)
		{
			unsigned long	signature;

			SetFilePointer(hf, hpos, NULL, FILE_BEGIN);
			ReadFile(hf, &signature, sizeof(signature), &red, NULL);

			if (signature == IMAGE_DOS_SIGNATURE || signature == IMAGE_OS2_SIGNATURE || signature == IMAGE_OS2_SIGNATURE_LE || signature == IMAGE_NT_SIGNATURE)
			{
				a	|= 0x00400000;
			}
		}
	}

	if (attr != NULL)
	{
		*attr	= a;
	}

	if (size != NULL)
	{
		*size	= hsize;
	}

	if (times != NULL)
	{
		times->atime	= filetime2timet(bhi.ftLastAccessTime);
		times->mtime	= filetime2timet(bhi.ftLastWriteTime);
		times->ctime	= filetime2timet(bhi.ftCreationTime);
	}

	if (timestamp != NULL)
	{
		WORD	dosdate;
		WORD	dostime;

		filetime2dosdatetime(bhi.ftLastWriteTime, &dosdate, &dostime);

		*timestamp	= (WORD)dostime | (((DWORD)dosdate) << 16);
	}

	return ZR_OK;
}

void Assert(TState& state, bool cond, const char* msg)
{
	if (cond)
	{
		return;
	}

	state.err	= msg;
}

void Trace(const char* x, ...)
{
	va_list	paramList;

	va_start(paramList, x);
	paramList;
	va_end(paramList);
}

void Tracec(bool, const char* x, ...)
{
	va_list	paramList;

	va_start(paramList, x);
	paramList;
	va_end(paramList);
}

void		init_block(TState&);
void		pqdownheap(TState&, ct_data* tree, int k);
void		gen_bitlen(TState&, tree_desc* desc);
void		gen_codes(TState& state, ct_data* tree, int max_code);
void		build_tree(TState&,tree_desc* desc);
void		scan_tree(TState&, ct_data* tree, int max_code);
void		send_tree(TState& state, ct_data* tree, int max_code);
int			build_bl_tree(TState&);
void		send_all_trees(TState& state,int lcodes, int dcodes, int blcodes);
void		compress_block(TState& state, ct_data* ltree, ct_data* dtree);
void		set_file_type(TState&);
void		send_bits(TState& state, int value, int length);
unsigned	bi_reverse(unsigned code, int len);
void		bi_windup(TState& state);
void		copy_block(TState& state, char* buf, unsigned len, int header);

#define send_code(state, c, tree)	send_bits(state, tree[c].fc.code, tree[c].dl.len)
#define d_code(dist)				((dist) < 256 ? state.ts.dist_code[dist] : state.ts.dist_code[256 + ((dist) >> 7)])
#define Max(a, b)					(a >= b ? a : b)

void ct_init(TState& state, ush* attr)
{
	int	n;
	int	bits;
	int	length;
	int	code;
	int	dist;

	state.ts.file_type		= attr;
	state.ts.cmpr_bytelen	= 0L;
	state.ts.cmpr_len_bits	= 0L;
	state.ts.input_len		= 0L;

	if (state.ts.static_dtree[0].dl.len != 0)
	{
		return;
	}

	length	= 0;

	for (code = 0; code < LENGTH_CODES - 1; code++)
	{
		state.ts.base_length[code]	= length;

		for (n = 0; n < (1 << extra_lbits[code]); n++)
		{
			state.ts.length_code[length++]	= (uch)code;
		}
	}

	Assert(state, length == 256, "ct_init: length != 256");

	state.ts.length_code[length - 1]	= (uch)code;
	dist								= 0;

	for (code = 0 ; code < 16; code++)
	{
		state.ts.base_dist[code]	= dist;

		for (n = 0; n < (1 << extra_dbits[code]); n++)
		{
			state.ts.dist_code[dist++]	= (uch)code;
		}
	}

	Assert(state, dist == 256, "ct_init: dist != 256");

	dist	>>= 7;

	for ( ; code < D_CODES; code++)
	{
		state.ts.base_dist[code]	= dist << 7;

		for (n = 0; n < (1 << (extra_dbits[code] - 7)); n++)
		{
			state.ts.dist_code[256 + dist++]	= (uch)code;
		}
	}

	Assert(state, dist == 256, "ct_init: 256 + dist != 512");

	for (bits = 0; bits <= MAX_BITS; bits++)
	{
		state.ts.bl_count[bits]	= 0;
	}

	n	= 0;

	while (n <= 143)
	{
		state.ts.static_ltree[n++].dl.len	= 8, state.ts.bl_count[8]++;
	}

	while (n <= 255)
	{
		state.ts.static_ltree[n++].dl.len	= 9, state.ts.bl_count[9]++;
	}

	while (n <= 279)
	{
		state.ts.static_ltree[n++].dl.len	= 7, state.ts.bl_count[7]++;
	}

	while (n <= 287)
	{
		state.ts.static_ltree[n++].dl.len	= 8, state.ts.bl_count[8]++;
	}

	gen_codes(state, (ct_data*)state.ts.static_ltree, L_CODES + 1);

	for (n = 0; n < D_CODES; n++)
	{
		state.ts.static_dtree[n].dl.len		= 5;
		state.ts.static_dtree[n].fc.code	= (ush)bi_reverse(n, 5);
	}

	init_block(state);
}

void init_block(TState& state)
{
	int	n;

	for (n = 0; n < L_CODES; n++)
	{
		state.ts.dyn_ltree[n].fc.freq	= 0;
	}

	for (n = 0; n < D_CODES; n++)
	{
		state.ts.dyn_dtree[n].fc.freq	= 0;
	}

	for (n = 0; n < BL_CODES; n++)
	{
		state.ts.bl_tree[n].fc.freq	= 0;
	}

	state.ts.dyn_ltree[END_BLOCK].fc.freq	= 1;
	state.ts.opt_len						= 0L;
	state.ts.static_len						= 0L;
	state.ts.last_lit						= 0;
	state.ts.last_dist						= 0;
	state.ts.last_flags						= 0;
	state.ts.flags							= 0;
	state.ts.flag_bit						= 1;
}

#define SMALLEST			1
#define pqremove(tree, top)	{ top = state.ts.heap[SMALLEST]; state.ts.heap[SMALLEST] = state.ts.heap[state.ts.heap_len--]; pqdownheap(state, tree, SMALLEST); }
#define smaller(tree, n, m)	(tree[n].fc.freq < tree[m].fc.freq || (tree[n].fc.freq == tree[m].fc.freq && state.ts.depth[n] <= state.ts.depth[m]))

void pqdownheap(TState& state,ct_data* tree, int k)
{
	int	v	= state.ts.heap[k];
	int	j	= k << 1;
	int	htemp;

	while (j <= state.ts.heap_len)
	{
		if (j < state.ts.heap_len && smaller(tree, state.ts.heap[j + 1], state.ts.heap[j]))
		{
			j++;
		}

		htemp	= state.ts.heap[j];

		if (smaller(tree, v, htemp))
		{
			break;
		}

		state.ts.heap[k]	= htemp;
		k					= j;
		j					<<= 1;
	}

	state.ts.heap[k]	= v;
}

void gen_bitlen(TState& state,tree_desc* desc)
{
	ct_data*	tree		= desc->dyn_tree;
	const int*	extra		= desc->extra_bits;
	int			base		= desc->extra_base;
	int			max_code	= desc->max_code;
	int			max_length	= desc->max_length;
	ct_data*	stree		= desc->static_tree;
	int			h;
	int			n;
	int			m;
	int			bits;
	int			xbits;
	ush			f;
	int			overflow	= 0;

	for (bits = 0; bits <= MAX_BITS; bits++)
	{
		state.ts.bl_count[bits]	= 0;
	}

	tree[state.ts.heap[state.ts.heap_max]].dl.len	= 0;

	for (h = state.ts.heap_max + 1; h < HEAP_SIZE; h++)
	{
		n		= state.ts.heap[h];
		bits	= tree[tree[n].dl.dad].dl.len + 1;

		if (bits > max_length)
		{
			bits	= max_length, overflow++;
		}

		tree[n].dl.len	= (ush)bits;

		if (n > max_code)
		{
			continue;
		}

		state.ts.bl_count[bits]++;
		xbits	= 0;

		if (n >= base)
		{
			xbits	= extra[n - base];
		}

		f					= tree[n].fc.freq;
		state.ts.opt_len	+= (ulg)f * (bits + xbits);

		if (stree)
		{
			state.ts.static_len	+= (ulg)f * (stree[n].dl.len + xbits);
		}
	}

	if (overflow == 0)
	{
		return;
	}

	Trace("\nbit length overflow\n");

	do
	{
		bits	= max_length - 1;

		while (state.ts.bl_count[bits] == 0)
		{
			bits--;
		}

		state.ts.bl_count[bits]--;
		state.ts.bl_count[bits + 1]		+= (ush)2;
		state.ts.bl_count[max_length]--;
		overflow						-= 2;
	} while (overflow > 0);

	for (bits = max_length; bits != 0; bits--)
	{
		n	= state.ts.bl_count[bits];

		while (n != 0)
		{
			m	= state.ts.heap[--h];

			if (m > max_code)
			{
				continue;
			}

			if (tree[m].dl.len != (ush)bits)
			{
				Trace("code %d bits %d->%d\n", m, tree[m].dl.len, bits);

				state.ts.opt_len	+= ((long)bits - (long)tree[m].dl.len) * (long)tree[m].fc.freq;
				tree[m].dl.len		= (ush)bits;
			}

			n--;
		}
	}
}

void gen_codes(TState& state, ct_data* tree, int max_code)
{
	ush	next_code[MAX_BITS + 1];
	ush	code	= 0;
	int	bits;
	int	n;

	for (bits = 1; bits <= MAX_BITS; bits++)
	{
		next_code[bits]	= code	= (ush)((code + state.ts.bl_count[bits - 1]) << 1);
	}

	Assert(state, code + state.ts.bl_count[MAX_BITS] - 1 == (1 << ((ush)MAX_BITS)) - 1, "inconsistent bit counts");
	Trace("\ngen_codes: max_code %d ", max_code);

	for (n = 0; n <= max_code; n++)
	{
		int	len	= tree[n].dl.len;

		if (len == 0)
		{
			continue;
		}

		tree[n].fc.code	= (ush)bi_reverse(next_code[len]++, len);
	}
}

void build_tree(TState& state,tree_desc* desc)
{
	ct_data*	tree		= desc->dyn_tree;
	ct_data*	stree		= desc->static_tree;
	int			elems		= desc->elems;
	int			n;
	int			m;
	int			max_code	= -1;
	int			node		= elems;

	state.ts.heap_len	= 0, state.ts.heap_max	= HEAP_SIZE;

	for (n = 0; n < elems; n++)
	{
		if (tree[n].fc.freq != 0)
		{
			state.ts.heap[++state.ts.heap_len]	= max_code	= n;
			state.ts.depth[n]					= 0;
		}
		else
		{
			tree[n].dl.len	= 0;
		}
	}

	while (state.ts.heap_len < 2)
	{
		int	newcp	= state.ts.heap[++state.ts.heap_len]	= (max_code < 2 ? ++max_code : 0);

		tree[newcp].fc.freq		= 1;
		state.ts.depth[newcp]	= 0;
		state.ts.opt_len--;

		if (stree)
		{
			state.ts.static_len	-= stree[newcp].dl.len;
		}
	}

	desc->max_code	= max_code;

	for (n = state.ts.heap_len / 2; n >= 1; n--)
	{
		pqdownheap(state, tree, n);
	}

	do
	{
		pqremove(tree, n);

		m	= state.ts.heap[SMALLEST];

		state.ts.heap[--state.ts.heap_max]	= n;
		state.ts.heap[--state.ts.heap_max]	= m;
		tree[node].fc.freq					= (ush)(tree[n].fc.freq + tree[m].fc.freq);
		state.ts.depth[node]				= (uch)(Max(state.ts.depth[n], state.ts.depth[m]) + 1);
		tree[n].dl.dad						= tree[m].dl.dad	= (ush)node;
		state.ts.heap[SMALLEST]				= node++;

		pqdownheap(state, tree, SMALLEST);
	} while (state.ts.heap_len >= 2);

	state.ts.heap[--state.ts.heap_max]	= state.ts.heap[SMALLEST];

	gen_bitlen(state, (tree_desc*)desc);
	gen_codes(state, (ct_data*)tree, max_code);
}

void scan_tree(TState& state, ct_data* tree, int max_code)
{
	int	n;
	int	prevlen		= -1;
	int	curlen;
	int	nextlen		= tree[0].dl.len;
	int	count		= 0;
	int	max_count	= 7;
	int	min_count	= 4;

	if (nextlen == 0)
	{
		max_count	= 138, min_count	= 3;
	}

	tree[max_code + 1].dl.len	= (ush)-1;

	for (n = 0; n <= max_code; n++)
	{
		curlen	= nextlen;
		nextlen	= tree[n + 1].dl.len;

		if (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		else if (count < min_count)
		{
			state.ts.bl_tree[curlen].fc.freq	= (ush)(state.ts.bl_tree[curlen].fc.freq + count);
		}
		else if (curlen != 0)
		{
			if (curlen != prevlen)
			{
				state.ts.bl_tree[curlen].fc.freq++;
			}

			state.ts.bl_tree[REP_3_6].fc.freq++;
		}
		else if (count <= 10)
		{
			state.ts.bl_tree[REPZ_3_10].fc.freq++;
		}
		else
		{
			state.ts.bl_tree[REPZ_11_138].fc.freq++;
		}

		count	= 0;
		prevlen	= curlen;

		if (nextlen == 0)
		{
			max_count	= 138, min_count	= 3;
		}
		else if (curlen == nextlen)
		{
			max_count	= 6, min_count	= 3;
		}
		else
		{
			max_count	= 7, min_count	= 4;
		}
	}
}

void send_tree(TState& state, ct_data* tree, int max_code)
{
	int	n;
	int	prevlen		= -1;
	int	curlen;
	int	nextlen		= tree[0].dl.len;
	int	count		= 0;
	int	max_count	= 7;
	int	min_count	= 4;

	if (nextlen == 0)
	{
		max_count	= 138;
		min_count	= 3;
	}

	for (n = 0; n <= max_code; n++)
	{
		curlen	= nextlen;
		nextlen	= tree[n + 1].dl.len;

		if (++count < max_count && curlen == nextlen)
		{
			continue;
		}
		else if (count < min_count)
		{
			do
			{
				send_code(state, curlen, state.ts.bl_tree);
			} while (--count != 0);
		}
		else if (curlen != 0)
		{
			if (curlen != prevlen)
			{
				send_code(state, curlen, state.ts.bl_tree);

				count--;
			}

			Assert(state, count >= 3 && count <= 6, " 3_6?");
			send_code(state, REP_3_6, state.ts.bl_tree);
			send_bits(state, count - 3, 2);
		}
		else if (count <= 10)
		{
			send_code(state, REPZ_3_10, state.ts.bl_tree);
			send_bits(state, count - 3, 3);
		}
		else
		{
			send_code(state, REPZ_11_138, state.ts.bl_tree);
			send_bits(state, count - 11, 7);
		}

		count	= 0;
		prevlen	= curlen;

		if (nextlen == 0)
		{
			max_count	= 138, min_count	= 3;
		}
		else if (curlen == nextlen)
		{
			max_count	= 6, min_count	= 3;
		}
		else
		{
			max_count	= 7, min_count	= 4;
		}
	}
}

int build_bl_tree(TState& state)
{
	int	max_blindex;

	scan_tree(state, (ct_data*)state.ts.dyn_ltree, state.ts.l_desc.max_code);
	scan_tree(state, (ct_data*)state.ts.dyn_dtree, state.ts.d_desc.max_code);
	build_tree(state, (tree_desc*)(&state.ts.bl_desc));

	for (max_blindex = BL_CODES - 1; max_blindex >= 3; max_blindex--)
	{
		if (state.ts.bl_tree[bl_order[max_blindex]].dl.len != 0)
		{
			break;
		}
	}

	state.ts.opt_len	+= 3 * (max_blindex + 1) + 5 + 5 + 4;

	Trace("\ndyn trees: dyn %ld, stat %ld", state.ts.opt_len, state.ts.static_len);
	return max_blindex;
}

void send_all_trees(TState& state, int lcodes, int dcodes, int blcodes)
{
	int	rank;

	Assert(state, lcodes >= 257 && dcodes >= 1 && blcodes >= 4, "not enough codes");
	Assert(state, lcodes <= L_CODES && dcodes <= D_CODES && blcodes <= BL_CODES, "too many codes");
	Trace("\nbl counts: ");
	send_bits(state, lcodes - 257, 5);
	send_bits(state, dcodes - 1, 5);
	send_bits(state, blcodes - 4, 4);

	for (rank = 0; rank < blcodes; rank++)
	{
		Trace("\nbl code %2d ", bl_order[rank]);
		send_bits(state, state.ts.bl_tree[bl_order[rank]].dl.len, 3);
	}

	Trace("\nbl tree: sent %ld", state.bs.bits_sent);
	send_tree(state, (ct_data*)state.ts.dyn_ltree, lcodes - 1);
	Trace("\nlit tree: sent %ld", state.bs.bits_sent);
	send_tree(state, (ct_data*)state.ts.dyn_dtree, dcodes - 1);
	Trace("\ndist tree: sent %ld", state.bs.bits_sent);
}

ulg flush_block(TState& state,char* buf, ulg stored_len, int eof)
{
	ulg	opt_lenb;
	ulg	static_lenb;
	int	max_blindex;

	state.ts.flag_buf[state.ts.last_flags]	= state.ts.flags;

	if (*state.ts.file_type == (ush)UNKNOWN)
	{
		set_file_type(state);
	}

	build_tree(state, (tree_desc*)(&state.ts.l_desc));
	Trace("\nlit data: dyn %ld, stat %ld", state.ts.opt_len, state.ts.static_len);
	build_tree(state, (tree_desc*)(&state.ts.d_desc));
	Trace("\ndist data: dyn %ld, stat %ld", state.ts.opt_len, state.ts.static_len);

	max_blindex			= build_bl_tree(state);
	opt_lenb			= (state.ts.opt_len + 3 + 7) >> 3;
	static_lenb			= (state.ts.static_len + 3 + 7) >> 3;
	state.ts.input_len	+= stored_len;

	Trace("\nopt %lu(%lu) stat %lu(%lu) stored %lu lit %u dist %u ", opt_lenb, state.ts.opt_len, static_lenb, state.ts.static_len, stored_len, state.ts.last_lit, state.ts.last_dist);

	if (static_lenb <= opt_lenb)
	{
		opt_lenb	= static_lenb;
	}

	if (stored_len + 4 <= opt_lenb && buf != (char*)NULL)
	{
		send_bits(state, (STORED_BLOCK << 1) + eof, 3);

		state.ts.cmpr_bytelen	+= ((state.ts.cmpr_len_bits + 3 + 7) >> 3) + stored_len + 4;
		state.ts.cmpr_len_bits	= 0L;

		copy_block(state, buf, (unsigned)stored_len, 1);
	}
	else if (static_lenb == opt_lenb)
	{
		send_bits(state, (STATIC_TREES << 1) + eof, 3);
		compress_block(state, (ct_data*)state.ts.static_ltree, (ct_data*)state.ts.static_dtree);

		state.ts.cmpr_len_bits	+= 3 + state.ts.static_len;
		state.ts.cmpr_bytelen	+= state.ts.cmpr_len_bits >> 3;
		state.ts.cmpr_len_bits	&= 7L;
	}
	else
	{
		send_bits(state, (DYN_TREES << 1) + eof, 3);
		send_all_trees(state, state.ts.l_desc.max_code + 1, state.ts.d_desc.max_code + 1, max_blindex + 1);
		compress_block(state, (ct_data*)state.ts.dyn_ltree, (ct_data*)state.ts.dyn_dtree);

		state.ts.cmpr_len_bits	+= 3 + state.ts.opt_len;
		state.ts.cmpr_bytelen	+= state.ts.cmpr_len_bits >> 3;
		state.ts.cmpr_len_bits	&= 7L;
	}

	Assert(state, ((state.ts.cmpr_bytelen << 3) + state.ts.cmpr_len_bits) == state.bs.bits_sent, "bad compressed size");
	init_block(state);

	if (eof)
	{
		bi_windup(state);

		state.ts.cmpr_len_bits	+= 7;
	}

	Trace("\n");
	return state.ts.cmpr_bytelen + (state.ts.cmpr_len_bits >> 3);
}

int ct_tally(TState& state,int dist, int lc)
{
	state.ts.l_buf[state.ts.last_lit++]	= (uch)lc;

	if (dist == 0)
	{
		state.ts.dyn_ltree[lc].fc.freq++;
	}
	else
	{
		dist--;

		Assert(state, (ush)dist < (ush)MAX_DIST && (ush)lc <= (ush)(MAX_MATCH-MIN_MATCH) && (ush)d_code(dist) < (ush)D_CODES, "ct_tally: bad match");

		state.ts.dyn_ltree[state.ts.length_code[lc] + LITERALS + 1].fc.freq++;
		state.ts.dyn_dtree[d_code(dist)].fc.freq++;
		state.ts.d_buf[state.ts.last_dist++]	= (ush)dist;
		state.ts.flags							|= state.ts.flag_bit;
	}

	state.ts.flag_bit	<<= 1;

	if ((state.ts.last_lit & 7) == 0)
	{
		state.ts.flag_buf[state.ts.last_flags++]	= state.ts.flags;
		state.ts.flags								= 0, state.ts.flag_bit	= 1;
	}

	if (state.level > 2 && (state.ts.last_lit & 0xfff) == 0)
	{
		ulg	out_length	= (ulg)state.ts.last_lit * 8L;
		ulg	in_length	= (ulg)state.ds.strstart - state.ds.block_start;
		int	dcode;

		for (dcode = 0; dcode < D_CODES; dcode++)
		{
			out_length	+= (ulg)state.ts.dyn_dtree[dcode].fc.freq * (5L + extra_dbits[dcode]);
		}

		out_length	>>= 3;

		Trace("\nlast_lit %u, last_dist %u, in %ld, out ~%ld(%ld%%) ", state.ts.last_lit, state.ts.last_dist, in_length, out_length, 100L - out_length * 100L / in_length);

		if (state.ts.last_dist < state.ts.last_lit / 2 && out_length < in_length / 2)
		{
			return 1;
		}
	}

	return (state.ts.last_lit == LIT_BUFSIZE - 1 || state.ts.last_dist == DIST_BUFSIZE);
}

void compress_block(TState& state,ct_data* ltree, ct_data* dtree)
{
	unsigned	dist;
	int			lc;
	unsigned	lx		= 0;
	unsigned	dx		= 0;
	unsigned	fx		= 0;
	uch			flag	= 0;
	unsigned	code;
	int			extra;

	if (state.ts.last_lit != 0)
	{
		do
		{
			if ((lx & 7) == 0)
			{
				flag	= state.ts.flag_buf[fx++];
			}

			lc	= state.ts.l_buf[lx++];

			if ((flag & 1) == 0)
			{
				send_code(state, lc, ltree);
			}
			else
			{
				code	= state.ts.length_code[lc];

				send_code(state, code + LITERALS + 1, ltree);

				extra	= extra_lbits[code];

				if (extra != 0)
				{
					lc	-= state.ts.base_length[code];

					send_bits(state, lc, extra);
				}

				dist	= state.ts.d_buf[dx++];
				code	= d_code(dist);

				Assert(state, code < D_CODES, "bad d_code");
				send_code(state, code, dtree);

				extra	= extra_dbits[code];

				if (extra != 0)
				{
					dist	-= state.ts.base_dist[code];

					send_bits(state, dist, extra);
				}
			}

			flag	>>= 1;
		} while (lx < state.ts.last_lit);
	}

	send_code(state, END_BLOCK, ltree);
}

void set_file_type(TState& state)
{
	int			n			= 0;
	unsigned	ascii_freq	= 0;
	unsigned	bin_freq	= 0;

	while (n < 7)
	{
		bin_freq	+= state.ts.dyn_ltree[n++].fc.freq;
	}

	while (n < 128)
	{
		ascii_freq	+= state.ts.dyn_ltree[n++].fc.freq;
	}

	while (n < LITERALS)
	{
		bin_freq	+= state.ts.dyn_ltree[n++].fc.freq;
	}

	*state.ts.file_type	= (ush)(bin_freq > (ascii_freq >> 2) ? BINARY : ASCII);
}

void bi_init(TState& state,char* tgt_buf, unsigned tgt_size, int flsh_allowed)
{
	state.bs.out_buf	= tgt_buf;
	state.bs.out_size	= tgt_size;
	state.bs.out_offset	= 0;
	state.bs.flush_flg	= flsh_allowed;
	state.bs.bi_buf		= 0;
	state.bs.bi_valid	= 0;
	state.bs.bits_sent	= 0L;
}

void send_bits(TState &state,int value, int length)
{
	Assert(state, length > 0 && length <= 15, "invalid length");

	state.bs.bits_sent	+= (ulg)length;
	state.bs.bi_buf		|= (value << state.bs.bi_valid);
	state.bs.bi_valid	+= length;

	if (state.bs.bi_valid > (int)Buf_size)
	{
		PUTSHORT(state, state.bs.bi_buf);

		state.bs.bi_valid	-= Buf_size;
		state.bs.bi_buf		= (unsigned)value >> (length - state.bs.bi_valid);
	}
}

unsigned bi_reverse(unsigned code, int len)
{
	register unsigned	res	= 0;

	do
	{
		res		|= code & 1;
		code	>>= 1, res	<<= 1;
	} while (--len > 0);

	return res >> 1;
}

void bi_windup(TState& state)
{
	if (state.bs.bi_valid > 8)
	{
		PUTSHORT(state, state.bs.bi_buf);
	}
	else if (state.bs.bi_valid > 0)
	{
		PUTBYTE(state, state.bs.bi_buf);
	}

	if (state.bs.flush_flg)
	{
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);
	}

	state.bs.bi_buf		= 0;
	state.bs.bi_valid	= 0;
	state.bs.bits_sent	= (state.bs.bits_sent + 7) & ~7;
}

void copy_block(TState& state, char* block, unsigned len, int header)
{
	bi_windup(state);

	if (header)
	{
		PUTSHORT(state, (ush)len);
		PUTSHORT(state, (ush)~len);

		state.bs.bits_sent	+= 2 * 16;
	}

	if (state.bs.flush_flg)
	{
		state.flush_outbuf(state.param, state.bs.out_buf, &state.bs.out_offset);

		state.bs.out_offset	= len;

		state.flush_outbuf(state.param,block, &state.bs.out_offset);
	}
	else if (state.bs.out_offset + len > state.bs.out_size)
	{
		Assert(state, false, "output buffer too small for in-memory compression");
	}
	else
	{
		memcpy(state.bs.out_buf + state.bs.out_offset, block, len);

		state.bs.out_offset	+= len;
	}

	state.bs.bits_sent	+= (ulg)len << 3;
}

void	fill_window(TState& state);
ulg 	deflate_fast(TState& state);
int 	longest_match(TState& state, IPos cur_match);

#define UPDATE_HASH(h, c)				(h	= (((h) << H_SHIFT) ^ (c)) & HASH_MASK)
#define INSERT_STRING(s, match_head)	(UPDATE_HASH(state.ds.ins_h, state.ds.window[(s) + (MIN_MATCH - 1)]), state.ds.prev[(s) & WMASK]	= match_head	= state.ds.head[state.ds.ins_h], state.ds.head[state.ds.ins_h]	= (s))

void lm_init(TState& state, int pack_level, ush* flags)
{
	register unsigned	j;

	Assert(state, pack_level >= 1 && pack_level <= 9, "bad pack level");

	state.ds.sliding	= 0;

	if (state.ds.window_size == 0L)
	{
		state.ds.sliding		= 1;
		state.ds.window_size	= (ulg)2L * WSIZE;
	}

	state.ds.head[HASH_SIZE - 1]	= NIL;

	memset((char*)state.ds.head, NIL, (unsigned)(HASH_SIZE - 1) * sizeof(*state.ds.head));

	state.ds.max_lazy_match		= configuration_table[pack_level].max_lazy;
	state.ds.good_match			= configuration_table[pack_level].good_length;
	state.ds.nice_match			= configuration_table[pack_level].nice_length;
	state.ds.max_chain_length	= configuration_table[pack_level].max_chain;

	if (pack_level <= 2)
	{
		*flags	|= FAST;
	}
	else if (pack_level >= 8)
	{
		*flags	|= SLOW;
	}

	state.ds.strstart		= 0;
	state.ds.block_start	= 0L;
	j						= WSIZE;
	j						<<= 1;
	state.ds.lookahead		= state.readfunc(state, (char*)state.ds.window, j);

	if (state.ds.lookahead == 0 || state.ds.lookahead == (unsigned)EOF)
	{
		state.ds.eofile	= 1, state.ds.lookahead	= 0;

		return;
	}

	state.ds.eofile	= 0;

	if (state.ds.lookahead < MIN_LOOKAHEAD)
	{
		fill_window(state);
	}

	state.ds.ins_h	= 0;

	for (j = 0; j < MIN_MATCH - 1; j++)
	{
		UPDATE_HASH(state.ds.ins_h, state.ds.window[j]);
	}
}

int longest_match(TState& state,IPos cur_match)
{
	unsigned		chain_length 	= state.ds.max_chain_length;
	register uch*	scan			= state.ds.window + state.ds.strstart;
	register uch*	match;
	register int	len;
	int				best_len		= state.ds.prev_length;
	IPos			limit			= state.ds.strstart > (IPos)MAX_DIST ? state.ds.strstart - (IPos)MAX_DIST : NIL;

	Assert(state, HASH_BITS >= 8 && MAX_MATCH == 258, "Code too clever");

	register uch*	strend		= state.ds.window + state.ds.strstart + MAX_MATCH;
	register uch	scan_end1	= scan[best_len - 1];
	register uch	scan_end	= scan[best_len];

	if (state.ds.prev_length >= state.ds.good_match)
	{
		chain_length >>= 2;
	}

	Assert(state, state.ds.strstart <= state.ds.window_size - MIN_LOOKAHEAD, "insufficient lookahead");

	do
	{
		Assert(state, cur_match < state.ds.strstart, "no future");

		match	= state.ds.window + cur_match;

		if (match[best_len] != scan_end || match[best_len - 1] != scan_end1 || *match != *scan || *++match != scan[1])
		{
			continue;
		}

		scan	+= 2, match++;

		do
		{
		} while (*++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match && *++scan == *++match && scan < strend);

		Assert(state,scan <= state.ds.window + (unsigned)(state.ds.window_size - 1), "wild scan");

		len		= MAX_MATCH - (int)(strend - scan);
		scan	= strend - MAX_MATCH;

		if (len > best_len)
		{
			state.ds.match_start	= cur_match;
			best_len				= len;

			if (len >= state.ds.nice_match)
			{
				break;
			}

			scan_end1	= scan[best_len - 1];
			scan_end	= scan[best_len];
		}
	} while ((cur_match = state.ds.prev[cur_match & WMASK]) > limit && --chain_length != 0);

	return best_len;
}

#define check_match(state, start, match, length)

void fill_window(TState& state)
{
	register unsigned	n;
	register unsigned	m;
	unsigned			more;

	do
	{
		more	= (unsigned)(state.ds.window_size - (ulg)state.ds.lookahead - (ulg)state.ds.strstart);

		if (more == (unsigned)EOF)
		{
			more--;
		}
		else if (state.ds.strstart >= WSIZE + MAX_DIST && state.ds.sliding)
		{
			memcpy((char*)state.ds.window, (char*)state.ds.window + WSIZE, (unsigned)WSIZE);

			state.ds.match_start	-= WSIZE;
			state.ds.strstart		-= WSIZE;
			state.ds.block_start	-= (long)WSIZE;

			for (n = 0; n < HASH_SIZE; n++)
			{
				m					= state.ds.head[n];
				state.ds.head[n]	= (Pos)(m >= WSIZE ? m - WSIZE : NIL);
			}

			for (n = 0; n < WSIZE; n++)
			{
				m					= state.ds.prev[n];
				state.ds.prev[n]	= (Pos)(m >= WSIZE ? m - WSIZE : NIL);
			}

			more	+= WSIZE;
		}

		if (state.ds.eofile)
		{
			return;
		}

		Assert(state, more >= 2, "more < 2");

		n	= state.readfunc(state, (char*)state.ds.window + state.ds.strstart + state.ds.lookahead, more);

		if (n == 0 || n == (unsigned)EOF)
		{
			state.ds.eofile	= 1;
		}
		else
		{
			state.ds.lookahead	+= n;
		}
	} while (state.ds.lookahead < MIN_LOOKAHEAD && !state.ds.eofile);
}

#define FLUSH_BLOCK(state,eof)	flush_block(state, state.ds.block_start >= 0L ? (char*)&state.ds.window[(unsigned)state.ds.block_start] : (char*)NULL, (long)state.ds.strstart - state.ds.block_start, (eof))

ulg deflate_fast(TState& state)
{
	IPos		hash_head		= NIL;
	int			flush;
	unsigned	match_length	= 0;

	state.ds.prev_length	= MIN_MATCH - 1;

	while (state.ds.lookahead != 0)
	{
		if (state.ds.lookahead >= MIN_MATCH)
		{
			INSERT_STRING(state.ds.strstart, hash_head);
		}

		if (hash_head != NIL && state.ds.strstart - hash_head <= MAX_DIST)
		{
			if ((unsigned)state.ds.nice_match > state.ds.lookahead)
			{
				state.ds.nice_match	= (int)state.ds.lookahead;
			}

			match_length	= longest_match(state,hash_head);

			if (match_length > state.ds.lookahead)
			{
				match_length	= state.ds.lookahead;
			}
		}

		if (match_length >= MIN_MATCH)
		{
			check_match(state,state.ds.strstart, state.ds.match_start, match_length);

			flush				= ct_tally(state, state.ds.strstart - state.ds.match_start, match_length - MIN_MATCH);
			state.ds.lookahead	-= match_length;

			if (match_length <= state.ds.max_insert_length && state.ds.lookahead >= MIN_MATCH)
			{
				match_length--;

				do
				{
					state.ds.strstart++;

					INSERT_STRING(state.ds.strstart, hash_head);
				} while (--match_length != 0);

				state.ds.strstart++;
			}
			else
			{
				state.ds.strstart	+= match_length;
				match_length		= 0;
				state.ds.ins_h		= state.ds.window[state.ds.strstart];

				UPDATE_HASH(state.ds.ins_h, state.ds.window[state.ds.strstart + 1]);
				Assert(state, MIN_MATCH == 3, "Call UPDATE_HASH() MIN_MATCH-3 more times");
			}
		}
		else
		{
			flush	= ct_tally(state, 0, state.ds.window[state.ds.strstart]);
			state.ds.lookahead--;
			state.ds.strstart++;
		}

		if (flush)
		{
			FLUSH_BLOCK(state, 0), state.ds.block_start	= state.ds.strstart;
		}

		if (state.ds.lookahead < MIN_LOOKAHEAD)
		{
			fill_window(state);
		}
	}

	return FLUSH_BLOCK(state, 1);
}

ulg deflate(TState& state)
{
	IPos				hash_head		= NIL;
	IPos				prev_match;
	int					flush;
	int					match_available	= 0;
	register unsigned	match_length	= MIN_MATCH - 1;

	if (state.level <= 3)
	{
		return deflate_fast(state);
	}

	while (state.ds.lookahead != 0)
	{
		if (state.ds.lookahead >= MIN_MATCH)
		{
			INSERT_STRING(state.ds.strstart, hash_head);
		}

		state.ds.prev_length	= match_length, prev_match	= state.ds.match_start;
		match_length			= MIN_MATCH - 1;

		if (hash_head != NIL && state.ds.prev_length < state.ds.max_lazy_match && state.ds.strstart - hash_head <= MAX_DIST)
		{
			if ((unsigned)state.ds.nice_match > state.ds.lookahead)
			{
				state.ds.nice_match	= (int)state.ds.lookahead;
			}

			match_length	= longest_match(state,hash_head);

			if (match_length > state.ds.lookahead)
			{
				match_length	= state.ds.lookahead;
			}

			if (match_length == MIN_MATCH && state.ds.strstart - state.ds.match_start > TOO_FAR)
			{
				match_length	= MIN_MATCH - 1;
			}
		}

		if (state.ds.prev_length >= MIN_MATCH && match_length <= state.ds.prev_length)
		{
			unsigned	max_insert	= state.ds.strstart + state.ds.lookahead - MIN_MATCH;

			check_match(state, state.ds.strstart - 1, prev_match, state.ds.prev_length);

			flush					= ct_tally(state, state.ds.strstart - 1 - prev_match, state.ds.prev_length - MIN_MATCH);
			state.ds.lookahead		-= state.ds.prev_length - 1;
			state.ds.prev_length	-= 2;

			do
			{
				if (++state.ds.strstart <= max_insert)
				{
					INSERT_STRING(state.ds.strstart, hash_head);
				}
			} while (--state.ds.prev_length != 0);

			state.ds.strstart++;
			match_available	= 0;
			match_length	= MIN_MATCH - 1;

			if (flush)
			{
				FLUSH_BLOCK(state, 0), state.ds.block_start	= state.ds.strstart;
			}
		}
		else if (match_available)
		{
			if (ct_tally (state, 0, state.ds.window[state.ds.strstart - 1]))
			{
				FLUSH_BLOCK(state, 0), state.ds.block_start	= state.ds.strstart;
			}

			state.ds.strstart++;
			state.ds.lookahead--;
		}
		else
		{
			match_available	= 1;
			state.ds.strstart++;
			state.ds.lookahead--;
		}

		if (state.ds.lookahead < MIN_LOOKAHEAD)
		{
			fill_window(state);
		}
	}

	if (match_available)
	{
		ct_tally(state, 0, state.ds.window[state.ds.strstart - 1]);
	}

	return FLUSH_BLOCK(state, 1);
}

int putlocal(struct zlist* z, WRITEFUNC wfunc,void* param)
{
	PUTLG(LOCSIG, f);
	PUTSH(z->ver, f);
	PUTSH(z->lflg, f);
	PUTSH(z->how, f);
	PUTLG(z->tim, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);
	PUTSH(z->nam, f);
	PUTSH(z->ext, f);

	size_t	res	= (size_t)wfunc(param, z->iname, (unsigned int)z->nam);

	if (res != z->nam)
	{
		return ZE_TEMP;
	}

	if (z->ext)
	{
		res	= (size_t)wfunc(param, z->extra, (unsigned int)z->ext);

		if (res != z->ext)
		{
			return ZE_TEMP;
		}
	}

	return ZE_OK;
}

int putextended(struct zlist* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(EXTLOCSIG, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);

	return ZE_OK;
}

int putcentral(struct zlist* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(CENSIG, f);
	PUTSH(z->vem, f);
	PUTSH(z->ver, f);
	PUTSH(z->flg, f);
	PUTSH(z->how, f);
	PUTLG(z->tim, f);
	PUTLG(z->crc, f);
	PUTLG(z->siz, f);
	PUTLG(z->len, f);
	PUTSH(z->nam, f);
	PUTSH(z->cext, f);
	PUTSH(z->com, f);
	PUTSH(z->dsk, f);
	PUTSH(z->att, f);
	PUTLG(z->atx, f);
	PUTLG(z->off, f);

	if ((size_t)wfunc(param, z->iname, (unsigned int)z->nam) != z->nam || (z->cext && (size_t)wfunc(param, z->cextra, (unsigned int)z->cext) != z->cext) || (z->com && (size_t)wfunc(param, z->comment, (unsigned int)z->com) != z->com))
	{
		return ZE_TEMP;
	}

	return ZE_OK;
}

int putend(int n, ulg s, ulg c, extent m, char* z, WRITEFUNC wfunc, void* param)
{
	PUTLG(ENDSIG, f);
	PUTSH(0, f);
	PUTSH(0, f);
	PUTSH(n, f);
	PUTSH(n, f);
	PUTLG(s, f);
	PUTLG(c, f);
	PUTSH(m, f);

	if (m && wfunc(param, z, (unsigned int)m) != m)
	{
		return ZE_TEMP;
	}

	return ZE_OK;
}

const ulg	crc_table[256]	=
{
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

#define CRC32(c, b)	(crc_table[((int)(c) ^ (b)) & 0xff] ^ ((c) >> 8))
#define DO1(buf)	crc	= CRC32(crc, *buf++)
#define DO2(buf)	DO1(buf); DO1(buf)
#define DO4(buf)	DO2(buf); DO2(buf)
#define DO8(buf)	DO4(buf); DO4(buf)

ulg crc32(ulg crc, const uch* buf, extent len)
{
	if (buf == NULL)
	{
		return 0L;
	}

	crc	= crc ^ 0xffffffffL;

	while (len >= 8)
	{
		DO8(buf);

		len	-= 8;
	}

	if (len)
	{
		do
		{
			DO1(buf);
		} while (--len);
	}

	return crc ^ 0xffffffffL;
}

void update_keys(unsigned long*keys, char c)
{
	keys[0]	= CRC32(keys[0], c);
	keys[1]	+= keys[0] & 0xFF;
	keys[1]	= keys[1] * 134775813L + 1;
	keys[2]	= CRC32(keys[2], keys[1] >> 24);
}

char decrypt_byte(unsigned long* keys)
{
	unsigned	temp	= ((unsigned)keys[2] & 0xffff) | 2;

	return (char)(((temp * (temp ^ 1)) >> 8) & 0xff);
}

char zencode(unsigned long* keys, char c)
{
	int	t	= decrypt_byte(keys);

	update_keys(keys, c);
	return (char)(t ^ c);
}

int lustricmp(const TCHAR* sa, const TCHAR* sb)
{
	for (const TCHAR* ca = sa, *cb = sb; ; ca++, cb++)
	{
		int	ia	= tolower(*ca), ib	= tolower(*cb);

		if (ia == ib && ia == 0)
		{
			return 0;
		}

		if (ia == ib)
		{
			continue;
		}

		if (ia < ib)
		{
			return -1;
		}

		if (ia > ib)
		{
			return 1;
		}
	}
}

bool HasZipSuffix(const TCHAR* fn)
{
	const TCHAR*	ext	= fn + _tcslen(fn);

	while (ext > fn && *ext != _T('.'))
	{
		ext--;
	}

	if (ext == fn && *ext != _T('.'))
	{
		return false;
	}

	if (lustricmp(ext, _T(".Z")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".zip")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".zoo")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".arc")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".lzh")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".arj")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".gz")) == 0)
	{
		return true;
	}

	if (lustricmp(ext, _T(".tgz")) == 0)
	{
		return true;
	}

	return false;
}

class TZip
{
public:
#if TRUE
	TZip() : hfout(0), mustclosehfout(false), hmapout(0), zfis(0), obuf(0), hfin(0), writ(0), oerr(false), hasputcen(false), ooffset(0), encwriting(false), encbuf(0), password(0), state(0)
#else
	TZip() : hfout(0), hmapout(0), zfis(0), obuf(0), hfin(0), writ(0), oerr(false), hasputcen(false), ooffset(0)
#endif
	{
	}

	TZip(const char* pwd) : hfout(0), mustclosehfout(false), hmapout(0), zfis(0), obuf(0), hfin(0), writ(0), oerr(false), hasputcen(false), ooffset(0), encwriting(false), encbuf(0), password(0), state(0)
	{
		if (pwd != 0 && *pwd != 0)
		{
			password	= new char[strlen(pwd) + 1];

			strcpy(password, pwd);
		}
	}

	~TZip()
	{
		if (state != 0)
		{
			delete state;
		}

		state	= 0;

		if (encbuf != 0)
		{
			delete[] encbuf;
		}

		encbuf	= 0;

		if (password != 0)
		{
			delete[] password;
		}

		password	= 0;
	}

	char*			password;
	HANDLE			hfout;
	bool			mustclosehfout;
	HANDLE			hmapout;
	unsigned		ooffset;
	ZRESULT			oerr;
	unsigned		writ;
	bool			ocanseek;
	char*			obuf;
	unsigned int	opos;
	unsigned int	mapsize;
	bool			hasputcen;
	bool			encwriting;
	unsigned long	keys[3];
	char*			encbuf;
	unsigned int	encbufsize;
	TZipFileInfo*	zfis;
	TState*			state;

	ZRESULT			Create(void* z,unsigned int len, DWORD flags);
	static unsigned	sflush(void* param, const char* buf, unsigned* size);
	static unsigned	swrite(void* param, const char* buf, unsigned size);
	unsigned int	write(const char* buf, unsigned int size);
	bool			oseek(unsigned int pos);
	ZRESULT			GetMemory(void** pbuf, unsigned long* plen);
	ZRESULT			Close();

	ulg				attr;
	iztimes			times;
	ulg				timestamp;
	bool			iseekable;
	long			isize;
	long			ired;
	ulg				crc;
	HANDLE			hfin;
	bool			selfclosehf;
	const char*		bufin;
	unsigned int	lenin;
	unsigned int	posin;
	ulg				csize;
	char			buf[16384];

	ZRESULT			open_file(const TCHAR* fn);
	ZRESULT			open_handle(HANDLE hf, unsigned int len);
	ZRESULT			open_mem(void* src, unsigned int len);
	ZRESULT			open_dir();
	static unsigned	sread(TState& s, char* buf, unsigned size);
	unsigned		read(char* buf, unsigned size);
	ZRESULT			iclose();
	ZRESULT			ideflate(TZipFileInfo* zfi);
	ZRESULT			istore();
	ZRESULT			Add(const TCHAR* odstzn, void* src, unsigned int len, DWORD flags);
	ZRESULT			AddCentral();
};

ZRESULT TZip::Create(void* z,unsigned int len, DWORD flags)
{
	if (hfout != 0 || hmapout != 0 || obuf != 0 || writ != 0 || oerr != ZR_OK || hasputcen)
	{
		return ZR_NOTINITED;
	}

	if (flags == ZIP_HANDLE)
	{
		HANDLE	hf	= (HANDLE)z;

		hfout			=	hf;
		mustclosehfout	= false;

#ifdef DuplicateHandle
		BOOL	res	= DuplicateHandle(GetCurrentProcess(), hf, GetCurrentProcess(), &hfout, 0, FALSE, DUPLICATE_SAME_ACCESS);

		if (res)
		{
			mustclosehandle	= true;
		}
#endif

		DWORD	res	= GetFilePosZ(hfout);

		ocanseek	= (res != 0xFFFFFFFF);
		ooffset		= ocanseek ? res : 0;

		return ZR_OK;
	}
	else if (flags == ZIP_FILENAME)
	{
		const TCHAR*	fn	= (const TCHAR*)z;

		hfout	= CreateFileW(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hfout == INVALID_HANDLE_VALUE)
		{
			hfout	= 0;

			return ZR_NOFILE;
		}

		ocanseek		= true;
		ooffset			= 0;
		mustclosehfout	= true;
		return ZR_OK;
	}
	else if (flags == ZIP_MEMORY)
	{
		unsigned int	size	= len;

		if (size == 0)
		{
			return ZR_MEMSIZE;
		}

		if (z != 0)
		{
			obuf	= (char*)z;
		}
		else
		{
			hmapout	= CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, NULL);

			if (hmapout == NULL)
			{
				return ZR_NOALLOC;
			}

			obuf	= (char*)MapViewOfFile(hmapout, FILE_MAP_ALL_ACCESS, 0, 0, size);

			if (obuf == 0)
			{
				CloseHandle(hmapout);

				hmapout	= 0;

				return ZR_NOALLOC;
			}
		}

		ocanseek	= true;
		opos		= 0;
		mapsize		= size;

		return ZR_OK;
	}
	else
	{
		return ZR_ARGS;
	}
}

unsigned TZip::sflush(void* param, const char* buf, unsigned* size)
{
	if (*size == 0)
	{
		return 0;
	}

	TZip*			zip		= (TZip*)param;
	unsigned int	writ	= zip->write(buf, *size);

	if (writ != 0)
	{
		*size	= 0;
	}

	return writ;
}

unsigned TZip::swrite(void* param,const char* buf, unsigned size)
{
	if (size == 0)
	{
		return 0;
	}

	TZip*	zip	= (TZip*)param;

	return zip->write(buf, size);
}

unsigned int TZip::write(const char* buf, unsigned int size)
{
	const char*	srcbuf	= buf;

	if (encwriting)
	{
		if (encbuf != 0 && encbufsize < size)
		{
			delete[] encbuf;

			encbuf	= 0;
		}

		if (encbuf == 0)
		{
			encbuf		= new char[size * 2];
			encbufsize	= size;
		}

		memcpy(encbuf, buf, size);

		for (unsigned int i = 0; i < size; i++)
		{
			encbuf[i]	= zencode(keys, encbuf[i]);
		}

		srcbuf	= encbuf;
	}

	if (obuf != 0)
	{
		if (opos + size >= mapsize)
		{
			oerr	= ZR_MEMSIZE;

			return 0;
		}

		memcpy(obuf + opos, srcbuf, size);

		opos	+= size;

		return size;
	}
	else if (hfout != 0)
	{
		DWORD	writ	= 0;

		WriteFile(hfout, srcbuf, size, &writ, NULL);

		return writ;
	}

	oerr	= ZR_NOTINITED;

	return 0;
}

bool TZip::oseek(unsigned int pos)
{
	if (!ocanseek)
	{
		oerr	= ZR_SEEK;

		return false;
	}

	if (obuf != 0)
	{
		if (pos >= mapsize)
		{
			oerr	= ZR_MEMSIZE;

			return false;
		}

		opos	= pos;

		return true;
	}
	else if (hfout != 0)
	{
		SetFilePointer(hfout, pos + ooffset, NULL, FILE_BEGIN);
		return true;
	}

	oerr	= ZR_NOTINITED;

	return 0;
}

ZRESULT TZip::GetMemory(void** pbuf, unsigned long* plen)
{
	if (!hasputcen)
	{
		AddCentral();
	}

	hasputcen	= true;

	if (pbuf != NULL)
	{
		*pbuf	= (void*)obuf;
	}

	if (plen != NULL)
	{
		*plen	= writ;
	}

	if (obuf == NULL)
	{
		return ZR_NOTMMAP;
	}

	return ZR_OK;
}

ZRESULT TZip::Close()
{
	ZRESULT	res	= ZR_OK;

	if (!hasputcen)
	{
		res	= AddCentral();
	}

	hasputcen	= true;

	if (obuf != 0 && hmapout != 0)
	{
		UnmapViewOfFile(obuf);
	}

	obuf	= 0;

	if (hmapout != 0)
	{
		CloseHandle(hmapout);
	}

	hmapout	= 0;

	if (hfout != 0 && mustclosehfout)
	{
		CloseHandle(hfout);
	}

	hfout			= 0;
	mustclosehfout	= false;

	return res;
}

ZRESULT TZip::open_file(const TCHAR* fn)
{
	hfin		= 0;
	bufin		= 0;
	selfclosehf	= false;
	crc			= CRCVAL_INITIAL;
	isize		= 0;
	csize		= 0;
	ired		= 0;

	if (fn == 0)
	{
		return ZR_ARGS;
	}

	HANDLE	hf	= CreateFile(fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hf == INVALID_HANDLE_VALUE)
	{
		return ZR_NOFILE;
	}

	ZRESULT	res	= open_handle(hf, 0);

	if (res != ZR_OK)
	{
		CloseHandle(hf);
		return res;
	}

	selfclosehf	= true;

	return ZR_OK;
}

ZRESULT TZip::open_handle(HANDLE hf,unsigned int len)
{
	hfin		= 0;
	bufin		= 0;
	selfclosehf	= false;
	crc			= CRCVAL_INITIAL;
	isize		= 0;
	csize		= 0;
	ired		= 0
;
	if (hf == 0 || hf == INVALID_HANDLE_VALUE)
	{
		return ZR_ARGS;
	}

	bool	canseek;
	DWORD	res	= SetFilePointer(hfout, 0, 0, FILE_CURRENT);

	canseek	= (res != 0xFFFFFFFF);

	if (canseek)
	{
		ZRESULT	res	= GetFileInfo(hf, &attr, &isize, &times, &timestamp);

		if (res != ZR_OK)
		{
			return res;
		}

		SetFilePointer(hf, 0, NULL, FILE_BEGIN);

		iseekable	= true;
		hfin		= hf;

		return ZR_OK;
	}
	else
	{
		attr	= 0x80000000;
		isize	= -1;

		if (len != 0)
		{
			isize	= len;
		}

		iseekable	= false;

		WORD	dosdate;
		WORD	dostime;

		GetNow(&times.atime, &dosdate, &dostime);

		times.mtime	= times.atime;
		times.ctime	= times.atime;
		timestamp	= (WORD)dostime | (((DWORD)dosdate) << 16);
		hfin		= hf;

		return ZR_OK;
	}
}

ZRESULT TZip::open_mem(void* src, unsigned int len)
{
	hfin		= 0;
	bufin		= (const char*)src;
	selfclosehf	= false;
	crc			= CRCVAL_INITIAL;
	ired		= 0;
	csize		= 0;
	ired		= 0;
	lenin		= len;
	posin		= 0;

	if (src == 0 || len == 0)
	{
		return ZR_ARGS;
	}

	attr		= 0x80000000;
	isize		= len;
	iseekable	= true;

	WORD	dosdate;
	WORD	dostime;

	GetNow(&times.atime, &dosdate, &dostime);

	times.mtime	= times.atime;
	times.ctime	= times.atime;
	timestamp	= (WORD)dostime | (((DWORD)dosdate) << 16);

	return ZR_OK;
}

ZRESULT TZip::open_dir()
{
	hfin		= 0;
	bufin		= 0;
	selfclosehf	= false;
	crc			= CRCVAL_INITIAL;
	isize		= 0;
	csize		= 0;
	ired		= 0;
	attr		= 0x41C00010;
	isize		= 0;
	iseekable	= false;

	WORD	dosdate;
	WORD	dostime;

	GetNow(&times.atime, &dosdate, &dostime);

	times.mtime	= times.atime;
	times.ctime	= times.atime;
	timestamp	= (WORD)dostime | (((DWORD)dosdate) << 16);

	return ZR_OK;
}

unsigned TZip::sread(TState& s, char* buf, unsigned size)
{
	TZip*	zip	= (TZip*)s.param;

	return zip->read(buf, size);
}

unsigned TZip::read(char* buf, unsigned size)
{
	if (bufin != 0)
	{
		if (posin >= lenin)
		{
			return 0;
		}

		ulg	red	= lenin - posin;

		if (red > size)
		{
			red = size;
		}

		memcpy(buf, bufin + posin, red);

		posin	+= red;
		ired	+= red;
		crc		= crc32(crc, (uch*)buf, red);

		return red;
	}
	else if (hfin != 0)
	{
		DWORD	red;
		BOOL	ok	= ReadFile(hfin, buf, size, &red, NULL);

		if (!ok)
		{
			return 0;
		}

		ired	+= red;
		crc		= crc32(crc, (uch*)buf, red);

		return red;
	}
	else
	{
		oerr	= ZR_NOTINITED;

		return 0;
	}
}

ZRESULT TZip::iclose()
{
	if (selfclosehf && hfin != 0)
	{
		CloseHandle(hfin);
	}

	hfin	= 0;

	bool	mismatch	= (isize != -1 && isize != ired);

	isize	= ired;

	if (mismatch)
	{
		return ZR_MISSIZE;
	}

	return ZR_OK;
}

ZRESULT TZip::ideflate(TZipFileInfo* zfi)
{
	if (state == 0)
	{
		state	= new TState();
	}

	state->err							= 0;
	state->readfunc						= sread;
	state->flush_outbuf					= sflush;
	state->param						= this;
	state->level						= g_nLevel /*8*/;
	state->seekable						= iseekable;
	state->err							= NULL;
	state->ts.static_dtree[0].dl.len	= 0;
	state->ds.window_size				= 0;

	bi_init(*state, buf, sizeof(buf), 1);
	ct_init(*state, &zfi->att);
	lm_init(*state, state->level, &zfi->flg);

	ulg	sz	= deflate(*state);

	csize	= sz;

	ZRESULT	r	= ZR_OK;

	if (state->err != NULL)
	{
		r	= ZR_FLATE;
	}

	return r;
}

ZRESULT TZip::istore()
{
	ulg	size	= 0;

	for ( ; ; )
	{
		unsigned int	cin		= read(buf, 16384);

		if (cin <= 0 || cin == (unsigned int)EOF)
		{
			break;
		}

		unsigned int	cout	= write(buf, cin);

		if (cout != cin)
		{
			return ZR_MISSIZE;
		}

		size	+= cin;
	}

	csize	= size;

	return ZR_OK;
}

bool	has_seeded	= false;

ZRESULT TZip::Add(const TCHAR* odstzn, void*src, unsigned int len, DWORD flags)
{
	if (oerr)
	{
		return ZR_FAILED;
	}

	if (hasputcen)
	{
		return ZR_ENDED;
	}

	int	passex	= 0;

	if (password != 0 && flags != ZIP_FOLDER)
	{
		passex	= 12;
	}

	TCHAR	dstzn[MAX_PATH];

	_tcsncpy(dstzn, odstzn, MAX_PATH);

	dstzn[MAX_PATH - 1]	= 0;

	if (*dstzn == 0)
	{
		return ZR_ARGS;
	}

	TCHAR*	d	= dstzn;

	while (*d != 0)
	{
		if (*d == _T('\\'))
		{
			*d	= _T('/');
		}

		d++;
	}

	bool	isdir					= (flags == ZIP_FOLDER);
	bool	needs_trailing_slash	= (isdir && dstzn[_tcslen(dstzn) - 1] != _T('/'));
	int		method					= DEFLATE;

	if (isdir || HasZipSuffix(dstzn))
	{
		method	= STORE;
	}

	ZRESULT	openres;

	if (flags == ZIP_FILENAME)
	{
		openres	= open_file((const TCHAR*)src);
	}
	else if (flags == ZIP_HANDLE)
	{
		openres	= open_handle((HANDLE)src, len);
	}
	else if (flags == ZIP_MEMORY)
	{
		openres	= open_mem(src, len);
	}
	else if (flags == ZIP_FOLDER)
	{
		openres	= open_dir();
	}
	else
	{
		return ZR_ARGS;
	}

	if (openres!=ZR_OK)
	{
		return openres;
	}

	TZipFileInfo	zfi;

	zfi.nxt	= NULL;

	strcpy(zfi.name, "");

#ifdef UNICODE
	WideCharToMultiByte(g_uiCodePageZ /*CP_UTF8*/, 0, dstzn, -1, zfi.iname, MAX_PATH, 0, 0);
#else
	strncpy(zfi.iname, dstzn, MAX_PATH);

	zfi.iname[MAX_PATH - 1]	= 0;
#endif

	zfi.nam	= strlen(zfi.iname);

	if (needs_trailing_slash)
	{
		strcat(zfi.iname, "/");

		zfi.nam++;
	}

	strcpy(zfi.zname, "");

	zfi.extra	= NULL;
	zfi.ext		= 0;
	zfi.cextra	= NULL;
	zfi.cext	= 0;
	zfi.comment	= NULL;
	zfi.com		= 0;
	zfi.mark	= 1;
	zfi.dosflag	= 0;
	zfi.att		= (ush)BINARY;
	zfi.vem		= (ush)0xB17;
	zfi.ver		= (ush)20;
	zfi.tim		= timestamp;
	zfi.crc		= 0;
	zfi.flg		= 8;

	if (password != 0 && !isdir)
	{
		zfi.flg	= 9;
	}

	zfi.lflg	= zfi.flg;
	zfi.how		= (ush)method;
	zfi.siz		= (ulg)(method == STORE && isize >= 0 ? isize + passex : 0);
	zfi.len		= (ulg)(isize);
	zfi.dsk		= 0;
	zfi.atx		= attr;
	zfi.off		= writ + ooffset;

	// nb. apparently there's a problem with PocketPC CE(zip)->CE(unzip) fails. And removing the following block fixes it up.
	char	xloc[EB_L_UT_SIZE];

	zfi.extra	= xloc;
	zfi.ext		= EB_L_UT_SIZE;

	char	xcen[EB_C_UT_SIZE];

	zfi.cextra	= xcen;
	zfi.cext	= EB_C_UT_SIZE;

	xloc[0]		= 'U';
	xloc[1]		= 'T';
	xloc[2]		= EB_UT_LEN(3);
	xloc[3]		= 0;
	xloc[4]		= EB_UT_FL_MTIME | EB_UT_FL_ATIME | EB_UT_FL_CTIME;
	xloc[5]		= (char)(times.mtime);
	xloc[6]		= (char)(times.mtime >> 8);
	xloc[7]		= (char)(times.mtime >> 16);
	xloc[8]		= (char)(times.mtime >> 24);
	xloc[9]		= (char)(times.atime);
	xloc[10]	= (char)(times.atime >> 8);
	xloc[11]	= (char)(times.atime >> 16);
	xloc[12]	= (char)(times.atime >> 24);
	xloc[13]	= (char)(times.ctime);
	xloc[14]	= (char)(times.ctime >> 8);
	xloc[15]	= (char)(times.ctime >> 16);
	xloc[16]	= (char)(times.ctime >> 24);

	memcpy(zfi.cextra, zfi.extra, EB_C_UT_SIZE);

	zfi.cextra[EB_LEN]	= EB_UT_LEN(1);

	int	r	= putlocal(&zfi, swrite, this);

	if (r != ZE_OK)
	{
		iclose();
		return ZR_WRITE;
	}

	writ	+= 4 + LOCHEAD + (unsigned int)zfi.nam + (unsigned int)zfi.ext;

	if (oerr != ZR_OK)
	{
		iclose();
		return oerr;
	}

	keys[0]	= 305419896L;
	keys[1]	= 591751049L;
	keys[2]	= 878082192L;

	for (const char* cp = password; cp != 0 && *cp != 0; cp++)
	{
		update_keys(keys, *cp);
	}

	if (!has_seeded)
	{
		srand(GetTickCount() ^ (unsigned long)GetDesktopWindow());
	}

	char	encbuf[12];

	for (int i = 0; i < 12; i++)
	{
		encbuf[i]	= (char)((rand() >> 7) & 0xff);
	}

	encbuf[11]	= (char)((zfi.tim >> 8) & 0xff);

	for (int ei = 0; ei < 12; ei++)
	{
		encbuf[ei]	= zencode(keys, encbuf[ei]);
	}

	if (password != 0 && !isdir)
	{
		swrite(this, encbuf, 12);

		writ	+= 12;
	}

	ZRESULT	writeres	= ZR_OK;

	encwriting	= (password != 0 && !isdir);

	if (!isdir && method == DEFLATE)
	{
		writeres	= ideflate(&zfi);
	}
	else if (!isdir && method == STORE)
	{
		writeres	= istore();
	}
	else if (isdir)
	{
		csize	= 0;
	}

	encwriting	= false;

	iclose();

	writ	+= csize;

	if (oerr != ZR_OK)
	{
		return oerr;
	}

	if (writeres != ZR_OK)
	{
		return ZR_WRITE;
	}

	bool	first_header_has_size_right	= (zfi.siz == csize + passex);

	zfi.crc	= crc;
	zfi.siz	= csize + passex;
	zfi.len	= isize;

	if (ocanseek && (password == 0 || isdir))
	{
		zfi.how	= (ush)method;

		if ((zfi.flg & 1) == 0)
		{
			zfi.flg	&= ~8;
		}

		zfi.lflg	= zfi.flg;

		if (!oseek(zfi.off - ooffset))
		{
			return ZR_SEEK;
		}

		if ((r = putlocal(&zfi, swrite, this)) != ZE_OK)
		{
			return ZR_WRITE;
		}

		if (!oseek(writ))
		{
			return ZR_SEEK;
		}
	}
	else
	{
		if (zfi.how != (ush)method)
		{
			return ZR_NOCHANGE;
		}

		if (method == STORE && !first_header_has_size_right)
		{
			return ZR_NOCHANGE;
		}

		if ((r = putextended(&zfi, swrite, this)) != ZE_OK)
		{
			return ZR_WRITE;
		}

		writ	+= 16L;
		zfi.flg	= zfi.lflg;
	}

	if (oerr != ZR_OK)
	{
		return oerr;
	}

	char*	cextra	= new char[zfi.cext];

	memcpy(cextra, zfi.cextra, zfi.cext);

	zfi.cextra	= cextra;

	TZipFileInfo*	pzfi	= new TZipFileInfo;

	memcpy(pzfi, &zfi, sizeof(zfi));

	if (zfis == NULL)
	{
		zfis	= pzfi;
	}
	else
	{
		TZipFileInfo*	z	= zfis;

		while (z->nxt != NULL)
		{
			z	= z->nxt;
		}

		z->nxt	= pzfi;
	}

	return ZR_OK;
}

ZRESULT TZip::AddCentral()
{
	int		numentries				= 0;
	ulg		pos_at_start_of_central	= writ;
	bool	okay					= true;

	for (TZipFileInfo* zfi = zfis; zfi !=NULL; )
	{
		if (okay)
		{
			int	res	= putcentral(zfi, swrite, this);

			if (res != ZE_OK)
			{
				okay	= false;
			}
		}

		writ	+= 4 + CENHEAD + (unsigned int)zfi->nam + (unsigned int)zfi->cext + (unsigned int)zfi->com;
		numentries++;

		TZipFileInfo*	zfinext	= zfi->nxt;

		if (zfi->cextra != 0)
		{
			delete[] zfi->cextra;
		}

		delete zfi;

		zfi	= zfinext;
	}

	ulg	center_size	= writ - pos_at_start_of_central;

	if (okay)
	{
		int	res	= putend(numentries, center_size, pos_at_start_of_central + ooffset, 0, NULL, swrite, this);

		if (res != ZE_OK)
		{
			okay	= false;
		}

		writ	+= 4 + ENDHEAD + 0;
	}

	if (!okay)
	{
		return ZR_WRITE;
	}

	return ZR_OK;
}

ZRESULT	lasterrorZ	= ZR_OK;

unsigned int FormatZipMessageZ(ZRESULT code, TCHAR* buf, unsigned int len)
{
	if (code == ZR_RECENT)
	{
		code	= lasterrorZ;
	}

	const TCHAR*	msg	= _T("unknown zip result code");

	switch (code)
	{
	case ZR_OK:
		msg	= _T("Success");
		break;
	case ZR_NODUPH:
		msg	= _T("Culdn't duplicate handle");
		break;
	case ZR_NOFILE:
		msg	= _T("Couldn't create/open file");
		break;
	case ZR_NOALLOC:
		msg	= _T("Failed to allocate memory");
		break;
	case ZR_WRITE:
		msg	= _T("Error writing to file");
		break;
	case ZR_NOTFOUND:
		msg	= _T("File not found in the zipfile");
		break;
	case ZR_MORE:
		msg	= _T("Still more data to unzip");
		break;
	case ZR_CORRUPT:
		msg	= _T("Zipfile is corrupt or not a zipfile");
		break;
	case ZR_READ:
		msg	= _T("Error reading file");
		break;
	case ZR_ARGS:
		msg	= _T("Caller: faulty arguments");
		break;
	case ZR_PARTIALUNZ:
		msg	= _T("Caller: the file had already been partially unzipped");
		break;
	case ZR_NOTMMAP:
		msg	= _T("Caller: can only get memory of a memory zipfile");
		break;
	case ZR_MEMSIZE:
		msg	= _T("Caller: not enough space allocated for memory zipfile");
		break;
	case ZR_FAILED:
		msg	= _T("Caller: there was a previous error");
		break;
	case ZR_ENDED:
		msg	= _T("Caller: additions to the zip have already been ended");
		break;
	case ZR_ZMODE:
		msg	= _T("Caller: mixing creation and opening of zip");
		break;
	case ZR_NOTINITED:
		msg	= _T("Zip-bug: internal initialisation not completed");
		break;
	case ZR_SEEK:
		msg	= _T("Zip-bug: trying to seek the unseekable");
		break;
	case ZR_MISSIZE:
		msg	= _T("Zip-bug: the anticipated size turned out wrong");
		break;
	case ZR_NOCHANGE:
		msg	= _T("Zip-bug: tried to change mind, but not allowed");
		break;
	case ZR_FLATE:
		msg	= _T("Zip-bug: an internal error during flation");
		break;
	}

	unsigned int	mlen	= (unsigned int)_tcslen(msg);

	if (buf == 0 || len == 0)
	{
		return mlen;
	}

	unsigned int	n	= mlen;

	if (n + 1 > len)
	{
		n	= len - 1;
	}

	_tcsncpy(buf, msg, n);

	buf[n]	= 0;

	return mlen;
}

typedef	struct
{
	DWORD	flag;
	TZip*	zip;
} TZipHandleData;

HZIP CreateZipInternal(void* z, unsigned int len, DWORD flags, const char* password)
{
	TZip*	zip	= new TZip(password);

	lasterrorZ	= zip->Create(z, len, flags);

	if (lasterrorZ != ZR_OK)
	{
		delete zip;
		return 0;
	}

	TZipHandleData*	han	= new TZipHandleData;

	han->flag	= 2;
	han->zip	= zip;

	return (HZIP)han;
}

HZIP CreateZipHandle(HANDLE h, const char* password)
{
	return CreateZipInternal(h, 0, ZIP_HANDLE, password);
}

HZIP CreateZip(const TCHAR* fn, const char* password)
{
	return CreateZipInternal((void*)fn, 0, ZIP_FILENAME, password);
}

HZIP CreateZip(void* z,unsigned int len, const char* password)
{
	return CreateZipInternal(z, len, ZIP_MEMORY, password);
}

ZRESULT ZipAddInternal(HZIP hz, const TCHAR* dstzn, void* src, unsigned int len, DWORD flags)
{
	if (hz == 0)
	{
		lasterrorZ	= ZR_ARGS;

		return ZR_ARGS;
	}

	TZipHandleData*	han	= (TZipHandleData*)hz;

	if (han->flag != 2)
	{
		lasterrorZ	= ZR_ZMODE;

		return ZR_ZMODE;
	}

	TZip*	zip	= han->zip;

	lasterrorZ	= zip->Add(dstzn, src, len, flags);

	return lasterrorZ;
}

ZRESULT ZipAdd(HZIP hz, const TCHAR* dstzn, const TCHAR* fn)
{
	return ZipAddInternal(hz, dstzn, (void*)fn, 0, ZIP_FILENAME);
}

ZRESULT ZipAdd(HZIP hz,const TCHAR* dstzn, void* src, unsigned int len)
{
	return ZipAddInternal(hz, dstzn, src, len, ZIP_MEMORY);
}

ZRESULT ZipAddHandle(HZIP hz, const TCHAR* dstzn, HANDLE h)
{
	return ZipAddInternal(hz, dstzn, h, 0, ZIP_HANDLE);
}

ZRESULT ZipAddHandle(HZIP hz, const TCHAR* dstzn, HANDLE h, unsigned int len)
{
	return ZipAddInternal(hz, dstzn, h, len, ZIP_HANDLE);
}

ZRESULT ZipAddFolder(HZIP hz, const TCHAR* dstzn)
{
	return ZipAddInternal(hz, dstzn, 0, 0, ZIP_FOLDER);
}

ZRESULT ZipGetMemory(HZIP hz, void** buf, unsigned long* len)
{
	if (hz == 0)
	{
		if (buf != 0)
		{
			*buf	= 0;
		}

		if (len != 0)
		{
			*len	= 0;
		}

		lasterrorZ	= ZR_ARGS;

		return ZR_ARGS;
	}

	TZipHandleData*	han	= (TZipHandleData*)hz;

	if (han->flag != 2)
	{
		lasterrorZ	= ZR_ZMODE;

		return ZR_ZMODE;
	}

	TZip*	zip	= han->zip;

	lasterrorZ	= zip->GetMemory(buf, len);

	return lasterrorZ;
}

ZRESULT CloseZipZ(HZIP hz)
{
	if (hz == 0)
	{
		lasterrorZ	= ZR_ARGS;

		return ZR_ARGS;
	}

	TZipHandleData* han	= (TZipHandleData*)hz;

	if (han->flag != 2)
	{
		lasterrorZ	= ZR_ZMODE;

		return ZR_ZMODE;
	}

	TZip* zip	= han->zip;

	lasterrorZ	= zip->Close();

	delete zip;
	delete han;

	hz	= 0;

	return lasterrorZ;
}

bool IsZipHandleZ(HZIP hz)
{
	if (hz == 0)
	{
		return false;
	}

	TZipHandleData*	han	= (TZipHandleData*)hz;

	return (han->flag == 2);
}
