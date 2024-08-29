#include <iostream>
#include <string>

#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

#include <ftxui/dom/direction.hpp>
#include <ftxui/dom/elements.hpp>

#include <ftxui/screen/screen.hpp>

#include "core.h"

int main(int argc, char *argv[])
{
	auto main_screen = ftxui::ScreenInteractive::Fullscreen();

	std::cout << "\033[2J\033[1;1H";

  	int result_size = 1;
  	int flags_size = 25;

	auto window_flags = ftxui::Renderer([&flags_size, argc, argv]
	{ 
		stData arrData; 
		GetOptions(&arrData, argv[argc - 1]);
		return ftxui::text(std::to_string(arrData.dataFlags[1])) | ftxui::center;
	});

	auto window_desciption = ftxui::Renderer([]
	{ 
		return ftxui::text("") | ftxui::center; 
	});

	auto window_result = ftxui::Renderer([&result_size] 
	{ 
		return ftxui::text(" Result: " + std::to_string(result_size)) | ftxui::bold; 
	});

  	auto layout_window = ftxui::ResizableSplit(
		ftxui::ResizableSplitOption({
			.main = window_result,
			.back = window_desciption,
			.direction = ftxui::Direction::Down,
			.main_size = &result_size,
			.separator_func = []{ return ftxui::separatorDouble(); },
		})
	);

  	auto main_window = ftxui::ResizableSplit(
		ftxui::ResizableSplitOption({
			.main = window_flags,
			.back = layout_window,
			.direction = ftxui::Direction::Left,
			.main_size = &flags_size,
			.separator_func = []{ return ftxui::separatorDouble(); },
		})
	);

	auto test_renderer = ftxui::Renderer(main_window, [&]{ 
		return main_window->Render() | ftxui::border; 
	});

	test_renderer |= ftxui::CatchEvent([&](ftxui::Event _event){
		if (_event.is_mouse())
		{
			if (flags_size < 25)
			{
				flags_size = 25;
				return true;
			}
			
			if (result_size < 1)
			{
				result_size = 1;
				return true;
			}
		}
		return false;
	});

	main_screen.Loop(test_renderer);
}
