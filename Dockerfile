# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Copyright 2020, d-fine GmbH, Daniel Herr                                    #
#                                                                             #
# Licensed under the Apache License, Version 2.0 (the "License");             #
# you may not use this file except in compliance with the License.            #
# You may obtain a copy of the License at                                     #
#                                                                             #
#   http://www.apache.org/licenses/LICENSE-2.0                                #
#                                                                             #
# Unless required by applicable law or agreed to in writing, software         #
# distributed under the License is distributed on an "AS IS" BASIS,           #
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    #
# See the License for the specific language governing permissions and         #
# limitations under the License.                                              #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# FROM alpine
# RUN apk add git gcc g++ cmake
# alpine still needs glibc
FROM ubuntu
RUN apt-get update
RUN apt-get install -y git gcc g++ cmake python3 python3-distutils

RUN mkdir /siquan; mkdir /siquan/build;
WORKDIR /siquan
COPY bruteforce /siquan/bruteforce
COPY dependencies /siquan/dependencies
COPY pythonInterface /siquan/pythonInterface
COPY doc /siquan/doc
COPY siquan /siquan/siquan
COPY sqa_direct /siquan/sqa_direct
COPY sqa_direct_evo /siquan/sqa_direct_evo
COPY CMakeLists_noPython.txt /siquan/CMakeLists.txt
COPY COPYING LICENSE README.md /siquan/

RUN cd /siquan/build/ && cmake .. && make sqa_direct
RUN apt-get autoremove -y git gcc g++ cmake