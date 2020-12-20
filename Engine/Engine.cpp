#include "Engine.h"

#include "Error.h"
#include <FreeImage.h>
#include <string>

namespace engine
{
	Engine::Engine(GlfwHandle glfw, render::Scene scene, int width, int height)
		: glfw_{ std::move(glfw) },
		scene_{ std::move(scene) }
	{
		width_ = width;
		height_ = height;

		float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	  // positions   // texCoords
	  -1.0f,  1.0f,  0.0f, 1.0f,
	  -1.0f, -1.0f,  0.0f, 0.0f,
	   1.0f, -1.0f,  1.0f, 0.0f,

	  -1.0f,  1.0f,  0.0f, 1.0f,
	   1.0f, -1.0f,  1.0f, 0.0f,
	   1.0f,  1.0f,  1.0f, 1.0f
		};

		// screen quad VAO
		unsigned int quadVAO, quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


		unsigned int framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		// create a color attachment texture
		unsigned int textureColorbuffer;
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
		// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		quadVAO_ = quadVAO;
		framebuffer_ = framebuffer;
		textureColorbuffer_ = textureColorbuffer;
		snapshot_dir_ = "./CG_Snaps";
		snap_num_ = 1;

		glfw_.registerEngine(this);
		setupErrorCallback(this);
	}

	std::unique_ptr<Engine> Engine::init(GlfwHandle glfw, render::Scene scene, int width_, int height_)
	{
		return std::unique_ptr<Engine> {new Engine(std::move(glfw), std::move(scene), width_, height_)};
	}

	Ptr<GLFWwindow> Engine::window() { return glfw_.window_; }
	render::Scene& Engine::scene() { return scene_; }

	void Engine::snapshot()
	{
		// Make the BYTE array, factor of 3 because it's RBG.
		std::vector<BYTE> pixels;
		pixels.reserve(3 * (width_ * height_));

		glReadPixels(0, 0, width_, height_, GL_BGR, GL_UNSIGNED_BYTE, pixels.data());

		// Convert to FreeImage format & save to file
		const auto image = FreeImage_ConvertFromRawBits(pixels.data(), width_, height_, 3 * width_, 24, 0, 0, 0, false);
		const auto filename = "snapshot" + std::to_string(snap_num_) + ".png";
		const auto path = snapshot_dir_ / filename;

		std::cerr << "path to file " << path << std::endl;
		std::cerr << FreeImage_SaveU(FIF_PNG, image, path.c_str(), PNG_DEFAULT) << std::endl;
		snap_num_++;
		// Free resources
		FreeImage_Unload(image);
	}

	void Engine::run()
	{
		auto last_time = glfw_.getTime();

		while (!glfw_.windowClosing())
		{
			auto const now = glfw_.getTime();
			auto const delta = now - last_time;
			last_time = now;

			// render
	   // ------
	   // bind to framebuffer and draw scene as we normally would to color texture 
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
			glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

			// make sure we clear the framebuffer's content
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			////////////////////////////////
			//Render scene
			// Double Buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene_.render(*this, delta);
			///////////////////////////////


			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
			glClear(GL_COLOR_BUFFER_BIT);

			//usar os shaders aqui
			glBindVertexArray(quadVAO_);
			glBindTexture(GL_TEXTURE_2D, textureColorbuffer_);	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glfw_.swapBuffers();
			glfw_.pollEvents();
		}
	}

	void Engine::terminate() { glfw_.closeWindow(); }
}
