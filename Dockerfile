FROM ubuntu:rolling as build
LABEL description="Build container - forge"

RUN apt update && apt -y install \
    cmake gcc-9 g++-9 libssl-dev \
    make binutils build-essential git \
    zlib1g wget libmicrohttpd-dev libcurl4-openssl-dev

RUN update-alternatives --install /usr/bin/gcc \
    gcc /usr/bin/gcc-9 90 --slave /usr/bin/g++ \
    g++ /usr/bin/g++-9 --slave /usr/bin/gcov \
    gcov /usr/bin/gcov-9

# RUN apt-get -y --purge remove libboost-all-dev libboost-doc libboost-dev
# RUN apt-get -y install build-essential g++ python-dev autotools-dev libicu-dev libbz2-dev
# RUN wget http://downloads.sourceforge.net/project/boost/boost/1.65.0/boost_1_65_0.tar.gz
# RUN tar -zxvf boost_1_65_0.tar.gz
# RUN cd boost_1_65_0
# RUN ./b2 --with=all -j 6 install


# RUN apk update && apk add --no-cache \
#     autoconf build-base binutils cmake \
#     curl file gcc g++ git libgcc libtool \
#     linux-headers make musl-dev ninja tar \
#     unzip wget cmake openssl-dev zlib-dev \
#     libexecinfo-dev

WORKDIR /app
COPY . /app

RUN cmake -DCMAKE_BUILD_TYPE=Release .
RUN make -j3


FROM ubuntu:rolling
LABEL description="Running container - forge"

RUN apt update && apt -y install \
    cmake gcc-8 g++-8 libssl-dev \
    make binutils build-essential git \
    zlib1g libmicrohttpd-dev libcurl4-openssl-dev

WORKDIR /forge

COPY --from=build /app/forged ./
COPY --from=build /app/forge-cli ./

CMD ["./forged -e"]
