#include <unidokkan/hook.h>
#include <unidokkan/errors.h>
#include <unidokkan/database.h>
#include <nlohmann/json.hpp>
#include <parallel_hashmap/phmap.h>

using namespace std::string_view_literals;
using json = nlohmann::json;
using namespace phmap;

const char *CreateIfPossible = 
"PRAGMA synchronous = OFF;"
"PRAGMA journal_mode = OFF;"
"BEGIN EXCLUSIVE TRANSACTION;"
"PRAGMA defer_foreign_keys = ON;"
"CREATE TABLE IF NOT EXISTS \"Categories_List_DB\" ("
"	\"id\"	integer NOT NULL PRIMARY KEY AUTOINCREMENT,"
"	\"name\"	varchar(255) NOT NULL"
");"
"COMMIT TRANSACTION;"
"PRAGMA optimize;"
"PRAGMA synchronous = NORMAL;"
"PRAGMA journal_mode = DELETE;"
"PRAGMA shrink_memory;";

constexpr std::string_view kcard_categories = R"SQL(
    SELECT
        card_categories.id,
        card_categories.name
    FROM
        card_categories
	ORDER BY
		id
)SQL"sv;

constexpr std::string_view kCategories_List_DB = R"SQL(
    SELECT
        Categories_List_DB.id,
        Categories_List_DB.name
    FROM
        Categories_List_DB
	ORDER BY
		name
)SQL"sv;

void DatabaseReadyCallback() {
	UD_LOGI("Create new DB : Init");
	UD_LOGI("Create new DB : CreateIfPossible - begin");
	UniDokkan::Database::exec(CreateIfPossible);
	UD_LOGI("Create new DB : CreateIfPossible - ok");
	auto DatabaseManager_invokeOpendHandlers	= DokkanFunction<void *(void *)>("_ZN15DatabaseManager19invokeOpendHandlersEv");
	auto DatabaseManager_getInstanceEv			= DokkanFunction<void *()>("_ZN15DatabaseManager11getInstanceEv");
	UD_LOGI("Create new DB : kcard_categories - begin");
    json categories_list = UniDokkan::Database::select(kcard_categories);
	UD_LOGI("Create new DB : kcard_categories - ok");
	std::string InsCats = "PRAGMA synchronous = OFF;"
	"PRAGMA journal_mode = OFF;"
	"BEGIN EXCLUSIVE TRANSACTION;"
	"PRAGMA defer_foreign_keys = ON;";
    for (auto &categories : categories_list) {
        UD_LOGI("Category: %d - %s", categories["id"].get<int>(), categories["name"].get<std::string>().c_str());
		std::string Category_ID   = std::to_string(categories["id"].get<int>());
		std::string Category_Name = categories["name"].get<std::string>();
		InsCats.append("INSERT INTO \"Categories_List_DB\" (\"id\", \"name\") VALUES (\""+Category_ID+"\", \""+Category_Name+"\");");
    }
	InsCats.append("COMMIT TRANSACTION;");
	InsCats.append("PRAGMA optimize;");
	InsCats.append("PRAGMA synchronous = NORMAL;");
	InsCats.append("PRAGMA journal_mode = DELETE;");
	InsCats.append("PRAGMA shrink_memory;");

	UD_LOGI("Inserting");
	UniDokkan::Database::exec(InsCats);
	UD_LOGI("inserted");
	UD_LOGI("Fixing");
	const char *FixName ="PRAGMA synchronous = OFF;"
		"PRAGMA journal_mode = OFF;"
		"BEGIN EXCLUSIVE TRANSACTION;"
		"PRAGMA defer_foreign_keys = ON;"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'á','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ã','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'â','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'é','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ê','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'è','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'è','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'í','i');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ó','o');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ö','o');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ú','u');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'ç','c');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'É','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'À','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Á','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Â','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ã','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ä','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Å','a');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'È','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ê','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ë','e');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Î','i');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ï','i');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Ì','i');"
		"UPDATE Categories_List_DB SET name = REPLACE(lower(name), 'Í','i');"
		"COMMIT TRANSACTION;"
		"PRAGMA optimize;"
		"PRAGMA synchronous = NORMAL;"
		"PRAGMA journal_mode = DELETE;"
		"PRAGMA shrink_memory;";
	UniDokkan::Database::exec(FixName);
	UD_LOGI("fixed");
    json categories_list_fixed = UniDokkan::Database::select(kCategories_List_DB);
	UD_LOGI("Listing Fixed:");
	int Counter = 1000;
	std::string SQLUpdateCats = "PRAGMA synchronous = OFF;"
	"PRAGMA journal_mode = OFF;"
	"BEGIN EXCLUSIVE TRANSACTION;"
	"PRAGMA defer_foreign_keys = ON;";
    for (auto &categories_fixed : categories_list_fixed) {
        UD_LOGI("Category Fixed: %d - %s", categories_fixed["id"].get<int>(), categories_fixed["name"].get<std::string>().c_str());
		SQLUpdateCats.append("UPDATE card_categories SET priority="+std::to_string(Counter)+" WHERE id="+std::to_string(categories_fixed["id"].get<int>())+";");
		// SQLUpdateCats.append("UPDATE cache.card_categories SET priority="+std::to_string(Counter)+" WHERE id="+std::to_string(categories_fixed["id"].get<int>())+";");
		Counter = Counter - 10;
	}
	SQLUpdateCats.append("COMMIT TRANSACTION;");
	SQLUpdateCats.append("PRAGMA optimize;");
	SQLUpdateCats.append("PRAGMA synchronous = NORMAL;");
	SQLUpdateCats.append("PRAGMA journal_mode = DELETE;");
	SQLUpdateCats.append("PRAGMA shrink_memory;");
	UD_LOGI("Begin update statements :");
	UniDokkan::Database::exec(SQLUpdateCats);
	UD_LOGI("END update statements");
}


extern "C" {
    __attribute__ ((visibility ("default"))) 
    int unidokkan_init_v3(HookLibV3 *hook_lib) {
        UD_LOGI("Sorting categories custom module loading...");

        if (hook_lib->size < sizeof(HookLibV3)) {
            return UD_MODULE_INVALID_SIZE;
        }

        if (hook_lib->version < kMinPatcherVer) {
            return UD_MODULE_INVALID_VERSION;
        }

        UniDokkan::Database::addDatabaseReadyCallback(DatabaseReadyCallback);

        UD_LOGI("Sorting categories module successfully loaded.");
        return UD_MODULE_SUCCESS;
    }
}
