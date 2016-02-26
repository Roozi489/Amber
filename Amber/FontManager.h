#pragma once

typedef struct FontType_
{
	const char*  filepathname;
	int          face_index;
	int          cmap_index;
	int          num_indices;
	void*        file_address;  // for preloaded files - what is this???
	size_t       file_size;

} FontType, *FontHandle;

class FontManager
{
public:
	FontManager();
	~FontManager();
};

