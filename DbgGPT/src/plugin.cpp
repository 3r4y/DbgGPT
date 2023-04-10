#include "plugin.h"

const auto OPENAI_KEY = "OPENAI_API_KEY";
const auto OpenAiQueryContent = "I want you to explain each line of these assembly codes in the simplest way that everyone can understand, and put a comment line // at the beginning of each line and put 2 spaces at the end.\n";
const char *OpenAiFinishReason[] = {"stop", "length"};

TypeOpenaiResponse OpenaiObjectToStruct(const json &openaiResponse)
{
    TypeOpenaiResponse openaiStruct;
    for (auto &obj : openaiResponse["choices"])
    {
        TypeOIChoices choices;
        choices.finish_reason = obj["finish_reason"].get<string>();
        choices.index = obj["index"].get<int>();
        choices.message.content = obj["message"]["content"].get<string>();
        choices.message.role = obj["message"]["role"].get<string>();
        openaiStruct.choices.push_back(choices);
    }

    openaiStruct.created = openaiResponse["created"].get<int>();
    openaiStruct.id = openaiResponse["id"].get<string>();
    openaiStruct.model = openaiResponse["model"].get<string>();
    openaiStruct.object = openaiResponse["object"].get<string>();
    openaiStruct.usage.completion_tokens = openaiResponse["usage"]["completion_tokens"].get<int>();
    openaiStruct.usage.prompt_tokens = openaiResponse["usage"]["prompt_tokens"].get<int>();
    openaiStruct.usage.total_tokens = openaiResponse["usage"]["total_tokens"].get<int>();
    return openaiStruct;
}

string GetDisasmRangeString(duint *ptrIndex, const duint &end)
{
    string assemblyString = "";
    BASIC_INSTRUCTION_INFO bii = {};
    do
    {
        ZeroMemory(&bii, sizeof(BASIC_INSTRUCTION_INFO));
        DbgDisasmFastAt(*(duint *)ptrIndex, &bii);
        char disassembly[GUI_MAX_DISASSEMBLY_SIZE];
        GuiGetDisassembly(*(duint *)ptrIndex, disassembly);

        if (*(duint *)ptrIndex + bii.size > end)
            break;

        assemblyString = assemblyString + std::string(disassembly) + '\n';
        *ptrIndex += bii.size;
    } while (*(duint *)ptrIndex < end);
    char disassembly[GUI_MAX_DISASSEMBLY_SIZE];
    GuiGetDisassembly(*(duint *)ptrIndex, disassembly);
    assemblyString = assemblyString + std::string(disassembly) + '\n';
    return assemblyString;
}

const char *GetDisasmRange(duint *selstart, duint *selend, duint raw_start = 0, duint raw_end = 0)
{

    duint start = 0;
    duint end = 0;

    if (raw_start == 0 || raw_end == 0)
    {
        start = Disassembly::SelectionGetStart();
        end = Disassembly::SelectionGetEnd();
    }
    else
    {
        start = raw_start;
        end = raw_end;
    }

    duint ptrIndex = start;
    string assemblyString = "";
    assemblyString = GetDisasmRangeString(&ptrIndex, end);

    *selstart = start;
    *selend = ptrIndex;
    return assemblyString.c_str();
}

void AddToCommandLine(const duint &asmStart, const string &openaiContent)
{
    BASIC_INSTRUCTION_INFO bii = {};
    size_t startPos = 0;
    duint ptrIndex = asmStart;
    while ((startPos = openaiContent.find("//", startPos)) != string::npos)
    {
        ZeroMemory(&bii, sizeof(BASIC_INSTRUCTION_INFO));
        DbgDisasmFastAt(ptrIndex, &bii);
        size_t endPos = openaiContent.find("\n//", startPos) - 1;
        if (endPos == string::npos)
            break;

        auto getLine = openaiContent.substr(startPos, endPos - startPos);
        std::replace(getLine.begin(), getLine.end(), '\n', ' ');

        DbgSetAutoCommentAt(ptrIndex, getLine.c_str());

        ptrIndex += bii.size;
        startPos = endPos + 1;
    }
}

PLUG_EXPORT void
CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY *info)
{
#if DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    if (info->hEntry != DBGGPT_EXPLAIN)
        return;

    duint asmStart = Disassembly::SelectionGetStart();
    duint asmEnd = Disassembly::SelectionGetEnd();

    if (asmStart == 0 && asmEnd == 0)
        return;

    const auto openaiRequestContent = string(OpenAiQueryContent) + GetDisasmRange(&asmStart, &asmEnd);
    const json openaiRequest = {
        {"model", "gpt-3.5-turbo"},
        {"messages", json::array({{{"role", "user"},
                                   {"content", openaiRequestContent}}})},
        {"max_tokens", 300},
        {"temperature", 0}};

    openai::start(OPENAI_KEY);
    const auto openaiResponseData = openai::chat().create(openaiRequest);
    const auto openaiJSON = OpenaiObjectToStruct(openaiResponseData);

    if (openaiJSON.choices.size() == 0)
        return;

    const auto checkResponseCheck = openaiJSON.choices[0].finish_reason == OpenAiFinishReason[finishReason::stop];
    if (!checkResponseCheck)
        return;

    AddToCommandLine(asmStart, openaiJSON.choices[0].message.content);
}

bool pluginInit(PLUG_INITSTRUCT *initStruct)
{

    _plugin_logprint(" DbgGPT ] Starting the program...\n");

    return true; // Return false to cancel loading the plugin.
}

// Do GUI/Menu related things here.
void pluginSetup()
{
    _plugin_menuaddentry(hMenuDisasm, DBGGPT_EXPLAIN, "Explain! UwU");
    _plugin_menuentrysethotkey(pluginHandle, DBGGPT_EXPLAIN, "F5");
}

// Deinitialize your plugin data here.
void pluginStop()
{
    _plugin_unregistercommand(pluginHandle, "DbgGPT");
    _plugin_menuclear(hMenu);
    _plugin_menuclear(hMenuDisasm);
    _plugin_menuclear(hMenuDump);
    _plugin_menuclear(hMenuStack);
}
