#ifndef ArraySizeH
#define ArraySizeH

#ifdef __cplusplus

template<typename T, int size> int ARRAY_SIZE(T(&)[size])
{
	return size;
}

#else

#define ARRAY_SIZE(a) ((sizeof(a))/(sizeof((a)[0])))

#endif

#endif