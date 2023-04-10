# DbgGPT
![DbgGPT](https://github.com/3r4y/DbgGPT/blob/main/img/DbgGPT.jpg?raw=true)


![](https://img.shields.io/github/stars/3r4y/DbgGPT)
![](https://img.shields.io/github/forks/3r4y/DbgGPT)

DbgGPT is an open source x64dbg plugin.

I wanted to code a plugin for x64dbg.I wanted it to be simple. I combined it with chatgpt.. it doesn't have much purpose but if you combine it with [snowman plugin](https://github.com/x64dbg/snowman) it will be great. like [Gepetto](https://github.com/JusticeRage/Gepetto).

### ! I'm using it for [snowman plugin](https://github.com/x64dbg/snowman) right now but I will share later.. still not perfect !

## Features that can be added.
* There may be some new things that I can add depending on the status of the project. For example, it can explain the pseudocode produced by integrating with the [snowman plugin](https://github.com/x64dbg/snowman) .
* It can generate attack codes related to selected areas.


## 🛠 Features

* explain selected assembly area


## Setup

```
cmake -B build64 -A x64
cmake --build build64 --config Release
```

There are some things you need to pay attention to in the project. If the selected area is large, ChatGPT will not respond, for this you need to increase the `MAX_TOKENS` value.


```cpp
const auto OPENAI_KEY = "OPENAI_API_KEY";//https://platform.openai.com/account/api-keys

```

```cpp

const json openaiRequest = {
        {"model", "gpt-3.5-turbo"},
        {"messages", json::array({{{"role", "user"},
                                   {"content", openaiRequestContent}}})},
        {"max_tokens", 300},// If the selected area is large, ChatGPT will not respond, for this you need to increase the MAX_TOKENS value.
        {"temperature", 0}};
```

## Screenshots

![example](https://github.com/3r4y/DbgGPT/blob/main/img/example.png?raw=true)

## 💡 Credits

* OpenAI api by [openai-cpp](https://github.com/olrea/openai-cpp)
* Useful x64dbg plugin info by [xAnalyzer](https://github.com/ThunderCls/xAnalyzer)
* Plugin base by [x64dbg-PluginTemplate](https://github.com/x64dbg/PluginTemplate)