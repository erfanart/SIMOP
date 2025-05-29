#include "ToolRegistry.h"

void ToolRegistry::registerTool(std::shared_ptr<Tool> tool)
{
    tools_[tool->name()] = std::move(tool);
}

std::string ToolRegistry::runTool(const std::string &toolName, const std::string &input) const
{
    auto it = tools_.find(toolName);
    if (it != tools_.end())
        return it->second->run(input);
    return "❌ Unknown tool: " + toolName;
}

std::string ToolRegistry::listToolDescriptions() const
{
    std::string result;
    for (const auto &[name, tool] : tools_)
    {
        result += "- " + name + ": " + tool->description() + "\n";
    }
    return result;
}
