#define GLM_ENABLE_EXPERIMENTAL true
//#include "gl_utils.h"		// utility functions discussed in earlier tutorials
#include <GL/glew.h>		// include GLEW and new version of GL on Windows
//#include <GLFW/glfw3.h> // GLFW helper library
//#include <assert.h>
//#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <GLM/mat4x4.hpp>
//#include <GLM/gtx/transform.hpp>
//#include <GLM/gtc/type_ptr.hpp>
//#include <GLM/gtc/matrix_access.hpp>
//#include <GLM/gtx/string_cast.hpp>
//#include "Primitives\ShapeGenerator.h"
//#define GL_LOG_FILE "gl.log"
//#define MY_PI 3.14159265358979323846
//#define ONE_DEG_IN_RAD ( 2.0 * MY_PI ) / 360.0 // 0.017444444
#include "MyGlCanvas1.h"


#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/glcanvas.h>


#include <NANOGUI/nanogui.h>
#include <iomanip>
#include <sstream>


#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

class App: public nanogui::Screen {
public:
	App() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test", false) {
		using namespace nanogui;

		Window *window = new Window(this, "Star Wars");
		window->setPosition(Vector2i(350, 15));
		window->setLayout(new GroupLayout());
		
		mCanvas = new MyGLCanvas(window);
		mCanvas->setBackgroundColor({ 100, 100, 100, 255 });
		mCanvas->setSize({ 600, 400 });

		window = new Window(this, "Info");
		Label *info = new Label(window, "Info", "sans-bold");
		window->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 15));
		window->setPosition(Vector2i(15, 500));
		info->setFixedWidth(800);
		info->setFixedHeight(200);
		mCanvas->infoBox = info;


		window = new Window(this, "Controls");
		window->setPosition(Vector2i(15, 15));
		GridLayout *layout = new GridLayout(Orientation::Horizontal, 2,	Alignment::Middle, 15, 5);
		layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
		layout->setSpacing(0, 10);
		window->setLayout(layout);

		new Label(window, "Points :", "sans-bold");
		TextBox *textBox = new TextBox(window);
		textBox->setEditable(false);
		textBox->setFixedSize(Vector2i(100, 20));
		textBox->setValue("0");
		mCanvas->pointBox = textBox;

		new Label(window, "Alien Qty.:", "sans-bold");
		ComboBox *combo = new ComboBox(window, { "12", "14", "16", "18", "20" });
		combo->setFontSize(16);
		combo->setFixedSize(Vector2i(100, 20));
		combo->setCallback([this](int id) {
			int value = 12;
			switch (id)
			{
				case 0: value = 12; break;
				case 1: value = 14; break;
				case 2: value = 16; break;
				case 3: value = 18; break;
				case 4: value = 20; break;
				default: value = 12; break;
			}
			mCanvas->setAlienQty(value);
		});
		mCanvas->alienQtyBomboBox = combo;
		
		new Label(window, "Alien Vel.:", "sans-bold");
		Widget *panel = new Widget(window);
		panel->setLayout(new BoxLayout(Orientation::Horizontal,	Alignment::Middle, 0, 20));
		Slider *slider = new Slider(panel);
		slider->setValue(0.5f);
		slider->setFixedWidth(100);
		slider->setRange({ 0.0f, 3.0f });

		TextBox *textBox1 = new TextBox(panel);
		slider->setCallback([textBox1, this](float value) {
			std::ostringstream buff;
			buff << std::setprecision(2) << value;
			textBox1->setValue(buff.str());
			mCanvas->setAlienSpeed(value);
		});
		mCanvas->speedSlider = slider;

		textBox1->setFixedSize(Vector2i(60, 25));
		textBox1->setValue("1.0");
		textBox1->setFixedSize(Vector2i(60, 25));
		textBox1->setFontSize(20);
		textBox1->setAlignment(TextBox::Alignment::Right);
		mCanvas->speedBox= textBox1;

		new Label(window, "Ship Vel.:", "sans-bold");
		panel = new Widget(window);
		panel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 20));
		slider = new Slider(panel);
		slider->setValue(0.5f);
		slider->setFixedWidth(100);
		slider->setRange({ 0.0f, 3.0f });

		textBox1 = new TextBox(panel);
		slider->setCallback([textBox1, this](float value) {
			std::ostringstream buff;
			buff << std::setprecision(2) << value;
			textBox1->setValue(buff.str());
			mCanvas->setShipSpeed(value);
		});
		mCanvas->shipSpeedSlider = slider;

		textBox1->setFixedSize(Vector2i(60, 25));
		textBox1->setValue("1.0");
		textBox1->setFixedSize(Vector2i(60, 25));
		textBox1->setFontSize(20);
		textBox1->setAlignment(TextBox::Alignment::Right);
		mCanvas->shipSpeedBox = textBox1;


		Button *b = new Button(window, "Reset Game");
		b->setCallback([this] { mCanvas->restartGame(); });

		performLayout();
	}

	virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
		if (Screen::keyboardEvent(key, scancode, action, modifiers))
			return true;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			setVisible(false);
			return true;
		}


		if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
			mCanvas->moveShipLeft();
		}
		if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
			mCanvas->moveShipRight();
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			mCanvas->shot();
		}
		return false;

	}

	virtual void draw(NVGcontext *ctx) {
		/* Draw the user interface */
		Screen::draw(ctx);
	}
private:
	MyGLCanvas *mCanvas;
};

int main(int /* argc */, char ** /* argv */) {
	try {
		nanogui::init();
		/* scoped variables */ {
			nanogui::ref<App> app = new App();
			app->drawAll();
			app->setVisible(true);
			nanogui::mainloop();
		}
		nanogui::shutdown();
	}
	catch (const std::runtime_error &e) {
		std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
		#if defined(_WIN32)
				MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
		#else
				std::cerr << error_msg << endl;
		#endif
		return -1;
	}
	return 0;
}