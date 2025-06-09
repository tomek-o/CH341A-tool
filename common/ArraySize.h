#ifndef ArraySizeH
#define ArraySizeH

#ifdef __cplusplus

template<typename T, const unsigned int size> unsigned int ARRAY_SIZE(const T(&)[size])
{
	return size;
}

#define C_ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

#else

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

#endif

#endif