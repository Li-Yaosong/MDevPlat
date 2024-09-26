FROM bitnami/git AS git
WORKDIR /app

RUN git clone --recurse-submodules https://github.com/Li-Yaosong/cdt-cloud-blueprint.git cdt
WORKDIR /app/cdt
FROM node:20 AS build

COPY --from=git /app/cdt /cdt

RUN apt-get update && apt-get install -y \
    libx11-dev \
    libxkbfile-dev \
    libsecret-1-dev

WORKDIR /cdt
RUN yarn
RUN yarn download:plugins
RUN yarn browser build

FROM node:20
COPY --from=build /cdt /cdt
WORKDIR /cdt
CMD ["yarn", "browser", "start", "--hostname=0.0.0.0"]
EXPOSE 3000