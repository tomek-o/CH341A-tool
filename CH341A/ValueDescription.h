//---------------------------------------------------------------------------

#ifndef ValueDescriptionH
#define ValueDescriptionH
//---------------------------------------------------------------------------

#include <stdint.h>
#include <assert.h>

struct ValueDescriptionU8
{
	uint8_t value;
	const char* description;
};

namespace Stdctrls
{
	class TComboBox;
}

template<typename T, int size>
void FillComboboxWithValues(T(&vds)[size], Stdctrls::TComboBox *cb, int defaultValue)
{
	cb->Items->Clear();
	for (int i=0; i<size; i++)
	{
		cb->Items->Add(vds[i].description);
		if (vds[i].value == defaultValue)
		{
			cb->ItemIndex = i;
		}
	}
	assert(cb->ItemIndex >= 0);
}


#endif
