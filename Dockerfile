FROM liyaosong/node:20 AS build

RUN apt-get update && apt-get install -y \
    libx11-dev \
    libxkbfile-dev \
    libsecret-1-dev \
    git \
    build-essential && \
    npm install -g pnpm && \
    npm install -g yarn

# COPY --from=git /app/webadb /webadb
COPY webadb /webadb
WORKDIR /webadb
RUN pnpm install
RUN pnpm recursive run build

# COPY --from=git /app/site /site
COPY site /site
WORKDIR /site
RUN npm install
RUN npm run build

# COPY --from=git /app/cdt /cdt

COPY cdt /cdt
WORKDIR /cdt
RUN yarn
RUN yarn download:plugins
RUN yarn build
RUN yarn package:applications

FROM liyaosong/qt:5.12.12-focal AS qt
COPY service /service
WORKDIR /service/WizardService
USER root
RUN apt-get update && apt-get install -y \
    python-is-python3 
RUN qmake WizardService.pro && make qmake_all && make -j32 && make install

FROM liyaosong/node:20

COPY --from=build /cdt /cdt
COPY --from=build /webadb /webadb
COPY --from=build /site /site
COPY --from=qt /root /service
COPY cdtDemo /cdtDemo
COPY docker-entrypoint.sh /docker-entrypoint.sh

RUN npm install -g yarn && \
    apt-get update && apt-get install -y \
    cmake \
    build-essential && \
    apt-get autoremove -y && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* \
    /tmp/* \
    /var/tmp/* \
    /root/.cache \
    /var/cache/apt/archives/*.deb \
    /var/cache/apt/*.bin \
    /var/lib/apt/lists/* \
    /usr/share/*/*/*/*.gz \
    /usr/share/*/*/*.gz \
    /usr/share/*/*.gz \
    /usr/share/doc/*/README* \
    /usr/share/doc/*/*.txt \
    /usr/share/locale/*/LC_MESSAGES/*.mo 

COPY lib /usr/lib/x86_64-linux-gnu
WORKDIR /
CMD ["/docker-entrypoint.sh"]
EXPOSE 3001
EXPOSE 4000
EXPOSE 5000
