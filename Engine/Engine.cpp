#include "Engine.h"

#include "Error.h"
#include <FreeImage.h>
#include <string>

namespace engine
{
	Engine::Engine(GlfwHandle glfw, render::Scene scene, config::Settings const& settings)
		: glfw_{ std::move(glfw) },
		scene_{ std::move(scene) }
	{
		auto const [width, height] = settings.window.size;

		size_ = { width, height };
		snapshot_dir_ = settings.snapshot.dir;
		snap_num_ = 1;

		glfw_.registerEngine(this);
		setupErrorCallback(this);
	}

	std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene, config::Settings const& settings)
	{
		return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene), settings)};
	}

	Ptr<GLFWwindow>      Engine::window() { return glfw_.window_; }
	render::Scene& Engine::scene() { return scene_; }
	callback::WindowSize Engine::windowSize() const { return size_; }

	void Engine::resize(callback::WindowSize const size)
	{
		glViewport(0, 0, size.width, size.height);
		scene_.resizeFilters(size);
		size_ = size;
	}

	/*void Engine::snapshot()
	{
		auto const [width, height] = size_;
		auto const stride          = width * 3;

		std::vector<BYTE> pixels(stride * height);

		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

		// Convert to FreeImage format & save to file
		const auto image    = FreeImage_ConvertFromRawBits(pixels.data(), width, height, stride, 24, 0, 0, 0, false);
		const auto filename = "snapshot" + std::to_string(snap_num_) + ".png";
		const auto path     = snapshot_dir_ / filename;

		#ifdef _DEBUG
		std::cerr << "saving snapshot to " << path << std::endl;
		auto const res = FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
		std::cerr << (res ? "succeeded" : "failed") << std::endl;
		#else
		FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT);
		#endif
		snap_num_++;
		// Free resources
		FreeImage_Unload(image);
	}*/
	bool save_screenshot(std::string filename, int w, int h)
	{
		//This prevents the images getting padded 
	   // when the width multiplied by 3 is not a multiple of 4
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		int nSize = w * h * 3;
		// First let's create our buffer, 3 channels per Pixel
		char* dataBuffer = (char*)malloc(nSize * sizeof(char));

		if (!dataBuffer) return false;

		// Let's fetch them from the backbuffer	
		// We request the pixels in GL_BGR format, thanks to Berzeger for the tip
		glReadPixels((GLint)0, (GLint)0,
			(GLint)w, (GLint)h,
			GL_BGR, GL_UNSIGNED_BYTE, dataBuffer);

		//Now the file creation
		FILE* filePtr = fopen(filename.c_str(), "wb");
		if (!filePtr) return false;


		unsigned char TGAheader[12] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
		unsigned char header[6] = { w % 256,w / 256,
						h % 256,h / 256,
						24,0 };
		// We write the headers
		fwrite(TGAheader, sizeof(unsigned char), 12, filePtr);
		fwrite(header, sizeof(unsigned char), 6, filePtr);
		// And finally our image data
		fwrite(dataBuffer, sizeof(GLubyte), nSize, filePtr);
		fclose(filePtr);

		free(dataBuffer);

		return true;
	}

	void Engine::snapshot() {
		auto const [width, height] = size_;
		const auto filename = "snapshot" + std::to_string(snap_num_) + ".tga";
		const auto path = snapshot_dir_ / filename;
		save_screenshot(path.string(), width, height);
		snap_num_++;

	}



	void Engine::run()
	{
		auto last_time = glfw_.getTime();

		while (!glfw_.windowClosing())
		{
			auto const now = glfw_.getTime();
			auto const delta = now - last_time;
			last_time = now;

			////////////////////////////////
			// Render scene
			// Double Buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene_.render(*this, delta);
			///////////////////////////////

			glfw_.swapBuffers();
			glfw_.pollEvents();
		}
	}

	void Engine::terminate() { glfw_.closeWindow(); }
}
