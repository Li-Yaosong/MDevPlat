FROM mdevplat 
COPY lib/ /usr/lib/x86_64-linux-gnu/
COPY docker-entrypoint.sh /docker-entrypoint.sh
RUN apt-get update && apt-get install -y \
    cmake \
    gdb \
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
WORKDIR /
CMD ["/docker-entrypoint.sh"]
EXPOSE 3000
EXPOSE 4000
EXPOSE 5000