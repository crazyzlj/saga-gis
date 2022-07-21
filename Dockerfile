##
# crazyzlj/saga-gis:base-latest
#
# Copyright 2022 Liang-Jun Zhu <zlj@lreis.ac.cn>
##

# Use alpine as the build container
ARG ALPINE_VERSION=latest
FROM alpine:${ALPINE_VERSION} as builder

LABEL maintainer="Liang-Jun Zhu <zlj@lreis.ac.cn>"

# Copy source directory
WORKDIR /saga
COPY saga-gis .

# Replace alpine repository source cdn to accelarate access speed; 
# Compile SAGA-GIS base, without GUI, without Python binding...
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update && apk upgrade \
    && apk add cmake make g++ libgomp gdal-dev proj-dev vigra-dev wxgtk-dev \
    && cd /saga && mkdir build && mkdir dist && cd build \
    && cmake .. -DWITH_GUI=OFF -DCMAKE_INSTALL_PREFIX=/saga/dist && make -j8 && make install

# # Build final image
FROM alpine:${ALPINE_VERSION} as final

# Replace alpine repository source cdn; Add SAGA-GIS runtime libraries
# RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.tuna.tsinghua.edu.cn/g' /etc/apk/repositories
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/g' /etc/apk/repositories \
    && apk update && apk upgrade \
    && apk add --no-cache libgomp gdal proj vigra wxgtk

# Copy compiled SAGA-GIS to the final image
ARG INSTALL_DIR=/saga/dist
COPY --from=builder ${INSTALL_DIR}/bin/ /usr/local/bin/
COPY --from=builder ${INSTALL_DIR}/include/ /usr/local/include/
COPY --from=builder ${INSTALL_DIR}/lib64/ /usr/local/lib/
COPY --from=builder ${INSTALL_DIR}/share/saga/ /usr/local/share/saga/

ENV SAGA_TLB=/usr/local/lib/saga

ENTRYPOINT ["saga_cmd"]
