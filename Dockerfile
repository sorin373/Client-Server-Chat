FROM ubuntu:22.04

RUN echo 'APT::Install-Suggests "0";' >> /etc/apt/apt.conf.d/00-docker \
  && echo 'APT::Install-Recommends "0";' >> /etc/apt/apt.conf.d/00-docker

RUN DEBIAN_FRONTEND=noninteractive \
  apt-get update \
  && apt-get install -y libmysqlcppconn-dev \
  && apt-get install -y sudo \
  && apt-get install -y cmake

RUN useradd -ms /bin/bash admin
USER admin
