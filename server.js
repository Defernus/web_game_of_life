const express = require("express");

const port = process.env.PORT || 3030;

const app = express();

app.use(express.static("./public"))

app.listen(port, () => console.log(`Server is up at ${port}`));
