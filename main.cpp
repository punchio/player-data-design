#include <vector>
class MG_INPUT_CDR
{
private:
	const char*	m_pBuff;
	size_t	m_tBuffSize;
	size_t	m_tBuffCur;

public:
	MG_INPUT_CDR(void)
	{
		m_pBuff = nullptr;
		m_tBuffSize = 0;
		m_tBuffCur = 0;
	}

	MG_INPUT_CDR(const char* pBuff, size_t tBuffSize)
	{
		m_pBuff = pBuff;
		m_tBuffSize = tBuffSize;
		m_tBuffCur = 0;
	}

	~MG_INPUT_CDR(void)
	{

	}

public:
	// 初始化，返回true为初始化成功
	bool Init(const char* pBuff, size_t tBuffSize)
	{
		if (nullptr == pBuff)
		{
			return false;
		}
		else
		{
			m_pBuff = pBuff;
			m_tBuffSize = tBuffSize;
			m_tBuffCur = 0;

			return true;
		}
	}

public:
	inline bool operator >> (char& x)
	{
		if ((m_tBuffSize - m_tBuffCur) < sizeof(char))
		{
			return false;
		}
		else
		{
			x = (char)(*(m_pBuff + m_tBuffCur));
			++m_tBuffCur;

			return true;
		}
	}
};


class MG_OUTPUT_CDR
{
private:
	char*	m_pBuff;
	size_t	m_tBuffSize;
	size_t	m_tBuffCur;

public:
	MG_OUTPUT_CDR(void)
	{
		m_pBuff = nullptr;
		m_tBuffSize = 0;
		m_tBuffCur = 0;
	}

	MG_OUTPUT_CDR(char* pBuff, size_t tBuffSize)
	{
		m_pBuff = pBuff;
		m_tBuffSize = tBuffSize;
		m_tBuffCur = 0;
	}

	~MG_OUTPUT_CDR(void)
	{

	}

public:
	// 初始化，返回true为初始化成功
	bool Init(char* pBuff, size_t tBuffSize)
	{
		if (nullptr == pBuff)
		{
			return false;
		}
		else
		{
			m_pBuff = pBuff;
			m_tBuffSize = tBuffSize;
			m_tBuffCur = 0;

			return true;
		}
	}

public:
	// 编解码函数，返回true为成功，返回false为失败
	inline bool operator << (char x) 
	{
		if ((m_tBuffSize - m_tBuffCur) < sizeof(char))
		{
			return false;
		}
		else
		{
			*(m_pBuff + m_tBuffCur) = (char)x;
			++m_tBuffCur;
			return true;
		}
	}
};

struct d_base
{
public:
	d_base(d_base* parent, char index)
	{
		this->index = index > 0 ? index : -index;
		if (parent != nullptr)
		{
			parent->decode_items.push_back(this);
			if (index > 0)
			{
				parent->encode_items.push_back(this);
			}
		}
	}

	virtual bool encode(MG_OUTPUT_CDR& out)
	{
		out << this->index;
		for (size_t i = 0; i < encode_items.size(); i++)
		{
			encode_items[i]->encode(out);
		}
		out << 0xff;
		return true;
	}

	virtual bool decode(MG_INPUT_CDR& in, char index)
	{
		in >> index;
		for (size_t i = 0; i < decode_items.size(); i++)
		{
			if (decode_items[i]->decode(in, index))
			{
				in >> index;
			}
		}
		return (unsigned char)index == 0xff;
	}

protected:
	char index;
	std::vector<d_base*> encode_items;
	std::vector<d_base*> decode_items;
};

struct c_int : public d_base
{
	c_int(d_base* parent, char index, char value) : d_base(parent, index)
	{
		this->value = value;
	}

	virtual bool encode(MG_OUTPUT_CDR& in)
	{
		in << this->index;
		in << this->value;
		return true;
	}

	virtual bool decode(MG_INPUT_CDR& out, char index)
	{
		if (this->index == index)
		{
			out >> this->value;
			return true;
		}
		return false;
	}

	char operator+(c_int& other)
	{
		return this->value + other.value;
	}

	char operator+(char& other)
	{
		return this->value + other;
	}

public:
	char value;
};

struct stData1 : public d_base
{
	stData1(d_base* parent, char index) : d_base(parent, index), d1(this, -1, 0), d2(this, 2, 0)
	{
	}

	//bool encode(MG_OUTPUT_CDR& in)
	//{
	//	in << this->index;
	//	d1.encode(in);
	//	//d2.encode(in);
	//	in << 0xff;
	//	return true;
	//}

	//bool decode(MG_INPUT_CDR& out, char index)
	//{
	//	if (this->index == index)
	//	{
	//		out >> index;
	//		if (index != 0xff && d1.decode(out, index))
	//		{
	//			out >> index;
	//		}
	//		
	//		if (index != 0xff && d2.decode(out, index))
	//		{
	//			out >> index;
	//		}
	//		return true;
	//	}
	//	return false;
	//}

public:
	c_int d1;
	c_int d2;
};

struct stData2 : public d_base
{
	stData2(d_base* parent, char index) : d_base(parent, index), d1(this, 1), d2(this, 2, 0)
	{
	}

	//bool encode(MG_OUTPUT_CDR& in)
	//{
	//	in << this->index;
	//	d1.encode(in);
	//	d2.encode(in);
	//	in << 0xff;
	//	return true;
	//}

	//bool decode(MG_INPUT_CDR& out, char index)
	//{
	//	if (this->index == index)
	//	{
	//		out >> index;
	//		if (index != 0xff && d1.decode(out, index))
	//		{
	//			out >> index;
	//		}

	//		if (index != 0xff && d2.decode(out, index))
	//		{
	//			out >> index;
	//		}

	//		return true;
	//	}
	//	return false;
	//}

public:
	stData1 d1;
	c_int d2;
};

//struct stData3
//{
//	stData3(char index) :s1(1), s2(2), d3(3, 0)
//	{
//		this->index = index;
//	}
//
//	bool encode(MG_OUTPUT_CDR& in)
//	{
//		in << this->index;
//		s1.encode(in);
//		s2.encode(in);
//		d3.encode(in);
//		return true;
//	}
//
//	bool decode(MG_INPUT_CDR& out, char index)
//	{
//		if (this->index == index)
//		{
//			out >> index;
//			if (s1.decode(out, index))
//			{
//				out >> index;
//			}
//
//			if (s2.decode(out, index))
//			{
//				out >> index;
//			}
//
//			if (d3.decode(out, index))
//			{
//				out >> index;
//			}
//			return true;
//		}
//
//		return false;
//	}
//
//public:
//	stData1 s1;
//	stData2 s2;
//	c_int d3;
//
//private:
//	char index;
//};

int main()
{
	MG_OUTPUT_CDR encode;
	MG_INPUT_CDR decode;
	char buff[1000] = { 0 };
	encode.Init(buff, 1000);
	decode.Init(buff, 1000);

	//stData3 d3(1);
	//d3.s1.d1.value = 11;
	//d3.s1.d2.value = 12;

	//d3.s2.d1.value = 21;
	//d3.d3.value = d3.s1.d1 + d3.s2.d1;

	//d3.encode(encode);

	//stData3 d4(1);
	//d4.decode(decode, 1);

	//stData1 d1(1);
	//d1.d1.value = 10;
	//d1.d2.value = 11;
	//d1.encode(encode);

	//stData1 d2(1);
	//char index;
	//decode >> index;
	//d2.decode(decode, index);

	stData2 d2(nullptr, 1);
	d2.d1.d1.value = 10;
	d2.d1.d2.value = 11;
	d2.d2.value = 20;

	d2.encode(encode);

	stData2 d3(nullptr, 1);
	char index;
	decode >> index;
	if (index == 1)
	{
		d3.decode(decode, index);
	}

	return 0;
}