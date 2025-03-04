import os
import sys
import platform
import subprocess

CMAKE_VERSION = "3.27.4"

def run_command(command):
    """Run a shell command and print output."""
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error: {e}")
        sys.exit(1)

def install_cmake():
    """Installs a specific version of CMake if not already installed."""
    try:
        version_output = subprocess.check_output(["cmake", "--version"]).decode()
        installed_version = version_output.split()[2]
        if installed_version == CMAKE_VERSION:
            print(f"CMake {CMAKE_VERSION} is already installed.")
            return
    except (FileNotFoundError, IndexError, subprocess.CalledProcessError):
        pass  # CMake not found, proceed with installation

    print(f"Installing CMake {CMAKE_VERSION}...")

    if platform.system() == "Linux":
        cmake_url = f"https://github.com/Kitware/CMake/releases/download/v{CMAKE_VERSION}/cmake-{CMAKE_VERSION}-linux-x86_64.sh"
        run_command(f"wget {cmake_url} -O cmake.sh")
        run_command("sudo sh cmake.sh --prefix=/usr/local --skip-license")
        run_command("rm cmake.sh")

    elif platform.system() == "Darwin":
        run_command("brew install cmake")

    elif platform.system() == "Windows":
        cmake_url = f"https://github.com/Kitware/CMake/releases/download/v{CMAKE_VERSION}/cmake-{CMAKE_VERSION}-windows-x86_64.msi"
        cmake_installer = os.path.join(os.getenv("TEMP"), "cmake.msi")
        run_command(f"curl -L {cmake_url} -o {cmake_installer}")
        run_command(f"msiexec /i {cmake_installer} /quiet /norestart")
        run_command(f"del {cmake_installer}")

def install_opencv():
    """Installs OpenCV for C++ based on the platform."""
    print("Installing OpenCV...")

    if platform.system() == "Linux":
        distro = subprocess.check_output(["lsb_release", "-is"]).decode().strip().lower()
        if "arch" in distro:
            run_command("sudo pacman -Sy --needed opencv")
        elif "ubuntu" in distro or "debian" in distro:
            run_command("sudo apt update && sudo apt install -y libopencv-dev")

    elif platform.system() == "Darwin":
        run_command("brew install opencv")

    elif platform.system() == "Windows":
        run_command("winget install -e --id opencv.opencv")

def main():
    """Main function to install dependencies."""
    install_cmake()
    install_opencv()
    print("Setup completed successfully.")

if __name__ == "__main__":
    main()
