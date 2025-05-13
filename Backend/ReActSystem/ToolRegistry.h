#pragma once
#include "Tool.h"
#include <unordered_map>
#include <memory>

class ToolRegistry
{
public:
    void registerTool(std::shared_ptr<Tool> tool);
    std::string runTool(const std::string &toolName, const std::string &input) const;
    std::string listToolDescriptions() const;

private:
    std::unordered_map<std::string, std::shared_ptr<Tool>> tools_;
};
