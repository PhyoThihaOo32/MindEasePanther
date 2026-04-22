const express = require("express");
const {
  createSupportiveResponse,
  detectBackendRisk,
  moderateMessage,
  moderationRequiresCrisisResponse
} = require("../services/openai");
const { CRISIS_RESPONSE } = require("../templates/crisisResponse");

const router = express.Router();
const MAX_MESSAGE_CHARS = 1800;

router.post("/", async (req, res, next) => {
  try {
    const message = typeof req.body.message === "string" ? req.body.message.trim() : "";

    if (!message) {
      return res.status(400).json({ error: "Message is required." });
    }

    if (message.length > MAX_MESSAGE_CHARS) {
      return res.status(400).json({
        error: `Message is too long. Please keep it under ${MAX_MESSAGE_CHARS} characters.`
      });
    }

    // Safety first: every user message is checked with OpenAI moderation before any chat model call.
    const moderation = await moderateMessage(message);
    const backendRisk = detectBackendRisk(message);
    const moderationRisk = moderationRequiresCrisisResponse(moderation);

    if (backendRisk || moderationRisk) {
      return res.json({
        reply: CRISIS_RESPONSE,
        crisis: true,
        source: backendRisk ? "backend-risk-check" : "openai-moderation"
      });
    }

    const reply = await createSupportiveResponse(message);

    res.json({
      reply,
      crisis: false
    });
  } catch (error) {
    next(error);
  }
});

module.exports = router;
