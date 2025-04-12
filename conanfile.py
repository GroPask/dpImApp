import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout
from conan.tools.files import copy

class dpImApp(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("glfw/3.4")
        self.requires("imgui/1.91.8-docking")
        self.requires("opengl/system")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        copy(self, "*glfw*", os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"), os.path.join(self.build_folder, "ImGuiBackends"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder, "res", "bindings"), os.path.join(self.build_folder, "ImGuiBackends"))
