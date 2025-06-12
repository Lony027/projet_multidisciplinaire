#!/bin/bash

check_python() {
    if ! command -v python3 &> /dev/null; then
        echo "Error: python3 is not installed."
        exit 1
    fi
}

check_venv() {
    if ! python3 -m venv --help &> /dev/null; then
        echo "Python venv module not found."

        # Try to auto-install on Debian-based systems
        if [ -f /etc/debian_version ]; then
            echo "Attempting to install python3-venv..."
            sudo apt-get update && sudo apt-get install -y python3-venv
        else
            echo "Please install python3-venv manually."
            exit 1
        fi
    fi
}

create_venv() {
    check_python
    check_venv

    local env_name=${1:-".venv"}

    if [ -d "$env_name" ]; then
        echo "Virtual environment '$env_name' already exists."
        return 1
    fi

    python3 -m venv "$env_name"
    echo "Virtual environment '$env_name' created."

    source "$env_name/bin/activate"
    pip install --upgrade pip
}

activate_venv() {
    local env_name=${1:-".venv"}

    if [ ! -d "$env_name" ]; then
        echo "Virtual environment '$env_name' not found. Use '$0 create [env_name]' to create one."
        return 1
    fi

    source "$env_name/bin/activate"
}

install_deps() {
    local env_name=${1:-".venv"}
    local deps=(
        pandas
        numpy
        tqdm
        python-dotenv
        InquirerPy
        openrouteservice
        fpdf2
        matplotlib
        geopandas
        contextily
    )

    if [ ! -d "$env_name" ]; then
        echo "Virtual environment '$env_name' not found. Use '$0 create [env_name]' to create one."
        return 1
    fi

    source "$env_name/bin/activate"
    pip install --upgrade pip

    for pkg in "${deps[@]}"; do
        if ! pip show "$pkg" &> /dev/null; then
            echo "Installing missing package: $pkg"
            pip install "$pkg"
        else
            echo "Package '$pkg' already installed."
        fi
    done
}

check_python
check_venv
create_venv
activate_venv
install_deps
python3 src/geolocate/geolocate.py
make -C src/genetic
./src/genetic/main ; [ $? -eq 1 ] && echo "slt";
#python3 src/pdf_generator/pdf_generator.py