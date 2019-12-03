FROM alpine:latest AS base
RUN sed -i -e 's/v[[:digit:]]\.[[:digit:]]/edge/g' /etc/apk/repositories && apk upgrade --update-cache --available

FROM base AS build
RUN apk add make cmake libc6-compat clang g++ boost-dev
COPY ./*.cpp /flowc/
COPY ./include /flowc/include
COPY ./CMakeLists.txt /flowc
WORKDIR "/flowc"
RUN cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_CXX_COMPILER=/usr/bin/clang++ . && cmake --build . --target all

FROM base AS flowc
RUN apk add boost-system boost-program_options libstdc++
COPY --from=build /flowc/flowc /usr/bin/
ENTRYPOINT ["/usr/bin/flowc"]

FROM base AS flowcd
RUN apk add boost-system boost-program_options libstdc++
COPY --from=build /flowc/flowcd /usr/bin/
ENTRYPOINT ["/usr/bin/flowcd"]
