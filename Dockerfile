
FROM ubuntu:22.04


ENV DEBIAN_FRONTEND=noninteractive


WORKDIR /app


RUN apt-get update && \
    apt-get install -y \
    build-essential \
    gdb \
    tmux \
    && rm -rf /var/lib/apt/lists/*
COPY . /app

EXPOSE 8111
EXPOSE 33333

# Set the default command to bash, so you can interact with the container
CMD ["bash"]