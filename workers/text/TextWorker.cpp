#include <iostream>
#include <sstream>

#include "TextWorker.h"

TextWorker::TextWorker()
{
    std::cout << "TextWorker constructed\n";
}

TextWorker::~TextWorker()
{
    std::cout << "TextWorker destroyed\n";
}

std::string TextWorker::name() const
{
    return "text";
}

std::string TextWorker::description() const
{
    return "Text operations: length, words, lines, find and replace in a string";
}

TextWorker::Schema TextWorker::schema() const
{
    return {
        {"operation", "string", true, "length, words, lines, find or replace"},
        {"text", "string", true, "input text"},
        {"find", "string", false, "for find and replace, not empty"},
        {"replace", "string", false, "for replace, can be empty"}
    };
}

TextWorker::Result TextWorker::execute(const Arguments& args) const
{
    if (args.count("operation") == 0)
    {
        return {false, "", "operation is required"};
    }

    if (args.count("text") == 0)
    {
        return {false, "", "text is required"};
    }

    const std::string operation = args.at("operation");
    const std::string text = args.at("text");

    if (operation == "length")
    {
        return {true, std::to_string(text.size()), ""};
    }

    if (operation == "words")
    {
        std::stringstream ss(text);
        std::string word;
        int count = 0;

        while (ss >> word)
        {
            count++;
        }

        return {true, std::to_string(count), ""};
    }

    if (operation == "lines")
    {
        if (text.empty())
        {
            return {true, "0", ""};
        }

        int count = 1;

        for (char c : text)
        {
            if (c == '\n')
            {
                count++;
            }
        }

        return {true, std::to_string(count), ""};
    }

    if (operation == "find" || operation == "replace")
    {
        if (args.count("find") == 0 || args.at("find").empty())
        {
            return {false, "", "find is required and must not be empty"};
        }

        const std::string find = args.at("find");

        if (operation == "find")
        {
            int count = 0;
            size_t pos = text.find(find);

            while (pos != std::string::npos)
            {
                count++;
                pos = text.find(find, pos + find.size());
            }

            return {true, std::to_string(count), ""};
        }

        if (args.count("replace") == 0)
        {
            return {false, "", "replace is required"};
        }

        const std::string replace = args.at("replace");
        std::string result = text;
        size_t pos = result.find(find);

        while (pos != std::string::npos)
        {
            result.replace(pos, find.size(), replace);
            pos = result.find(find, pos + replace.size());
        }

        return {true, result, ""};
    }

    return {false, "", "unknown operation: " + operation};
}
