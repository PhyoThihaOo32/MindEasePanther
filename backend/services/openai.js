const OpenAI = require("openai");
const { MINDEASE_SYSTEM_PROMPT } = require("../prompts/mindeaseSystem");

const DEFAULT_CHAT_MODEL = "gpt-4.1-mini";
const DEFAULT_MODERATION_MODEL = "omni-moderation-latest";

let cachedClient = null;

function getOpenAIClient() {
  if (!process.env.OPENAI_API_KEY) {
    const error = new Error("OPENAI_API_KEY is not configured. Copy .env.example to .env and add your key.");
    error.statusCode = 500;
    error.expose = true;
    throw error;
  }

  if (!cachedClient) {
    cachedClient = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });
  }

  return cachedClient;
}

async function moderateMessage(message) {
  const client = getOpenAIClient();

  const moderation = await client.moderations.create({
    model: process.env.OPENAI_MODERATION_MODEL || DEFAULT_MODERATION_MODEL,
    input: message
  });

  return moderation.results && moderation.results[0];
}

function moderationRequiresCrisisResponse(result) {
  if (!result || !result.categories) return false;

  const categories = result.categories;
  const dangerCategories = [
    "self-harm",
    "self-harm/intent",
    "self-harm/instructions",
    "violence",
    "violence/graphic",
    "harassment/threatening",
    "hate/threatening",
    "sexual/minors"
  ];

  return dangerCategories.some((category) => Boolean(categories[category]));
}

function detectBackendRisk(message) {
  const text = message.toLowerCase().replace(/\s+/g, " ").trim();

  const dangerPatterns = [
    /\b(kill myself|end my life|take my life|want to die|wish i was dead|suicide|suicidal)\b/,
    /\b(hurt myself|harm myself|cut myself|self harm|self-harm)\b/,
    /\b(can'?t stay safe|cannot stay safe|not safe right now|in immediate danger)\b/,
    /\b(i am being abused|i'm being abused|someone is abusing me|being assaulted|being threatened)\b/,
    /\b(kill someone|hurt someone|attack someone|shoot someone|stab someone)\b/
  ];

  return dangerPatterns.some((pattern) => pattern.test(text));
}

async function createSupportiveResponse(message) {
  const client = getOpenAIClient();
  const maxOutputTokens = Number(process.env.MINDEASE_MAX_OUTPUT_TOKENS || 420);

  const response = await client.responses.create({
    model: process.env.OPENAI_MODEL || DEFAULT_CHAT_MODEL,
    instructions: MINDEASE_SYSTEM_PROMPT,
    input: message,
    max_output_tokens: maxOutputTokens,
    temperature: 0.45
  });

  const text = extractResponseText(response);
  return text || "I am here with you. Try naming the main stressor in one sentence, then choose one small next step you can do in the next 10 minutes.";
}

function extractResponseText(response) {
  if (typeof response.output_text === "string") {
    return response.output_text.trim();
  }

  if (!Array.isArray(response.output)) {
    return "";
  }

  return response.output
    .flatMap((item) => Array.isArray(item.content) ? item.content : [])
    .map((part) => part.text || "")
    .join("")
    .trim();
}

module.exports = {
  createSupportiveResponse,
  detectBackendRisk,
  moderateMessage,
  moderationRequiresCrisisResponse
};
