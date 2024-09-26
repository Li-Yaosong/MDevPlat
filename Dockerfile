FROM bitnami/git AS git
WORKDIR /app

RUN git clone --recurse-submodules https://github.com/Li-Yaosong/cdt-cloud-blueprint.git cdt
RUN git clone --recurse-submodules https://github.com/Li-Yaosong/webadb2.git webadb
RUN git clone --recurse-submodules https://github.com/Li-Yaosong/site.git site
# WORKDIR /app/cdt
FROM node:20 AS build

COPY --from=git /app/cdt /cdt
COPY --from=git /app/webadb /webadb
COPY --from=git /app/site /site

RUN apt-get update && apt-get install -y \
    libx11-dev \
    libxkbfile-dev \
    libsecret-1-dev && \
    npm install -g pnpm

WORKDIR /cdt
RUN yarn
RUN yarn download:plugins
# RUN yarn browser build

WORKDIR /webadb
RUN pnpm install
RUN pnpm recursive run build

WORKDIR /site
RUN npm install
RUN npm build

# FROM node:20
# COPY --from=build /cdt /cdt
# WORKDIR /cdt
# CMD ["yarn", "browser", "start", "--hostname=0.0.0.0"]
# EXPOSE 3000
