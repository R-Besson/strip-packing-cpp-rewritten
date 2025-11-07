/**==============================================
 * ?                    ABOUT
 * @author      : Romain BESSON
 * @description : Visualizer code for viewing a Result
 *=============================================**/

#include <cmath>
#include <iostream>
#include <algorithm>

#include "visualizer.h"
#include <SFML/Graphics.hpp>

void window_rectangle(sf::Vector2f pos, sf::Vector2f size, sf::Color c, int outline, sf::Color oc, sf::RenderWindow &win)
{
	sf::Vector2u windowSize = win.getSize();
	if (pos.x + size.x < 0 || pos.x > windowSize.x ||
		pos.y + size.y < 0 || pos.y > windowSize.y)
	{
		return;
	}

	sf::RectangleShape rect;
	rect.setPosition(pos);
	rect.setSize(size);
	rect.setFillColor(c);
	rect.setOutlineThickness(static_cast<float>(outline));
	rect.setOutlineColor(oc);
	win.draw(rect);
}

enum class TextOrigin
{
	TL,
	TR,
	BL,
	BR,
	C
};

sf::Text window_text(const std::string &s, sf::Vector2f pos, int size, TextOrigin origin, sf::Font &f, sf::RenderWindow &win)
{
	sf::Text text;
	text.setFont(f);
	text.setString(s);
	text.setCharacterSize(size);
	text.setFillColor(sf::Color::Black);

	sf::FloatRect bounds = text.getLocalBounds();

	switch (origin)
	{
	case TextOrigin::TR:
		text.setOrigin(std::round(bounds.left + bounds.width), std::round(bounds.top));
		break;
	case TextOrigin::BL:
		text.setOrigin(std::round(bounds.left), std::round(bounds.top + bounds.height));
		break;
	case TextOrigin::BR:
		text.setOrigin(std::round(bounds.left + bounds.width), std::round(bounds.top + bounds.height));
		break;
	case TextOrigin::C:
		text.setOrigin(std::round(bounds.left + bounds.width / 2.f), std::round(bounds.top + bounds.height / 2.f));
		break;
	case TextOrigin::TL:
	default:
		text.setOrigin(std::round(bounds.left), std::round(bounds.top));
		break;
	}

	pos.x = std::round(pos.x);
	pos.y = std::round(pos.y);

	text.setPosition(pos);
	win.draw(text);

	return text;
}

sf::Vector2f to_screen(sf::Vector2f world_pos, sf::Vector2f pivot, sf::Vector2f offset, float zoom)
{
	float screen_x = (world_pos.x + offset.x - pivot.x) * zoom + pivot.x;
	float screen_y = (world_pos.y + offset.y - pivot.y) * zoom + pivot.y;
	return sf::Vector2f(screen_x, screen_y);
}

void visualize(const Result &result, uint32_t ww, uint32_t hh, const std::string &font_path)
{
	constexpr int PAN_SPEED = 20;
	constexpr float ZOOM_FACTOR = 0.05f;
	constexpr float MIN_ZOOM = 0.01f;
	constexpr int FONT_SIZE_INFO = 14;
	constexpr int FONT_SIZE_ID = 12;
	constexpr float UI_PADDING = 5.f;
	constexpr float UI_WIDTH = 195.f;

	sf::RenderWindow window(sf::VideoMode(ww, hh), "Packed!");
	window.setFramerateLimit(60);

	sf::Font font;
	if (!font.loadFromFile(font_path))
	{
		std::cerr << "Error: Could not load font from " << font_path << std::endl;
		return;
	}

	sf::Vector2f offset(30.f, 30.f);
	float zoom = 1.0f;
	bool show_ids = true;
	bool show_controls = true;

	bool needs_redraw = true;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::Resized)
			{
				sf::View view(sf::FloatRect(0.f, 0.f, static_cast<float>(event.size.width), static_cast<float>(event.size.height)));
				window.setView(view);
				needs_redraw = true;
			}

			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					offset.y += PAN_SPEED / zoom;
					break;
				case sf::Keyboard::Down:
					offset.y -= PAN_SPEED / zoom;
					break;
				case sf::Keyboard::Left:
					offset.x += PAN_SPEED / zoom;
					break;
				case sf::Keyboard::Right:
					offset.x -= PAN_SPEED / zoom;
					break;
				case sf::Keyboard::P:
					zoom += zoom * ZOOM_FACTOR;
					break;
				case sf::Keyboard::M:
					zoom -= zoom * ZOOM_FACTOR;
					break;
				case sf::Keyboard::I:
					show_ids = !show_ids;
					break;
				case sf::Keyboard::C:
					show_controls = !show_controls;
					break;
				case sf::Keyboard::Q:
					window.close();
					break;
				default:
					continue;
				}
				zoom = std::max(MIN_ZOOM, zoom);
				needs_redraw = true;
			}
		}

		if (needs_redraw)
		{
			needs_redraw = false;

			window.clear(sf::Color::White);

			const sf::Vector2f window_size(window.getSize());
			const sf::Vector2f pivot(window_size.x / 2.f, window_size.y / 2.f);

			const sf::Vector2f frame_pos = to_screen({0.f, 0.f}, pivot, offset, zoom);
			const sf::Vector2f frame_size(result.w * zoom, result.h * zoom);

			window_rectangle(frame_pos,
							 {frame_size.x > 1 ? frame_size.x - 1 : 0, frame_size.y > 1 ? frame_size.y - 1 : 0},
							 sf::Color::Transparent, 1, sf::Color::Black, window);

			const float LABEL_OFFSET = 16.f;
			window_text("(0, 0)", frame_pos - sf::Vector2f(0, LABEL_OFFSET), FONT_SIZE_INFO, TextOrigin::C, font, window);
			window_text("(" + std::to_string(result.w) + ", 0)", frame_pos + sf::Vector2f(frame_size.x, -LABEL_OFFSET), FONT_SIZE_INFO, TextOrigin::C, font, window);
			window_text("(0, " + std::to_string(result.h) + ")", frame_pos + sf::Vector2f(0, frame_size.y + LABEL_OFFSET), FONT_SIZE_INFO, TextOrigin::C, font, window);
			window_text("(" + std::to_string(result.w) + ", " + std::to_string(result.h) + ")", frame_pos + sf::Vector2f(frame_size.x, frame_size.y + LABEL_OFFSET), FONT_SIZE_INFO, TextOrigin::C, font, window);

			for (const Shape &rectangle : result.rectangles)
			{
				const sf::Vector2f rect_pos = to_screen({float(rectangle.x()), float(rectangle.y())}, pivot, offset, zoom);
				const sf::Vector2f rect_size(rectangle.w() * zoom, rectangle.h() * zoom);

				if (rect_pos.x + rect_size.x < 0 || rect_pos.x > window_size.x ||
					rect_pos.y + rect_size.y < 0 || rect_pos.y > window_size.y)
				{
					continue;
				}

				window_rectangle(rect_pos,
								 {rect_size.x > 1 ? rect_size.x - 1 : 0, rect_size.y > 1 ? rect_size.y - 1 : 0},
								 sf::Color(215, 215, 215), 1, sf::Color::Black, window);

				if (show_ids && rect_size.x > 15 && rect_size.y > 15)
				{
					window_text(std::to_string(rectangle.id()),
								rect_pos + (rect_size / 2.f),
								FONT_SIZE_ID, TextOrigin::C, font, window);
				}
			}

			window_rectangle({pivot.x - 5.f, pivot.y - 1.f}, {10.f, 2.f}, sf::Color(0, 0, 0, 150), 0, sf::Color::Black, window);
			window_rectangle({pivot.x - 1.f, pivot.y - 5.f}, {2.f, 10.f}, sf::Color(0, 0, 0, 150), 0, sf::Color::Black, window);

			sf::Vector2f info_panel_pos(window_size.x - UI_WIDTH - 20.f, window_size.y - 20.f);

			std::string info_string = "W = " + std::to_string(result.w) + "\nH = " + std::to_string(result.h) + "\nOptimal H = " + std::to_string(result.opt_h) + "\nH/Opt.H = " + std::to_string(static_cast<float>(result.h) / static_cast<float>(result.opt_h)) + "\nLoss = " + std::to_string(result.loss) + "%";
			sf::Text info_text = window_text(info_string, info_panel_pos, FONT_SIZE_INFO, TextOrigin::BL, font, window);

			sf::FloatRect info_bounds = info_text.getGlobalBounds();
			window_rectangle({info_bounds.left - UI_PADDING, info_bounds.top - UI_PADDING},
							 {UI_WIDTH, info_bounds.height + 2 * UI_PADDING},
							 sf::Color::Transparent, 1, sf::Color::Black, window);

			if (show_controls)
			{
				std::string controls_string = "p - Zoom in\nm - Zoom out\narrows - Move\ni - Show/Hide id's\nc - Show/Hide controls\nq - Quit";
				sf::Text controls_text = window_text(controls_string,
													 {info_bounds.left, info_bounds.top - 2 * UI_PADDING},
													 FONT_SIZE_INFO, TextOrigin::BL, font, window);

				sf::FloatRect controls_bounds = controls_text.getGlobalBounds();
				window_rectangle({controls_bounds.left - UI_PADDING, controls_bounds.top - UI_PADDING},
								 {UI_WIDTH, controls_bounds.height + 2 * UI_PADDING},
								 sf::Color::Transparent, 1, sf::Color::Black, window);
			}

			window.display();
		}
	}
}