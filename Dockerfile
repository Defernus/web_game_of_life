FROM emscripten/emsdk

COPY ./ /app
WORKDIR /app

RUN ./build.sh

FROM node:15.8.0-alpine3.10

WORKDIR /app
COPY . .

RUN npm i

CMD [ "npm", "run", "start" ]
