#pragma once

#include <memory>
#include <string>
#include <vector>
#include <chrono>

static std::string get_key_name(int vk);
class c_menu {
	int m_selected_tab = 0;
	bool m_style_initialized = false;
	float m_dpi_scale = 1.0f;

public:
	bool m_opened = true;

	void draw();
	void rebuild_fonts(float scale);

	bool is_open() const { return m_opened; }
	float get_dpi_scale() const { return m_dpi_scale; }
	void set_dpi_scale(float scale) { m_dpi_scale = scale; }

private:
	void setup_style();
};

inline const auto g_menu = std::make_unique<c_menu>();
