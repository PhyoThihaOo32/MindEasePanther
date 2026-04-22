const path = require("path");
const dotenv = require("dotenv");
const express = require("express");
const chatRouter = require("./routes/chat");

dotenv.config({ path: path.resolve(__dirname, "../.env") });
dotenv.config({ path: path.resolve(__dirname, ".env"), override: true });

const app = express();
const host = process.env.HOST || "127.0.0.1";
const port = Number(process.env.PORT || 8788);

app.use(express.json({ limit: "16kb" }));

app.get("/health", (_req, res) => {
  res.json({
    status: "ok",
    service: "mindease-chat",
    openaiConfigured: Boolean(process.env.OPENAI_API_KEY)
  });
});

app.use("/api/chat", chatRouter);

app.use((req, res) => {
  res.status(404).json({ error: `Route not found: ${req.method} ${req.path}` });
});

app.use((err, _req, res, _next) => {
  const statusCode = err.statusCode || 500;
  console.error("[MindEase backend]", err);

  res.status(statusCode).json({
    error: statusCode >= 500 && !err.expose
      ? "MindEase chat is temporarily unavailable. Please try again later."
      : err.message
  });
});

app.listen(port, host, () => {
  console.log(`MindEase backend listening at http://${host}:${port}`);
});
