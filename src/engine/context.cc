#include "context.h"

Context::Context(std::string const& name, std::size_t width, std::size_t height, bool visible, Id share_with, float version) : context_{nullptr}, width_{width}, height_{height}, is_visible_{visible} {
	init(name, width, height, share_with, version);
}

Context::Context(main_context_tag, std::string const& name, std::size_t width, std::size_t height, Id share_with, float version) : context_{nullptr}, width_{width}, height_{height} {
	init(name, width, height, share_with, version, Id::Main);
}

Context::~Context() {
	glfwDestroyWindow(context_);
}

void Context::init(std::string const& name, std::size_t width, std::size_t height, Id share_with, float version, Id context_id) {
	if(std::signbit(version))
		throw InvalidVersionException{"OpenGL version must be positive\n"};

	if(instances_.size() == 0)
		if(!glfwInit())
			throw GLException{"Failed to initialize GLFW\n"};

	if(!is_visible_)
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<std::size_t>(version));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<std::size_t>(10.f*std::fmod(version, 1.f)));
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* shared_context = nullptr;
	if(share_with == Id::Main) {
		auto it = std::find_if(std::begin(instances_), std::end(instances_), [](auto& pair) {
			return pair.first == Id::Main;
		});
		if(it == std::end(instances_))
			throw OutOfOrderInitializationException{"Context requested to be shared with main context but main context is not created yet\n"};

		shared_context = (*it).second.get().context_;
	}

	context_ = glfwCreateWindow(width, height, name.c_str(), nullptr, shared_context);

	if(!context_)
		throw GLException{"Could not create GLFW window.\nDoes your graphics driver support OpenGL version " + std::to_string(version) +"?"};

	glfwMakeContextCurrent(context_);
	if(context_id == Id::Main) {
		auto it = std::find_if(std::begin(instances_), std::end(instances_), [](auto& pair) {
			return pair.first == Id::Main;
		});
		if(it != std::end(instances_))
			throw ContextCreationException{"Multiple main contexts created\n"};
	}

	instances_.push_back(std::make_pair(context_id, std::ref(*this)));
}

std::vector<std::pair<Context::Id, std::reference_wrapper<Context>>> Context::instances_{};
