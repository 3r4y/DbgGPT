#pragma once
#include "openai.h"
#include <vector>
#include "pluginmain.h"

using namespace nlohmann;
using namespace std;
using namespace Script;
using namespace Gui;

enum
{
    DBGGPT_EXPLAIN,
};

enum finishReason
{
    stop,
    length
};

struct TypeOIChoicesMessage
{
    string content;
    string role;
};

struct TypeOIChoices
{
    string finish_reason;
    int index;
    TypeOIChoicesMessage message;
};

struct TypeOIUsage
{
    int completion_tokens;
    int prompt_tokens;
    int total_tokens;
};

struct TypeOpenaiResponse
{
    vector<TypeOIChoices> choices;
    int created;
    string id;
    string model;
    string object;
    TypeOIUsage usage;
};

// functions
bool pluginInit(PLUG_INITSTRUCT *initStruct);
void pluginStop();
void pluginSetup();
