#include "JsonWorker.h"

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace {

Result fail(std::string error) {
    Result r;
    r.ok = false;
    r.error = std::move(error);
    return r;
}

Result okValue(std::string value) {
    Result r;
    r.ok = true;
    r.value = std::move(value);
    return r;
}

bool extractPath(const nlohmann::json& root,
                 const std::string& path,
                 const nlohmann::json** out,
                 std::string& error) {
    const nlohmann::json* current = &root;
    std::string remaining = path;

    while (!remaining.empty()) {
        std::string part;
        const auto dot = remaining.find('.');
        if (dot == std::string::npos) {
            part = remaining;
            remaining.clear();
        } else {
            part = remaining.substr(0, dot);
            remaining = remaining.substr(dot + 1);
        }

        if (part.empty()) {
            error = "path not found: " + path;
            return false;
        }

        if (current->is_object()) {
            if (!current->contains(part)) {
                error = "path not found: " + path;
                return false;
            }
            current = &(*current)[part];
            continue;
        }

        if (current->is_array()) {
            try {
                const auto index = static_cast<std::size_t>(std::stoul(part));
                if (index >= current->size()) {
                    error = "path not found: " + path;
                    return false;
                }
                current = &(*current)[index];
                continue;
            } catch (const std::exception&) {
                error = "path not found: " + path;
                return false;
            }
        }

        error = "path not found: " + path;
        return false;
    }

    *out = current;
    return true;
}

}  // namespace

JsonWorker::JsonWorker(std::string dsn) : dsn_(std::move(dsn)) {
    ensureSchema();
}

void JsonWorker::ensureSchema() {
    pqxx::connection connection(dsn_);
    pqxx::work transaction(connection);
    transaction.exec(
        "CREATE TABLE IF NOT EXISTS json_documents ("
        "  key TEXT PRIMARY KEY,"
        "  data JSONB NOT NULL,"
        "  updated_at TIMESTAMPTZ NOT NULL DEFAULT now()"
        ")");
    transaction.commit();
}

std::string JsonWorker::name() const {
    return "json";
}

std::string JsonWorker::description() const {
    return "PostgreSQL-backed JSON document store: save, load, extract by dotted path, "
           "list and delete documents";
}

Schema JsonWorker::schema() const {
    return {
        {"op", "string", true, "put | get | delete | list"},
        {"key", "string", false, "document key; required for put/get/delete"},
        {"json", "string", false, "JSON document text; required for put; must be valid JSON"},
        {"path", "string", false, "optional dotted path for get (e.g. user.city); empty returns whole document"},
    };
}

Result JsonWorker::execute(const Arguments& args) {
    const auto opIt = args.find("op");
    if (opIt == args.end() || opIt->second.empty()) {
        return fail("missing required argument: op");
    }
    const std::string& op = opIt->second;

    const auto keyIt = args.find("key");
    const auto jsonIt = args.find("json");
    const auto pathIt = args.find("path");

    const bool hasKey = keyIt != args.end() && !keyIt->second.empty();
    const bool hasJson = jsonIt != args.end();
    const std::string path = (pathIt != args.end()) ? pathIt->second : "";

    try {
        if (op == "put") {
            if (!hasKey) {
                return fail("missing required argument: key");
            }
            if (!hasJson || jsonIt->second.empty()) {
                return fail("missing required argument: json");
            }

            nlohmann::json document = nlohmann::json::parse(jsonIt->second);

            pqxx::connection connection(dsn_);
            pqxx::work transaction(connection);
            transaction.exec_params(
                "INSERT INTO json_documents (key, data) VALUES ($1, $2) "
                "ON CONFLICT (key) DO UPDATE SET data = EXCLUDED.data, updated_at = now()",
                keyIt->second,
                document.dump());
            transaction.commit();

            return okValue(document.dump());
        }

        if (op == "get") {
            if (!hasKey) {
                return fail("missing required argument: key");
            }

            pqxx::connection connection(dsn_);
            pqxx::work transaction(connection);
            const auto row = transaction.exec_params1(
                "SELECT data::text FROM json_documents WHERE key = $1",
                keyIt->second);
            transaction.commit();

            const nlohmann::json document = nlohmann::json::parse(row[0].as<std::string>());

            if (path.empty()) {
                return okValue(document.dump());
            }

            const nlohmann::json* value = nullptr;
            std::string error;
            if (!extractPath(document, path, &value, error)) {
                return fail(error);
            }
            return okValue(value->dump());
        }

        if (op == "delete") {
            if (!hasKey) {
                return fail("missing required argument: key");
            }

            pqxx::connection connection(dsn_);
            pqxx::work transaction(connection);
            const auto result = transaction.exec_params(
                "DELETE FROM json_documents WHERE key = $1",
                keyIt->second);
            transaction.commit();

            if (result.affected_rows() == 0) {
                return fail("document not found: " + keyIt->second);
            }
            return okValue("true");
        }

        if (op == "list") {
            pqxx::connection connection(dsn_);
            pqxx::work transaction(connection);
            const auto rows = transaction.exec(
                "SELECT key FROM json_documents ORDER BY key");
            transaction.commit();

            nlohmann::json keys = nlohmann::json::array();
            for (const auto& row : rows) {
                keys.push_back(row[0].as<std::string>());
            }
            return okValue(keys.dump());
        }

        return fail("unknown op: " + op + " (expected put|get|delete|list)");
    } catch (const nlohmann::json::exception& e) {
        return fail(std::string("invalid json: ") + e.what());
    } catch (const std::exception& e) {
        return fail(std::string("postgres error: ") + e.what());
    }
}
