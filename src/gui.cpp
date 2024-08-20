#include <ftxui/component/screen_interactive.hpp>
#include <iostream>

#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>

int main()
{
	auto main_screen = ftxui::ScreenInteractive::Fullscreen();

	std::cout << "\033[2J\033[1;1H";

  	int result_size = ftxui::Dimension::Full().dimy - 4;
  	int flags_size = ftxui::Dimension::Full().dimx - 40;

	auto window_flags = ftxui::Renderer([] 
	{ 
		return ftxui::text("middle") | ftxui::center; }
	);
	auto window_desciption = ftxui::Renderer([] 
	{ 
		return ftxui::text("middle") | ftxui::center; 
	});
	auto window_result = ftxui::Renderer([] 
	{ 
		return ftxui::text(" Result: ") | ftxui::bold; }
	);
 
  	auto main_window = window_flags;
  	main_window = ftxui::ResizableSplitRight(
		ftxui::ResizableSplitTop(window_desciption, window_result, &result_size),  
		main_window, &flags_size
	);

	auto test_renderer = ftxui::Renderer(main_window, [&] { return main_window->Render() | ftxui::border; });
	main_screen.Loop(test_renderer);
}
