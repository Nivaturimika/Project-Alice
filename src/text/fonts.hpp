#pragma once

//#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"
#include "unordered_dense.h"
#include "bmfont.hpp"
#include "hb.h"

namespace sys {
struct state;
}

namespace text {

inline constexpr uint32_t max_texture_layers = 256;

uint16_t name_into_font_id(sys::state& state, std::string_view text);
int32_t size_from_font_id(uint16_t id);
bool is_black_from_font_id(uint16_t id);
uint32_t font_index_from_font_id(sys::state& state, uint16_t id);

struct glyph_sub_offset {
	float x = 0.0f;
	float y = 0.0f;
};

class font_manager;


enum class font_feature {
	none, small_caps
};

struct cached_text_entry {
	unsigned int glyph_count = 0;
	std::vector<hb_glyph_info_t> glyph_info;
	std::vector<hb_glyph_position_t> glyph_pos;
};

class font {
private:
	font(font const&) = delete;
	font(font&&) = default;
	font& operator=(font const&) = delete;
	font& operator=(font&&) noexcept = default;
	font() = default;

public:
	ankerl::unordered_dense::map<std::string, cached_text_entry> cached_text;
	FT_Face font_face;
	hb_font_t* hb_font_face = nullptr;
	uint8_t const* gs = nullptr;
	hb_buffer_t* hb_buf = nullptr;
	hb_feature_t hb_features[1];
	unsigned int num_features = 0;

	float internal_line_height = 0.0f;
	float internal_ascender = 0.0f;
	float internal_descender = 0.0f;
	float internal_top_adj = 0.0f;

	font_feature features = font_feature::none;
	bool loaded = false;
	bool convert_win1252 = false;

	uint32_t texture_array = 0;
	ankerl::unordered_dense::map<char32_t, float> glyph_advances;
	ankerl::unordered_dense::map<char32_t, bool> glyph_loaded;
	ankerl::unordered_dense::map<char32_t, glyph_sub_offset> glyph_positions;

	std::unique_ptr<FT_Byte[]> file_data;

	~font();

	char codepoint_to_alnum(char32_t codepoint);
	bool can_display(char32_t ch_in) const;
	std::string get_conditional_indicator(bool v) const;
	void make_glyph(char32_t ch_in);
	float line_height(int32_t size) const;
	float ascender(int32_t size) const;
	float descender(int32_t size) const;
	float top_adjustment(int32_t size) const;
	float text_extent(sys::state& state, char const* codepoints, uint32_t count, int32_t size);
	decltype(cached_text)::iterator get_cached_glyphs(char const* codepoints, uint32_t count);

	friend class font_manager;
};

class font_manager {
public:
	font_manager();
	~font_manager();

	ankerl::unordered_dense::map<uint16_t, dcon::text_key> font_names;
	ankerl::unordered_dense::map<uint16_t, bm_font> bitmap_fonts;
	FT_Library ft_library;
	font fonts[12];
	bool map_font_is_black = false;

	void load_font(font& fnt, char const* file_data, uint32_t file_size, font_feature f);
	void load_all_glyphs();

	float line_height(sys::state& state, uint16_t font_id) const;
	float text_extent(sys::state& state, char const* codepoints, uint32_t count, uint16_t font_id);
};

void load_standard_fonts(sys::state& state);
void load_bmfonts(sys::state& state);
} // namespace text
