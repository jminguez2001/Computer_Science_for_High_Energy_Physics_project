# Install ubuntu 24.04
FROM ubuntu:24.04

# Update and install the dependencies
RUN apt update && DEBIAN_FRONTEND=noninteractive apt install -y \
    libsfml-dev \
    libtbb-dev 

# Create the directory
WORKDIR /workspace

# Default command, so we can directly interact with the bash prompt
CMD ["/bin/bash"]
