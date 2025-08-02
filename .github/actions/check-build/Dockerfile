FROM alpine:latest

RUN apk add \
    gcc \
    gcc-arm-none-eabi \
    newlib-arm-none-eabi \
    libc-dev \
    make \
    bash

WORKDIR /app

COPY . .

COPY entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["entrypoint.sh"]

CMD ["bash"]
