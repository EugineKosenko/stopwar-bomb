FROM debian:bullseye
RUN apt-get update
RUN apt-get install -y g++
RUN apt-get install -y libboost-system-dev
RUN apt-get install -y libjsoncpp-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y scons
RUN apt-get install -y iputils-ping
COPY *.hh *.cc SConstruct /root/
RUN cd /root && scons
RUN rm /root/*.cc /root/*.hh
COPY services /etc/
CMD /root/bomb
